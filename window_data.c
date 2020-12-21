#include "window_data.h"

int
file_exists(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if(f == NULL)
        return 0;
    fclose(f);
    return 1;
}

Config
config_init()
{
    Config config = {
        .path_max_len = 512,
        .status_max_len = 200,
        .number_len = 10,
        .square_size = 100,
        .scroll_step = 20,
        .default_width = 1200,
        .default_height = 800,
        .minimal_width = 600,
        .minimal_height = 400,
        .minimal_square = 30,
        .window_title = "Cboard",
        .normal_status = "[NORMAL]",
        .edit_status = "[EDIT]",
        .annotate_status = "[COMMENT]",
        .promotion_status = "[PROMOTION]",
        .move_annotation_status = "[MOVE ANNOTATION]",
        .position_annotation_status = "[POSITION ANNOTATION]",
        .tag_status = "[TAG]",
        .filename_status = "[FILENAME]",
        .number_status = "[NUMBER]",
        .clipboard_status = "[CLIPBOARD]",
        .san_status = "[SAN]",
        .training_status = "[TRAINING]",
        .game_list_status = "[GAME LIST]",
        .cursor = "│",
        .piece_path = "/usr/share/cboard/alpha",
        .font_path = "/usr/share/cboard/DejaVuSansCondensed.ttf",
        .config_path = "~/.config/cboard/config",
        .rotate_str = "black",
        .font_size = 16,
        .machine_cmd_list = {NULL, NULL},
        .machine_uci_list = {NULL, NULL},
        .status_font_color = {255, 255, 255, 255},
        .notation_font_color = {0, 0, 0, 255},
        .notation_current_fg = {255, 255, 255, 255},
        .comment_font_color = {0, 0, 255, 255},
        .variation_font_color = {120, 120, 120, 255},
    };
    return config;
}

void
window_data_init(WindowData *data)
{
    data->conf = config_init();
    //load_machine_config(data);
    data->status.str = calloc(sizeof(char), data->conf.status_max_len);
    data->status.mode = calloc(sizeof(char), data->conf.status_max_len);
    data->status.info = calloc(sizeof(char), data->conf.status_max_len);
    data->filename = calloc(sizeof(char), data->conf.status_max_len);
    data->number = malloc(sizeof(char) * data->conf.number_len);
    snprintf(data->number, data->conf.number_len, "a");
    data->loop = 1;
    data->mouse.x = data->conf.default_width / 2;
    data->mouse.y = data->conf.default_height / 2;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    data->notation_scroll = 0;
    data->notation_hidden = 0;
    data->game_list_show = 0;
    data->game_list_scroll = 0;
    data->game_list_current = 0;
    undo_init(data->undo_list);
    data->undo_current = -1;
    undo_init(data->redo_list);
    data->redo_current = -1;
}

void
window_open(WindowData *data, const char *fen)
{
    data->window = SDL_CreateWindow(data->conf.window_title,
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            data->conf.default_width, data->conf.default_height,
            SDL_WINDOW_MAXIMIZED|SDL_WINDOW_RESIZABLE);
    SDL_SetWindowMinimumSize(data->window, data->conf.minimal_width,
            data->conf.minimal_height);
    data->renderer = SDL_CreateRenderer(data->window, -1,
            SDL_RENDERER_ACCELERATED);
    data->font = FC_CreateFont();
    if(!file_exists(data->conf.font_path)){
        data->conf.font_path = "./resources/DejaVuSansCondensed.ttf";
    }
    FC_LoadFont(data->font, data->renderer, data->conf.font_path,
            data->conf.font_size, data->conf.status_font_color,
            TTF_STYLE_NORMAL);
    board_fen_import(&data->board, fen);
}

void
window_data_free(WindowData *data)
{
    free(data->status.str);
    free(data->status.mode);
    free(data->status.info);
    free(data->filename);
    free(data->number);
    free_machine_config(data);
    undo_free(data->undo_list);
    undo_free(data->redo_list);
    notation_free(&data->notation);
    FC_FreeFont(data->font);
    SDL_DestroyRenderer(data->renderer);
    SDL_DestroyWindow(data->window);
    SDL_Quit();
}

