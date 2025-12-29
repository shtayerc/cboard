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
        .message_duration = 2000, //miliseconds
        .machine_cmd_list = {NULL, NULL},
        .machine_uci_list = {NULL, NULL},
        .colors = {
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
        .text_elements = {
            [TextElementStatus] = {{.top = 0, .right = 0, .bottom = 0, .left = 4}, ColorStatusFont, ColorNone},
            [TextElementGameTag] = {{.top = 0, .right = 8, .bottom = 0, .left = 2}, ColorNotationFont, ColorNone},
            [TextElementTraining] = {{.top = 4, .right = 0, .bottom = 0, .left = 4}, ColorNotationFont, ColorNone},
            [TextElementMoveMainline] = {{.top = 0, .right = 5, .bottom = 0, .left = 2}, ColorNotationFont, ColorNone},
            [TextElementMoveVariation] = {{.top = 0, .right = 5, .bottom = 0, .left = 2}, ColorVariationFont, ColorNone},
            [TextElementMoveCurrent] = {{.top = 0, .right = 5, .bottom = 0, .left = 2}, ColorNotationActiveFont, ColorNotationActiveBackground},
            [TextElementMoveComment] = {{.top = 0, .right = 5, .bottom = 0, .left = 2}, ColorCommentFont, ColorNone},
            [TextElementCustomText] = {{.top = 4, .right = 0, .bottom = 0, .left = 4}, ColorNotationFont, ColorNone},
            [TextElementGameStatRow] = {{.top = 0, .right = 0, .bottom = 0, .left = 4}, ColorNotationFont, ColorNone},
            [TextElementGameListRowNormal] = {{.top = 4, .right = 0, .bottom = 0, .left = 4}, ColorNotationFont, ColorNone},
            [TextElementGameListRowCurrent] = {{.top = 4, .right = 0, .bottom = 0, .left = 4}, ColorNotationActiveFont, ColorNotationActiveBackground},
            [TextElementGameListRowColor] = {{.top = 4, .right = 0, .bottom = 0, .left = 4}, ColorCommentFont, ColorNone},
            [TextElementGameListRowColorCurrent] = {{.top = 4, .right = 0, .bottom = 0, .left = 4}, ColorNotationActiveFont, ColorCommentFont},
            [TextElementGameListStatus] = {{.top = 4, .right = 2, .bottom = 0, .left = 2}, ColorStatusFont, ColorStatusBackground},
            [TextElementMachineRow] = {{0}, ColorMachineFont, ColorNone},
            [TextElementMachineComment] = {{0}, ColorCommentFont, ColorNone},
            [TextElementExplorerRow] = {{.top = 0, .right = 0, .bottom = 0, .left = 4}, ColorNotationFont, ColorNone},
        },
        .explorer_exe_list = {NULL, NULL},
    };
    return config;
}

void
window_data_init(WindowData* data) {
    int i;
    data->conf = config_init();
    data->layout = layout_init();
    data->status.mode = calloc(data->conf.status_max_len, sizeof(char));
    data->status.info = calloc(data->conf.status_max_len, sizeof(char));
    data->filename = malloc(sizeof(char) * data->conf.status_max_len);
    snprintf(data->filename, data->conf.status_max_len, "%s", data->conf.default_filename);
    data->number = malloc(sizeof(char) * data->conf.number_len);
    snprintf(data->number, data->conf.number_len, "a");
    data->custom_text = calloc(data->conf.status_max_len, sizeof(char));
    data->loop = 1;
    data->from_game_list = 0;
    data->mouse.x = data->conf.default_width / 2;
    data->mouse.y = data->conf.default_height / 2;
    scroll_init(&data->notation_scroll);
    data->notation_mode = ModeMoves;
    scroll_init(&data->game_list_scroll);
    data->game_list_sort_tag = calloc(TAG_LEN, sizeof(char));
    snprintf(data->game_list_sort_tag, TAG_LEN, "File");
    data->game_list_sort_direction = calloc(TAG_LEN, sizeof(char));
    snprintf(data->game_list_sort_direction, TAG_LEN, "Desc");
    game_list_current_init(data);
    data->machine_mode = ModeComment;
    undo_init(data->undo_list);
    data->undo_current = -1;
    undo_init(data->redo_list);
    data->redo_current = -1;
    data->rotation = RotationWhite;
    data->message = 0;
    data->message_timer = 0;
    data->piece = Empty;
    data->hidden = none;
    data->font_size = 16;
    game_list_init(&data->game_list);
    gls_init(&data->game_list_stat);
    explorer_init(&data->explorer);
    for (i = 0; i < MACHINE_COUNT; i++) {
        data->machine_list[i] = calloc(1, sizeof(Machine));
        data->machine_list[i]->sp.running = 0;
    }
    vs_init(&data->vs);
}

