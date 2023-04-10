#ifdef _WIN32
#include "explorer_win.h"

int
explorer_read(void *p)
{
    WindowData *data = (WindowData*)p;
    Explorer *e = &data->explorer;
    char *joined_str = malloc(sizeof(char) * EXPLORER_BUFFER_LEN);
    DWORD len;

    while(e->running){
        memset(joined_str, 0, EXPLORER_BUFFER_LEN);
        ReadFile(e->fd_output, joined_str, MACHINE_OUTPUT_LEN, &len, NULL);
        explorer_parse_str(e, joined_str);
        push_user_event();
    }
    free(joined_str);
    return 1;
}

int
explorer_write(void *p)
{
    WindowData *data = (WindowData*)p;
    Explorer *e = &data->explorer;

    char fen[FEN_LEN];
    char cmd[100];
    cmd[0] = '\0';
    fen[0] = '\0';
    board_fen_export(&game_move_get(&data->game)->board, e->fen);

    e->running = 1;
    SDL_Thread * thread = SDL_CreateThread(explorer_read, NULL, (void*)data);
    SDL_DetachThread(thread);

    DWORD len;

    while(e->running){
        if(strcmp(fen, e->fen) && e->fen_changed){
            e->fen_changed = 0;
            snprintf(fen, FEN_LEN, "fen %s\n", e->fen);
            WriteFile(e->fd_input, fen, strlen(fen)+1, &len, NULL);
        }else if(e->event > -1){
            snprintf(cmd, 100, "event %d\n", e->event);
            e->event = -1;
            WriteFile(e->fd_input, cmd, strlen(cmd)+1, &len, NULL);
        }
        Sleep(400);
    }
    return 1;
}

int
explorer_start(WindowData *data)
{
    Explorer *e = &data->explorer;
    if(e->running)
        return 0;

    machine_config_free(data);
    machine_config_load(data);

    if(data->conf.explorer_exe == NULL)
        return 0;

    e->fen_changed = 1;

    HANDLE pipe_in;
    HANDLE pipe_out;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, 1 };
    CreatePipe(&e->fd_output, &pipe_out, &sa, 0);
    CreatePipe(&pipe_in, &e->fd_input, &sa, 0);

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdOutput = pipe_out;
    si.hStdInput = pipe_in;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    DuplicateHandle(GetCurrentProcess(),
        e->fd_output,
        GetCurrentProcess(),
        NULL,
        0,
        0,
        DUPLICATE_SAME_ACCESS);

    DuplicateHandle(GetCurrentProcess(),
        e->fd_input,
        GetCurrentProcess(),
        NULL,
        0,
        0,
        DUPLICATE_SAME_ACCESS);

    CreateProcess(NULL,
            data->conf.explorer_exe,
            NULL,
            NULL,
            1,
            CREATE_NEW_PROCESS_GROUP | CREATE_NO_WINDOW,
            NULL,
            NULL,
            &si,
            &pi);

    CloseHandle(&pipe_out);
    CloseHandle(&pipe_in);
    CloseHandle(&pi.hThread);
    e->pid = pi.hProcess;

    SDL_Thread * thread = SDL_CreateThread(machine_write, NULL,
            (void*)data);
    SDL_DetachThread(thread);
    return 1;
}

void
explorer_stop(WindowData *data)
{
    Explorer *e = &data->explorer;
    if(e->running){
        explorer_row_free(e);
        TerminateProcess(e->pid, 0);
    }
    e->running = 0;
}

#endif