void
window_resize(WindowData *data, int width, int height)
{
    data->window_width = width;
    data->window_height = height;

    data->layout.status.x = 0;
    data->layout.status.h = 20;
    data->layout.status.w = width;
    data->layout.status.y = height - data->layout.status.h;

    data->layout.square.h = data->conf.square_size;

    data->layout.board.x = 0;
    data->layout.board.y = 0;
    data->layout.board.h = data->layout.status.y;
    data->layout.board.w = data->conf.square_size * 8;
    if(data->layout.board.h < data->layout.board.w){
        data->layout.board.w = data->layout.board.h;
        data->layout.square.h = data->layout.board.w / 8;
        data->conf.square_size = data->layout.square.h;
    }

    data->layout.machine.x = 0;
    data->layout.machine.y = data->layout.board.w;
    data->layout.machine.w = data->layout.board.w;
    data->layout.machine.h = height - data->layout.board.w
        - data->layout.status.h;
    data->layout.square.w = data->layout.square.h;

    data->layout.notation.x = data->layout.square.h * 8;
    data->layout.notation.y = 0;
    data->layout.notation.w = width - data->layout.notation.x;
    data->layout.notation.h = height - data->layout.status.h;
}

void
handle_resize(WindowData *data, SDL_Event *e)
{
    switch(e->window.event){
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        window_resize(data, e->window.data1, e->window.data2);
        break;

    case SDL_WINDOWEVENT_EXPOSED:
        //fix broken font after lid open
        FC_ResetFontFromRendererReset(data->font, data->renderer, 0);
        window_resize(data, data->window_width, data->window_height);
        break;
    }
}

void undo_init(Notation *list[])
{
    memset(list, 0, sizeof(Notation*)*UNDO_COUNT);
}

void
undo_add(WindowData *data)
{
    int i = (data->undo_current + 1) % UNDO_COUNT;
    if(data->undo_list[i] != NULL
            && data->undo_list[i]->line_main != data->notation.line_main){
        notation_free(data->undo_list[i]);
        free(data->undo_list[i]);
    }
    data->undo_list[i] = notation_clone(&data->notation);
    data->undo_current = i;
}

void
undo_do(WindowData *data)
{
    int i = data->undo_current;
    if(i < 0)
        return;
    if(data->undo_list[i] != NULL){
        redo_add(data);
        notation_free(&data->notation);
        data->notation = *data->undo_list[i];
        free(data->undo_list[i]);
        data->undo_list[i] = NULL;
        data->board = notation_move_get(&data->notation)->board;
        i--;
        if(i < 0)
            i = UNDO_COUNT - 1;
        data->undo_current = i;
    }
}

void
redo_add(WindowData *data)
{
    int i = (data->redo_current + 1) % UNDO_COUNT;
    if(data->redo_list[i] != NULL
            && data->redo_list[i]->line_main != data->notation.line_main){
        notation_free(data->redo_list[i]);
        free(data->redo_list[i]);
    }
    data->redo_list[i] = notation_clone(&data->notation);
    data->redo_current = i;
}

void
redo_do(WindowData *data)
{
    int i = data->redo_current;
    if(i < 0)
        return;
    if(data->redo_list[i] != NULL){
        notation_free(&data->notation);
        data->notation = *data->redo_list[i];
        free(data->redo_list[i]);
        data->redo_list[i] = NULL;
        data->board = notation_move_get(&data->notation)->board;
        i--;
        if(i < 0)
            i = UNDO_COUNT - 1;
        data->redo_current = i;
    }
}

void
undo_free(Notation *list[])
{
    int i;
    for(i = 0; i < UNDO_COUNT; i++){
        if(list[i] != NULL){
            notation_free(list[i]);
            free(list[i]);
        }
    }
}

void
load_machine_config(WindowData *data)
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
    int tmp;
    while(fgets(line, sizeof(line), f)){
        trimendl(line);
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
                tmp = strlen(line);
                data->conf.machine_uci_list[0][machine_1_uci_count-2][tmp] = '\n';
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
                tmp = strlen(line);
                data->conf.machine_uci_list[1][machine_2_uci_count-2][tmp] = '\n';
                data->conf.machine_uci_list[1][machine_2_uci_count-1] = NULL;
            }
        }
    }
    fclose(f);
}

void
free_machine_config(WindowData *data)
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
