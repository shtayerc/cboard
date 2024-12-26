#include "window.h"

int
file_exists(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        return 0;
    }
    fclose(f);
    return 1;
}

Config
config_init() {
    Config config = {
        .path_max_len = 512,
        .status_max_len = 200,
        .number_len = 10,
        .square_size = 100,
        .default_width = 1200,
        .default_height = 800,
        .minimal_width = 600,
        .minimal_height = 400,
        .minimal_square = 30,
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
        .tag_filter_status = "[TAG FILTER]",
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
        .colors =
            {
                [ColorSquareWhite] = {240, 217, 181, SDL_ALPHA_OPAQUE},
                [ColorSquareBlack] = {181, 136, 99, SDL_ALPHA_OPAQUE},
                [ColorSquareInactive] = {163, 163, 163, SDL_ALPHA_OPAQUE},
                [ColorSquareActive] = {202, 109, 55, SDL_ALPHA_OPAQUE},
                [ColorSquareWhiteLast] = {206, 210, 107, SDL_ALPHA_OPAQUE},
                [ColorSquareBlackLast] = {171, 162, 58, SDL_ALPHA_OPAQUE},
                [ColorStatusBackground] = {85, 85, 85, SDL_ALPHA_OPAQUE},
                [ColorStatusFont] = {255, 255, 255, 255},
                [ColorNotationBackground] = {38, 36, 33, SDL_ALPHA_OPAQUE},
                [ColorNotationFont] = {255, 255, 255, SDL_ALPHA_OPAQUE},
                [ColorNotationActiveBackground] = {255, 255, 255, SDL_ALPHA_OPAQUE},
                [ColorNotationActiveFont] = {38, 36, 33, SDL_ALPHA_OPAQUE},
                [ColorCommentFont] = {0, 221, 255, SDL_ALPHA_OPAQUE},
                [ColorVariationFont] = {153, 153, 153, SDL_ALPHA_OPAQUE},
                [ColorMachineBackground] = {38, 36, 33, SDL_ALPHA_OPAQUE},
                [ColorMachineFont] = {255, 255, 255, SDL_ALPHA_OPAQUE},
            },
        .explorer_exe_list = {NULL, NULL},
    };
    return config;
}

void
window_data_init(WindowData* data) {
    int i;
    data->conf = config_init();
    data->status.mode = calloc(sizeof(char), data->conf.status_max_len);
    data->status.info = calloc(sizeof(char), data->conf.status_max_len);
    data->filename = malloc(sizeof(char) * data->conf.status_max_len);
    snprintf(data->filename, data->conf.status_max_len, "%s", data->conf.default_filename);
    data->number = malloc(sizeof(char) * data->conf.number_len);
    snprintf(data->number, data->conf.number_len, "a");
    data->custom_text = calloc(sizeof(char), data->conf.status_max_len);
    data->loop = 1;
    data->from_game_list = 0;
    data->mouse.x = data->conf.default_width / 2;
    data->mouse.y = data->conf.default_height / 2;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    scroll_init(&data->notation_scroll);
    data->notation_hidden = 0;
    data->notation_mode = ModeMoves;
    scroll_init(&data->game_list_scroll);
    data->game_list_sorting = SortAscending;
    game_list_current_init(data);
    data->machine_hidden = 0;
    undo_init(data->undo_list);
    data->undo_current = -1;
    undo_init(data->redo_list);
    data->redo_current = -1;
    data->rotation = RotationWhite;
    data->message = 0;
    data->message_timestamp = 0;
    data->piece = Empty;
    data->hidden = none;
    game_list_init(&data->game_list);
    gls_init(&data->game_list_stat);
    explorer_init(&data->explorer);
    for (i = 0; i < MACHINE_COUNT; i++) {
        data->machine_list[i] = calloc(sizeof(Machine), 1);
        data->machine_list[i]->running = 0;
    }
    vs_init(&data->vs);
}

