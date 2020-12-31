#include "window.h"

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
        .piece_path = PIECE_PATH,
        .font_path = FONT_PATH,
        .config_path = "~/.config/cboard/config",
        .rotate_str = "black",
        .default_filename = "new_games.pgn",
        .font_size = 16,
        .message_duration = 2000, //miliseconds
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
    data->status.str = calloc(sizeof(char), data->conf.status_max_len);
    data->status.mode = calloc(sizeof(char), data->conf.status_max_len);
    data->status.info = calloc(sizeof(char), data->conf.status_max_len);
    data->filename = malloc(sizeof(char) * data->conf.status_max_len);
    snprintf(data->filename, data->conf.status_max_len, "%s",
            data->conf.default_filename);
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
    data->rotation = RotationWhite;
    data->message = 0;
    data->message_timestamp = 0;
    game_list_init(&data->game_list);
}

void
window_open(WindowData *data)
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
    data->font_height = FC_GetLineHeight(data->font);
}

void
window_data_free(WindowData *data)
{
    free(data->status.str);
    free(data->status.mode);
    free(data->status.info);
    free(data->filename);
    free(data->number);
    machine_config_free(data);
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
