#ifndef WIN32
#include "explorer_unix.h"

int
explorer_read(void* p) {
    WindowData* data = (WindowData*)p;
    Explorer* e = &data->explorer;
    char* joined_str = malloc(sizeof(char) * EXPLORER_BUFFER_LEN);
    while (e->running) {
        memset(joined_str, 0, EXPLORER_BUFFER_LEN);
        read(e->fd_output, joined_str, EXPLORER_BUFFER_LEN);
        explorer_parse_str(e, joined_str);
        push_user_event();
    }
    free(joined_str);
    return 1;
}

int
explorer_write(void* p) {
    WindowData* data = (WindowData*)p;
    Explorer* e = &data->explorer;

    char fen[FEN_LEN];
    char cmd[100];
    cmd[0] = '\0';
    fen[0] = '\0';
    board_fen_export(&game_move_get(&data->game)->board, e->fen);

    e->running = 1;
    SDL_Thread* thread = SDL_CreateThread(explorer_read, NULL, (void*)data);
    SDL_DetachThread(thread);
    while (e->running) {
        if (strcmp(fen, e->fen) && e->fen_changed) {
            e->fen_changed = 0;
            snprintf(fen, FEN_LEN, "fen %s\n", e->fen);
            write(e->fd_input, fen, strlen(fen) + 1);
        } else if (e->event > -1) {
            snprintf(cmd, 100, "event %d\n", e->event);
            e->event = -1;
            write(e->fd_input, cmd, strlen(cmd) + 1);
        }
        msleep(400);
    }
    return 1;
}

int
explorer_start(WindowData* data, int index) {
    Explorer* e = &data->explorer;
    if (e->running) {
        return 0;
    }

    machine_config_free(data);
    machine_config_load(data);

    if (data->conf.explorer_exe_list[index] == NULL) {
        return 0;
    }

    e->fen_changed = 1;

    char* argv[] = {data->conf.explorer_exe_list[index], NULL};
    int pipe_in[2], pipe_out[2];
    pipe(pipe_in);
    pipe(pipe_out);
    e->fd_input = pipe_in[1];
    e->fd_output = pipe_out[0];

    signal(SIGCHLD, SIG_IGN); //kernel will clear child process after its exit
    e->pid = fork();
    if (e->pid == 0) {
        close(0);
        dup(pipe_in[0]);
        close(1);
        close(2);
        dup(pipe_out[1]);
        execvp(data->conf.explorer_exe_list[index], argv);
        exit(0);
    } else {
        SDL_Thread* thread = SDL_CreateThread(explorer_write, NULL, (void*)data);
        SDL_DetachThread(thread);
        return 1;
    }
}

void
explorer_stop(WindowData* data) {
    Explorer* e = &data->explorer;
    if (e->running) {
        explorer_row_free(e);
        kill(e->pid, SIGTERM);
    }
    e->running = 0;
}

#endif
