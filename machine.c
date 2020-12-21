#include "machine.h"

Machine machine_list[2] = { 0 };

void
push_user_event()
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
}

int
machine_control(void *data)
{
    MachineData *md = (MachineData*)data;
    Machine *mc = &machine_list[md->index];
    char *tmp = NULL;
    char *saveptr = NULL;
    char backup;
    char **uci_list = md->data->conf.machine_uci_list[md->index];
    UciScoreType type;
    int depth, score, multipv, pv;
    char fen[FEN_LEN];
    int len, i, j;
    Board b;
    fen[0] = '\0';

    write(mc->fd_input[1], "uci\n", 4);
    read(mc->fd_output[0], mc->output, MACHINE_OUTPUT_LEN);
    while(strstr(mc->output, "uciok") == NULL){
        read(mc->fd_output[0], mc->output, MACHINE_OUTPUT_LEN);
    }

    write(mc->fd_input[1], "isready\n", 8);
    read(mc->fd_output[0], mc->output, MACHINE_OUTPUT_LEN);
    while(strstr(mc->output, "readyok") == NULL){
        read(mc->fd_output[0], mc->output, MACHINE_OUTPUT_LEN);
    }

    mc->line_count = 1;
    if(uci_list != NULL){
        for(i = 0; uci_list[i] != NULL; i++){
            for(j = 0; uci_list[i][j] != '\n'; j++);
            //some hacks to manipulate non null terminated string
            if(j > 29){ //setoption name Multipv value
                backup = uci_list[i][28];
                uci_list[i][28] = '\0';
                if(!strcmp("setoption name Multipv value", uci_list[i]) ||
                        !strcmp("setoption name MultiPV value", uci_list[i])){
                    tmp = calloc((j - 29 + 1), sizeof(char));
                    memcpy(tmp, &uci_list[i][29], j - 29);
                    tmp[j-29] = '\0';
                    mc->line_count = strtol(tmp, NULL, 10);
                    free(tmp);
                    tmp = NULL;
                }
                uci_list[i][28] = backup;
            }
            write(mc->fd_input[1], uci_list[i], j+1);
        }
    }
    board_fen_import(&b, mc->fen);
    machine_line_init(mc, &b);
    write(mc->fd_input[1], "go infinite\n", 12);
    while(mc->running){
        if(strcmp(fen, mc->fen)){
            snprintf(fen, FEN_LEN, "%s", mc->fen);
            write(mc->fd_input[1], "stop\n", 5);
            board_fen_import(&b, mc->fen);
            write(mc->fd_input[1], "position fen ", 13);
            write(mc->fd_input[1], fen, strlen(fen));
            write(mc->fd_input[1], "\ngo infinite\n", 13);
        }
        len = read(mc->fd_output[0], mc->output, MACHINE_OUTPUT_LEN);
        saveptr = NULL;
        if(strstr(mc->output, "multipv") == NULL)
            continue;
        tmp = strtok_r(mc->output, "\n", &saveptr);
        while(tmp != NULL){
            if(strstr(tmp, "multipv") != NULL){
                multipv = 0;
                uci_line_parse(tmp, len, NULL, &depth, &multipv,
                        &type, &score, NULL);
                pv = multipv -1;
                if(pv > -1 && pv < mc->line_count && type != NoType)
                    uci_line_parse(tmp, len, &b, &mc->depth[pv], &multipv,
                        &mc->type[pv], &mc->score[pv], &mc->line[pv]);
                push_user_event();
            }
            tmp = strtok_r(NULL, "\n", &saveptr);
        }
    }
    return 1;
}

void
machine_start(WindowData *data, int index)
{
    Machine *mc = &machine_list[index];
    if(mc->running)
        return;
    free_machine_config(data);
    load_machine_config(data);
    pipe(mc->fd_input);
    pipe(mc->fd_output);
    mc->pid = fork();
    if(mc->pid == 0){
        close(0);
        dup(mc->fd_input[0]);
        close(1);
        dup(mc->fd_output[1]);
        execvp(data->conf.machine_cmd_list[index][0],
                data->conf.machine_cmd_list[index]);
        exit(0);
    }else{
        MachineData md = { index, data};
        mc->running = 1;
        SDL_Thread * thread = SDL_CreateThread(machine_control, NULL,
                (void*)&md);
        SDL_DetachThread(thread);
        free(thread);
    }
}