void
window_open(WindowData* data) {
    SDL_InitSubSystem(SDL_INIT_VIDEO);
    const SDL_DisplayMode* dm = NULL;
    int i, num_displays = 0;
    SDL_DisplayID* displays = SDL_GetDisplays(&num_displays);
    if (displays) {
        for (i = 0; i < num_displays; ++i) {
            SDL_DisplayID instance_id = displays[i];
            dm = SDL_GetCurrentDisplayMode(instance_id);
            break;
        }
        SDL_free(displays);
    }
    data->window = SDL_CreateWindow(data->conf.window_title, dm->w, dm->h,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
    SDL_EnableScreenSaver();
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetWindowMinimumSize(data->window, data->conf.minimal_width, data->conf.minimal_height);
    data->renderer = SDL_CreateRenderer(data->window, NULL);
    data->font = NULL;
    font_init(data);
}

void
font_init(WindowData* data) {
    data->font = FC_CreateFont();
    if (!file_exists(data->conf.font_path)) {
        data->conf.font_path = FALLBACK_PATH "/DejaVuSansCondensed.ttf";
    }
    FC_LoadFont(data->font, data->renderer, data->conf.font_path, data->font_size, data->conf.colors[ColorStatusFont],
                TTF_STYLE_NORMAL);
    data->font_height = FC_GetLineHeight(data->font);
}

void
font_free(WindowData* data) {
    if (data->font != NULL) {
        FC_FreeFont(data->font);
    }
}

void
font_resize(WindowData* data, int step) {
    data->font_size += step;
    font_free(data);
    font_init(data);
}

void
window_data_free(WindowData* data) {
    int i;
    free(data->status.mode);
    free(data->status.info);
    free(data->filename);
    free(data->number);
    free(data->custom_text);
    free(data->game_list_sort_tag);
    free(data->game_list_sort_direction);
    machine_config_free(data);
    undo_free(data->undo_list);
    undo_free(data->redo_list);
    game_free(&data->game);
    game_list_free(&data->game_list);
    explorer_free(&data->explorer);
    font_free(data);
    for (i = 0; i < MACHINE_COUNT; i++) {
        free(data->machine_list[i]);
    }
    vs_free(&data->vs);
    if (data->message_timer > 0) {
        SDL_RemoveTimer(data->message_timer);
    }
    SDL_DestroyRenderer(data->renderer);
    SDL_DestroyWindow(data->window);
    SDL_Quit();
}

Layout
layout_init() {
    Layout layout = {
        .board = {
            .padding = {0},
            .rect = {0}
        },
        .square = {
            .padding = {0},
            .rect = {0}
        },
        .notation = {
            .padding = {.top = 4, .right = 0, .bottom = 0, .left = 4},
            .rect = {0}
        },
        .status = {
            .padding = {.top = 0, .right = 0, .bottom = 0, .left = 4},
            .rect = {0}
        },
        .machine = {
            .padding = {0},
            .rect = {0}
        }
    };
    return layout;
}

void
window_resize(WindowData* data, int width, int height) {
    data->window_width = width;
    data->window_height = height;

    data->layout.status.rect.x = 0;
    data->layout.status.rect.h = data->font_size + data->font_size / 4;
    data->layout.status.rect.w = width;
    data->layout.status.rect.y = height - data->layout.status.rect.h;

    data->layout.square.rect.h = data->conf.square_size;

    data->layout.board.rect.x = 0;
    data->layout.board.rect.y = 0;
    data->layout.board.rect.h = data->layout.status.rect.y;
    data->layout.board.rect.w = data->conf.square_size * 8;
    if (data->layout.board.rect.h < data->layout.board.rect.w) {
        data->layout.board.rect.w = data->layout.board.rect.h;
        data->layout.square.rect.h = data->layout.board.rect.w / 8;
        data->conf.square_size = data->layout.square.rect.h;
    }

    data->layout.machine.rect.x = 0;
    data->layout.machine.rect.y = data->layout.board.rect.w;
    data->layout.machine.rect.w = data->layout.board.rect.w;
    data->layout.machine.rect.h = height - data->layout.board.rect.w - data->layout.status.rect.h;
    data->layout.square.rect.w = data->layout.square.rect.h;

    data->layout.notation.rect.x = data->layout.square.rect.h * 8;
    data->layout.notation.rect.y = 0;
    data->layout.notation.rect.w = width - data->layout.notation.rect.x;
    data->layout.notation.rect.h = height - data->layout.status.rect.h;
    data->notation_scroll.step = data->layout.notation.rect.h / 2;
    data->notation_scroll.step -= data->notation_scroll.step % data->font_height;
    data->notation_scroll.shown = data->layout.notation.rect.h;
    data->game_list_scroll.step = data->layout.notation.rect.h / 2;
    data->game_list_scroll.step -= data->game_list_scroll.step % data->font_height;
    data->game_list_scroll.shown = data->layout.notation.rect.h;
}

void
window_set_title(WindowData* data) {
    snprintf(data->conf.window_title, WINDOW_TITLE_LEN, "%s %s", WINDOW_TITLE_PREFIX, data->filename);
}

void
window_update_title(WindowData* data) {
    SDL_SetWindowTitle(data->window, data->conf.window_title);
}

void
window_calculate_content_size(WindowData* data) {
    int h, w, font_size;
    SDL_GetWindowSize(data->window, &w, &h);
    data->conf.square_size = (w / 2) / 9;
    font_size = SDL_lround(data->conf.square_size / 5);
    font_size = font_size - (font_size % FONT_STEP); //keep it in FONT_STEP
    font_resize(data, font_size - data->font_size);  //calculate step
    window_resize(data, w, h);
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

        case ModeTraining: training_draw(data); break;

        case ModeCustomText: custom_text_draw(data); break;
    }
    status_draw(data);
    if (data->piece != Empty) {
        piece_mouse_position(data);
    }

    //For some reason on slower hardware the last drawn thing is not visible.
    //We solve this by drawing point off screen.
    SDL_RenderPoint(data->renderer, -1, -1);
}

