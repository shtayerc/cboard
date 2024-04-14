#ifndef WIN32
#include "machine_unix.h"

int
machine_read(void* p) {
    MachineData* md = (MachineData*)p;
    WindowData* data = (WindowData*)md->data;
    Machine* mc = data->machine_list[md->index];

    while (mc->running) {
        read(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN);
        if (strstr(mc->output, "multipv") == NULL) {
            continue;
        }
        machine_line_parse(data, md->index);
        push_user_event();
    }
    return 1;
}

int
machine_write(void* p) {
    MachineData* md = (MachineData*)p;
    WindowData* data = (WindowData*)md->data;
    Machine* mc = data->machine_list[md->index];
    char** uci_list = data->conf.machine_uci_list[md->index];
    char fen[FEN_LEN];
    int i;
    fen[0] = '\0';

    board_fen_import(&mc->board, mc->fen);
    machine_line_init(mc, &mc->board);
    mc->running = 1;

    write(mc->fd_input, "uci\n", 4);
    read(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN);
    while (strstr(mc->output, "uciok") == NULL) {
        read(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN);
    }

    write(mc->fd_input, "isready\n", 8);
    read(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN);
    while (strstr(mc->output, "readyok") == NULL) {
        read(mc->fd_output, mc->output, MACHINE_OUTPUT_LEN);
    }

    if (uci_list != NULL) {
        for (i = 0; uci_list[i] != NULL; i++) {
            write(mc->fd_input, uci_list[i], strlen(uci_list[i]));
            write(mc->fd_input, "\n", 1);
        }
    }

    //board_fen_import(&mc->board, mc->fen);
    //machine_line_init(mc, &mc->board);
    SDL_Thread* thread = SDL_CreateThread(machine_read, NULL, (void*)md);
    SDL_DetachThread(thread);
    while (mc->running) {
        if (strcmp(fen, mc->fen) && mc->fen_changed) {
            mc->fen_changed = 0;
            snprintf(fen, FEN_LEN, "%s", mc->fen);
            write(mc->fd_input, "stop\n", 5);
            board_fen_import(&mc->board, mc->fen);
            write(mc->fd_input, "position fen ", 13);
            write(mc->fd_input, fen, strlen(fen));
            write(mc->fd_input, "\ngo infinite\n", 13);
        }
        msleep(400);
    }
    return 1;
}

void
machine_start(WindowData* data, int index) {
    Machine* mc = data->machine_list[index];
    if (mc->running) {
        return;
    }
    mc->fen_changed = 1;
    machine_config_free(data);
    machine_config_load(data);
    machine_set_line_count(data, index);
    machine_resize(data, index);

    int pipe_in[2], pipe_out[2];
    pipe(pipe_in);
    pipe(pipe_out);
    mc->fd_input = pipe_in[1];
    mc->fd_output = pipe_out[0];

    signal(SIGCHLD, SIG_IGN); //kernel will clear child process after its exit
    mc->pid = fork();
    if (mc->pid == 0) {
        close(0);
        dup(pipe_in[0]);
        close(1);
        close(2);
        dup(pipe_out[1]);
        execvp(data->conf.machine_cmd_list[index][0], data->conf.machine_cmd_list[index]);
        exit(0);
    } else {
        MachineData* md = &mc->md;
        md->index = index;
        md->data = data;
        SDL_Thread* thread = SDL_CreateThread(machine_write, NULL, (void*)md);
        SDL_DetachThread(thread);
    }
}

void
machine_stop(WindowData* data, int index) {
    Machine* mc = data->machine_list[index];
    if (mc->running) {
        machine_line_free(mc);
        kill(mc->pid, SIGTERM);
    }
    mc->running = 0;
}

#endif