void
machine_stop(int index)
{
    Machine *mc = &machine_list[index];
    if(mc->running){
        machine_line_free(mc);
        kill(mc->pid, SIGKILL);
    }
    mc->running = 0;
}

void
machine_draw(WindowData *data)
{
    char num[MOVENUM_LEN];
    char move[MOVENUM_LEN+SAN_LEN];
    int i, j, l, font_width, font_height;
    Machine *mc;
    int x = data->layout.machine.x;
    int y = data->layout.machine.y;
    int max_len = data->layout.machine.w + data->layout.machine.x;
    SDL_SetRenderDrawColor(data->renderer, MACHINE_BACKGROUND);
    SDL_RenderFillRect(data->renderer, &data->layout.machine);
    font_height = FC_GetHeight(data->font, "(0.0)");
    for(j = 0; j < MACHINE_COUNT; j++){
        mc = &machine_list[j];
        if(mc->running){
            for(l = 0; l < mc->line_count; l++){
                if(mc->type[l] == NoType)
                        continue;
                x = data->layout.machine.x;
                for(i = 0; i < mc->line[l].move_count; i++){
                    if(i == 0){
                        if(mc->type[l] == Centipawn){
                            snprintf(move, MOVENUM_LEN+SAN_LEN, "(%.2f)[%d]",
                                    (mc->score[l] / 100.f) *
                                    (data->board.turn == White ? 1 : -1),
                                    mc->depth[l]);
                        }else if(mc->type[l] == Mate){
                            snprintf(move, MOVENUM_LEN+SAN_LEN, "(#%d)[%d]",
                                    abs(mc->score[l]), mc->depth[l]);
                        }
                        font_width = FC_GetWidth(data->font, move);
                    }else{
                        if(!strlen(mc->line[l].move_list[i].san))
                            break;
                        variation_movenumber_export(&mc->line[l], i, -1,
                                num, MOVENUM_LEN);
                        snprintf(move, MOVENUM_LEN+SAN_LEN, "%s%s ", num,
                                mc->line[l].move_list[i].san);
                        font_width = FC_GetWidth(data->font, move);
                        if(x + font_width > max_len)
                            break;
                    }
                    FC_DrawColor(data->font, data->renderer, x, y,
                            data->conf.comment_font_color, move);
                    x += font_width;
                }
                y += font_height;
            }
        }
    }
}

void
machine_position(Board *b)
{
    int i;
    for(i = 0; i < MACHINE_COUNT; i++){
        board_fen_export(b, machine_list[i].fen);
    }
}

void
machine_line_init(Machine *m, Board *b)
{
    m->line = calloc(sizeof(Variation), m->line_count);
    m->depth = calloc(sizeof(int), m->line_count);
    m->type = calloc(sizeof(UciScoreType), m->line_count);
    m->score = calloc(sizeof(int), m->line_count);
    int i;
    for(i = 0; i < m->line_count; i++){
        variation_init(&m->line[i], b);
    }
}

void
machine_line_free(Machine *m)
{
    int i;
    for(i = 0; i < m->line_count; i++){
        variation_free(&m->line[i]);
    }
    m->line_count = 0;
    free(m->line);
    free(m->depth);
    free(m->type);
    free(m->score);
}

void
machine_init(Board *b)
{
    int i;
    for(i = 0; i < MACHINE_COUNT; i++){
        machine_list[i].line_count = 1;
        machine_line_init(&machine_list[i], b);
    }
}

void
machine_free()
{
    int i;
    for(i = 0; i < MACHINE_COUNT; i++){
        machine_line_free(&machine_list[i]);
    }
}