void
window_open(WindowData* data) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_DisplayMode dm;
    SDL_GetDisplayMode(0, 0, &dm);
    window_set_title(data);
    data->window = SDL_CreateWindow(data->conf.window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, dm.w,
                dm.h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
    SDL_EnableScreenSaver();
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetWindowMinimumSize(data->window, data->conf.minimal_width, data->conf.minimal_height);
    data->renderer = SDL_CreateRenderer(data->window, -1, SDL_RENDERER_ACCELERATED);
    data->font = FC_CreateFont();
    if (!file_exists(data->conf.font_path)) {
        data->conf.font_path = FALLBACK_PATH "/DejaVuSansCondensed.ttf";
    }
    FC_LoadFont(data->font, data->renderer, data->conf.font_path, data->conf.font_size,
                data->conf.colors[ColorStatusFont], TTF_STYLE_NORMAL);
    data->font_height = FC_GetLineHeight(data->font);
}

void
window_data_free(WindowData* data) {
    int i;
    free(data->status.mode);
    free(data->status.info);
    free(data->filename);
    free(data->number);
    free(data->custom_text);
    machine_config_free(data);
    undo_free(data->undo_list);
    undo_free(data->redo_list);
    game_free(&data->game);
    game_list_free(&data->game_list);
    explorer_free(&data->explorer);
    FC_FreeFont(data->font);
    for (i = 0; i < MACHINE_COUNT; i++) {
        free(data->machine_list[i]);
    }
    vs_free(&data->vs);
    SDL_DestroyRenderer(data->renderer);
    SDL_DestroyWindow(data->window);
    SDL_Quit();
}

void
window_resize(WindowData* data, int width, int height) {
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
    if (data->layout.board.h < data->layout.board.w) {
        data->layout.board.w = data->layout.board.h;
        data->layout.square.h = data->layout.board.w / 8;
        data->conf.square_size = data->layout.square.h;
    }

    data->layout.machine.x = 0;
    data->layout.machine.y = data->layout.board.w;
    data->layout.machine.w = data->layout.board.w;
    data->layout.machine.h = height - data->layout.board.w - data->layout.status.h;
    data->layout.square.w = data->layout.square.h;

    data->layout.notation.x = data->layout.square.h * 8;
    data->layout.notation.y = 0;
    data->layout.notation.w = width - data->layout.notation.x;
    data->layout.notation.h = height - data->layout.status.h;
    data->notation_scroll.step = data->layout.notation.h / 2;
    data->notation_scroll.step -= data->notation_scroll.step % data->font_height;
    data->notation_scroll.shown = data->layout.notation.h;
    data->game_list_scroll.step = data->layout.notation.h / 2;
    data->game_list_scroll.step -= data->game_list_scroll.step % data->font_height;
    data->game_list_scroll.shown = data->layout.notation.h;
}

void
window_set_title(WindowData* data)
{
    snprintf(data->conf.window_title, WINDOW_TITLE_LEN, "%s %s", WINDOW_TITLE_PREFIX, data->filename);
}

void
window_update_title(WindowData* data)
{
    SDL_SetWindowTitle(data->window, data->conf.window_title);
}

void
draw(WindowData* data) {
    background_draw(data);
    foreground_draw(data);
    machine_draw(data);

    switch (data->notation_mode) {
        case ModeMoves: notation_draw(data); break;

        case ModeGameList: game_list_draw(data); break;

        case ModeGameListStat: game_list_stat_draw(data); break;

        case ModeExplorer: explorer_draw(data); break;

        case ModeCustomText: custom_text_draw(data); break;
    }
    status_draw(data);
    if (data->piece != Empty) {
        piece_mouse_position(data);
    }

    //For some reason on slower hardware the last drawn thing is not visible.
    //We solve this by drawing point off screen.
    SDL_RenderDrawPoint(data->renderer, -1, -1);
}

void
draw_render(WindowData* data) {
    draw(data);
    SDL_RenderPresent(data->renderer);
}