SDL_Rect
draw_text(WindowData* data, LayoutRect* bounds, SDL_Rect pos, int wrap, TextElementIndex eli, const char* fmt_text, ...) {
    TextElement el = data->conf.text_elements[eli];
    char buffer[1024] = {'\0'};
    va_list args;
    va_start(args, fmt_text);
    vsnprintf(buffer, 1024, fmt_text, args);
    va_end(args);
    pos.w = FC_GetWidth(data->font, buffer) + el.padding.right;
    pos.h = data->font_height;
    if (bounds != NULL) {
        if (!wrap) {
            pos.w = bounds->rect.w - bounds->padding.right;
            pos.y += el.padding.top;
        } else if (pos.x + pos.w >= bounds->rect.x + bounds->rect.w) {
            pos.x = bounds->rect.x + bounds->padding.left + el.padding.left;
            pos.y += data->font_height + el.padding.top;
        }
    }
    if (el.bg_color != ColorNone) {
        draw_background(data, pos, el.bg_color);
    }
    //pad this later, so background is drawn on padded space
    pos.h += el.padding.bottom;
    pos.w += el.padding.right;
    pos.x += el.padding.left;
    SDL_Rect rect = FC_DrawColorSimple(data->font, data->renderer, pos.x, pos.y, data->conf.colors[el.fg_color], buffer);
    //rect.h = data->font_height - el.padding.top;
    rect.w += el.padding.right;
    if (wrap) {
        rect.x += rect.w;
    } else {
        rect.x -= el.padding.left;
        rect.y += data->font_height - el.padding.top;
        rect.h = data->font_height - el.padding.top;
    }
    return rect;
}

void
draw_background(WindowData* data, SDL_Rect rect, ColorIndex color_index)
{
    SDL_FRect frect;
    SDL_RectToFRect(&rect, &frect);
    SDL_Color c = data->conf.colors[color_index];
    SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(data->renderer, &frect);
}

void
draw_render(WindowData* data) {
    draw(data);
    SDL_RenderPresent(data->renderer);
}

SDL_Rect
pad_layout(LayoutRect *lrect) {
    SDL_Rect rect = lrect->rect;
    rect.x += lrect->padding.left;
    rect.y += lrect->padding.top;
    return rect;
}
