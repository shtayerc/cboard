#include "machine.h"

Machine machine_list[2] = {};

void
push_user_event(int index)
{
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = index;
    SDL_PushEvent(&event);
}

int
machine_read(void *data)
{
    MachineData *md = (MachineData*)data;
    Machine *mc = &machine_list[md->index];

    while(mc->running){
        read(mc->fd_output[0], mc->output, MACHINE_OUTPUT_LEN);
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

    if(uci_list != NULL){
        for(i = 0; uci_list[i] != NULL; i++){
            write(mc->fd_input[1], uci_list[i], strlen(uci_list[i]));
            write(mc->fd_input[1], "\n", 1);
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
            write(mc->fd_input[1], "stop\n", 5);
            board_fen_import(&mc->board, mc->fen);
            write(mc->fd_input[1], "position fen ", 13);
            write(mc->fd_input[1], fen, strlen(fen));
            write(mc->fd_input[1], "\ngo infinite\n", 13);
        }
        sleep(0.4);
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
    pipe(mc->fd_input);
    pipe(mc->fd_output);
    signal(SIGCHLD, SIG_IGN); //kernel will clear child process after its exit
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
        MachineData *md = &mc->md;
        md->index = index;
        md->data = data;
        SDL_Thread * thread = SDL_CreateThread(machine_write, NULL,
                (void*)md);
        SDL_DetachThread(thread);
    }
}

void
machine_stop(int index)
{
    Machine *mc = &machine_list[index];
    if(mc->running){
        machine_line_free(mc);
        kill(mc->pid, SIGTERM);
    }
    mc->running = 0;
}

void
machine_draw(WindowData *data)
{
    char num[MOVENUM_LEN];
    char move[MOVENUM_LEN+SAN_LEN];
    int i, j, l, font_width;
    Machine *mc;
    int x = data->layout.machine.x;
    int y = data->layout.machine.y;
    int max_len = data->layout.machine.w + data->layout.machine.x;
    SDL_SetRenderDrawColor(data->renderer, MACHINE_BACKGROUND);
    SDL_RenderFillRect(data->renderer, &data->layout.machine);
    for(j = 0; j < MACHINE_COUNT; j++){
        mc = &machine_list[j];
        if(mc->running){
            x = data->layout.machine.x;
            FC_DrawColor(data->font, data->renderer, x, y,
                    data->conf.comment_font_color,
                    data->conf.machine_cmd_list[j][0]);
            y += data->font_height;
            for(l = 0; l < mc->line_count; l++){
                if(mc->type[l] == NoType)
                        continue;
                x = data->layout.machine.x;
                for(i = 0; i < mc->line[l].move_count; i++){
                    if(i == 0){
                        if(mc->type[l] == Centipawn){
                            snprintf(move, MOVENUM_LEN+SAN_LEN, "(%.2f)[%d]",
                                    (mc->score[l] / 100.f) *
                                    (notation_move_get(&data->notation)->board.turn == White ? 1 : -1),
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
                y += data->font_height;
            }
        }
    }
}

void
machine_position(Notation *n)
{
    Board b = notation_move_get(n)->board;
    int i, j;
    for(i = 0; i < MACHINE_COUNT; i++){
        board_fen_export(&b, machine_list[i].fen);
        for(j = 0; j < machine_list[i].line_count; j++){
            machine_list[i].line->move_current = 0;
            variation_delete_next_moves(machine_list[i].line);
            machine_list[i].depth[j] = 0;
            machine_list[i].type[j] = Centipawn;
            machine_list[i].score[j] = 0;
        }
    }

    for(i = 0; i < MACHINE_COUNT; i++){
        machine_list[i].fen_changed = 1;
    }
}

void
machine_line_init(Machine *m, Board *b)
{
    m->line = calloc(sizeof(Variation), m->line_count);
    m->depth = calloc(sizeof(int), m->line_count);
    m->type = calloc(sizeof(UciScoreType), m->line_count);
    m->score = calloc(sizeof(int), m->line_count);
    m->board = *b;
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

void
machine_line_parse(int index)
{
    Machine *mc = &machine_list[index];
    char *tmp;
    char *saveptr;
    int depth, score, multipv, pv;
    UciScoreType type;

    tmp = strtok_r(mc->output, "\n", &saveptr);
    while(tmp != NULL){
        if(strstr(tmp, "multipv") != NULL){
            multipv = 0;
            uci_line_parse(tmp, strlen(tmp), NULL, &depth, &multipv,
                    &type, &score, NULL);
            pv = multipv -1;
            if(pv > -1 && pv < mc->line_count && type != NoType)
                uci_line_parse(tmp, strlen(tmp), &mc->board, &mc->depth[pv],
                        &multipv, &mc->type[pv], &mc->score[pv],
                        &mc->line[pv]);
        }
        tmp = strtok_r(NULL, "\n", &saveptr);
    }
}

void
machine_config_load(WindowData *data)
{
    char line[1024];
    char path[1024];
    path[0] = '\0';
    if(data->conf.config_path[0] == '~'){
        snprintf(path, 1024, "%s%s", getenv("HOME"),
                data->conf.config_path+1);
    }else{
        snprintf(path, 1024, "%s", data->conf.config_path);
    }
    char *key, *value, *saveptr;
    FILE *f = fopen(path, "r");
    if(f == NULL)
        return;

    int machine_1_arg_count = 0;
    int machine_2_arg_count = 0;
    int machine_1_uci_count = 0;
    int machine_2_uci_count = 0;
    int machine_1_uci = 0;
    int machine_2_uci = 0;
    while(fgets(line, sizeof(line), f)){
        trimendl(line);
        if(line[0] == '#') //skip comments
            continue;

        if(!machine_1_uci && !machine_2_uci){
            if(!strcmp(line, "machine_1_uci_option_start")){
                data->conf.machine_uci_list[0] = malloc(sizeof(char**));
                data->conf.machine_uci_list[0][0] = NULL;
                machine_1_uci_count = 1;
                machine_1_uci = 1;
                continue;
            }
            if(!strcmp(line, "machine_2_uci_option_start")){
                data->conf.machine_uci_list[1] = malloc(sizeof(char**));
                data->conf.machine_uci_list[1][0] = NULL;
                machine_2_uci_count = 1;
                machine_2_uci = 1;
                continue;
            }
            key = strtok_r(line, "=", &saveptr);
            value = saveptr;
            if(!strcmp(key, "machine_1_exe")){
                if(machine_1_arg_count < 2){
                    machine_1_arg_count = 2;
                    data->conf.machine_cmd_list[0] = realloc(
                            data->conf.machine_cmd_list[0], sizeof(char**)
                            * machine_1_arg_count);
                }
                data->conf.machine_cmd_list[0][0] = strdup(value);
                data->conf.machine_cmd_list[0][1] = NULL;
            }
            if(!strcmp(key, "machine_1_param")){
                if(machine_1_arg_count < 2){
                    machine_1_arg_count = 2;
                    data->conf.machine_cmd_list[0] = realloc(
                            data->conf.machine_cmd_list[0], sizeof(char**)
                            * machine_1_arg_count);
                }
                machine_1_arg_count++;
                data->conf.machine_cmd_list[0] = realloc(
                        data->conf.machine_cmd_list[0], sizeof(char**)
                        * machine_2_arg_count);
                data->conf.machine_cmd_list[0][machine_1_arg_count-2] = strdup(
                        value);
                data->conf.machine_cmd_list[0][machine_1_arg_count-1] = NULL;
            }
            if(!strcmp(key, "machine_2_exe")){
                if(machine_2_arg_count < 2){
                    machine_2_arg_count = 2;
                    data->conf.machine_cmd_list[1] = realloc(
                            data->conf.machine_cmd_list[1], sizeof(char**)
                            * machine_2_arg_count);
                }
                data->conf.machine_cmd_list[1][0] = strdup(value);
                data->conf.machine_cmd_list[1][1] = NULL;
            }
            if(!strcmp(key, "machine_2_param")){
                if(machine_2_arg_count < 2){
                    machine_2_arg_count = 2;
                    data->conf.machine_cmd_list[1] = realloc(
                            data->conf.machine_cmd_list[1], sizeof(char**)
                            * machine_2_arg_count);
                }
                machine_2_arg_count++;
                data->conf.machine_cmd_list[1] = realloc(
                        data->conf.machine_cmd_list[1], sizeof(char**)
                        * machine_2_arg_count);
                data->conf.machine_cmd_list[1][machine_2_arg_count-2] = strdup(
                        value);
                data->conf.machine_cmd_list[1][machine_2_arg_count-1] = NULL;
            }
        }

        if(machine_1_uci){
            if(!strcmp(line, "machine_1_uci_option_end")){
                machine_1_uci = 0;
                continue;
            }else{
                machine_1_uci_count++;
                data->conf.machine_uci_list[0] = realloc(
                        data->conf.machine_uci_list[0], sizeof(char**)
                        * machine_1_uci_count);
                data->conf.machine_uci_list[0][machine_1_uci_count-2] = strdup(
                        line);
                data->conf.machine_uci_list[0][machine_1_uci_count-1] = NULL;
            }
        }
        if(machine_2_uci){
            if(!strcmp(line, "machine_2_uci_option_end")){
                machine_2_uci = 0;
                continue;
            }else{
                machine_2_uci_count++;
                data->conf.machine_uci_list[1] = realloc(
                        data->conf.machine_uci_list[1], sizeof(char**)
                        * machine_2_uci_count);
                data->conf.machine_uci_list[1][machine_2_uci_count-2] = strdup(
                        line);
                data->conf.machine_uci_list[1][machine_2_uci_count-1] = NULL;
            }
        }
    }
    fclose(f);
}

void
machine_config_free(WindowData *data)
{
    int i, j;
    for(i = 0; i < 2; i++){
        if(data->conf.machine_cmd_list[i] == NULL)
            continue;
        for(j = 0; data->conf.machine_cmd_list[i][j] != NULL; j++){
            free(data->conf.machine_cmd_list[i][j]);
        }
        free(data->conf.machine_cmd_list[i]);
        data->conf.machine_cmd_list[i] = NULL;
    }
    for(i = 0; i < 2; i++){
        if(data->conf.machine_uci_list[i] == NULL)
            continue;
        for(j = 0; data->conf.machine_uci_list[i][j] != NULL; j++){
            free(data->conf.machine_uci_list[i][j]);
        }
        free(data->conf.machine_uci_list[i]);
        data->conf.machine_uci_list[i] = NULL;
    }
}

void
machine_set_line_count(WindowData *data, int index)
{
    int i;
    char *num;
    for(i = 0; data->conf.machine_uci_list[index][i] != NULL; i++){
        if(isubstr(data->conf.machine_uci_list[index][i], "multipv")){
            num = strrchr(data->conf.machine_uci_list[index][i], ' ');
            if(num){
                num++;
                machine_list[index].line_count = strtol(num, NULL, 10);
            }
        }
    }
}

void
machine_resize(WindowData *data, int index)
{
    int i, diff;
    int height = 0;
    for(i = 0; i < MACHINE_COUNT; i++){
        if(machine_list[i].running || i == index){
            height += (machine_list[i].line_count + 1) * data->font_height;
        }
    }
    if(height > data->layout.machine.h){
        diff = height - data->layout.machine.h;
        data->conf.square_size -= (diff + (diff % 8)) / 8;
        window_resize(data, data->window_width, data->window_height);
    }
}
