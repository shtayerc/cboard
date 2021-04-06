#ifdef _WIN32
#include "machine_win.h"

int
machine_read(void *data)
{
    MachineData *md = (MachineData*)data;
    Machine *mc = &machine_list[md->index];
    DWORD len;

    while(mc->running){
        ReadFile(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN, &len, NULL);
        if(strstr(mc->output, "multipv") == NULL)
            continue;
        machine_line_parse(md->index);
        push_user_event(md->index);
    }
    return 1;
}

int
machine_write(void *data)
{
    MachineData *md = (MachineData*)data;
    Machine *mc = &machine_list[md->index];
    char **uci_list = md->data->conf.machine_uci_list[md->index];
    char fen[FEN_LEN];
    int i;
    DWORD len;
    fen[0] = '\0';

    WriteFile(mc->fd_input, "uci\n", 4, &len, NULL);
    ReadFile(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN, &len, NULL);
    while(strstr(mc->output, "uciok") == NULL){
        ReadFile(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN, &len, NULL);
    }

    WriteFile(mc->fd_input, "isready\n", 8, &len, NULL);
    ReadFile(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN, &len, NULL);
    while(strstr(mc->output, "readyok") == NULL){
        ReadFile(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN, &len, NULL);
    }

    if(uci_list != NULL){
        for(i = 0; uci_list[i] != NULL; i++){
            WriteFile(mc->fd_input, uci_list[i], strlen(uci_list[i]), &len,
                    NULL);
            WriteFile(mc->fd_input, "\n", 1, &len, NULL);
        }
    }

    board_fen_import(&mc->board, mc->fen);
    machine_line_init(mc, &mc->board);
    mc->running = 1;
    SDL_Thread * thread = SDL_CreateThread(machine_read, NULL,
            (void*)md);
    SDL_DetachThread(thread);
    while(mc->running){
        if(strcmp(fen, mc->fen) && mc->fen_changed){
            mc->fen_changed = 0;
            snprintf(fen, FEN_LEN, "%s", mc->fen);
            WriteFile(mc->fd_input, "stop\n", 5, &len, NULL);
            board_fen_import(&mc->board, mc->fen);
            WriteFile(mc->fd_input, "position fen ", 13, &len, NULL);
            WriteFile(mc->fd_input, fen, strlen(fen), &len, NULL);
            WriteFile(mc->fd_input, "\ngo infinite\n", 13, &len, NULL);
        }
        Sleep(400);
    }
    return 1;
}

void
machine_start(WindowData *data, int index)
{
    Machine *mc = &machine_list[index];
    if(mc->running)
        return;
    mc->fen_changed = 1;
    machine_config_free(data);
    machine_config_load(data);
    machine_set_line_count(data, index);
    machine_resize(data, index);

    HANDLE pipe_in;
    HANDLE pipe_out;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, 1 };
    CreatePipe(&mc->fd_output, &pipe_out, &sa, 0);
    CreatePipe(&pipe_in, &mc->fd_input, &sa, 0);

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdOutput = pipe_out;
    si.hStdInput = pipe_in;
    si.dwFlags |= STARTF_USESTDHANDLES;

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    DuplicateHandle(GetCurrentProcess(),
        mc->fd_output,
        GetCurrentProcess(),
        NULL,
        0,
        0,
        DUPLICATE_SAME_ACCESS);

    DuplicateHandle(GetCurrentProcess(),
        mc->fd_input,
        GetCurrentProcess(),
        NULL,
        0,
        0,
        DUPLICATE_SAME_ACCESS);

    CreateProcess(NULL,
            data->conf.machine_cmd_list[index][0],
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
    mc->pid = pi.hProcess;

    MachineData *md = &mc->md;
    md->index = index;
    md->data = data;
    SDL_Thread * thread = SDL_CreateThread(machine_write, NULL,
            (void*)md);
    SDL_DetachThread(thread);
}

void
machine_stop(int index)
{
    Machine *mc = &machine_list[index];
    if(mc->running){
        machine_line_free(mc);
        TerminateProcess(mc->pid, 0);
    }
    mc->running = 0;
}

#endif
