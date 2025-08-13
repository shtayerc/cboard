#include "explorer.h"

void
explorer_draw(WindowData* data) {
    notation_background_draw(data);
    int x_start = data->layout.notation.x + NOTATION_PADDING_LEFT;
    int y = data->layout.notation.y + NOTATION_PADDING_TOP;
    int i, x;
    for (i = 0; i < data->explorer.row_count; i++) {
        x = x_start;
        FC_DrawColor(data->font, data->renderer, x, y, data->conf.colors[ColorNotationFont],
                     data->explorer.row_list[i]);
        y += data->font_height;
    }
}

void
explorer_init(Explorer* e) {
    e->fen_changed = 0;
    e->row_count = 0;
    e->row_list = NULL;
    e->running = 0;
    e->fen[0] = '\0';
    e->event = -1;
}

void
explorer_free(Explorer* e) {
    explorer_row_free(e);
    free(e->row_list);
}

void
explorer_row_add(Explorer* e, char* row) {
    e->row_count++;
    e->row_list = realloc(e->row_list, sizeof(char*) * e->row_count);
    e->row_list[e->row_count - 1] = calloc(BUFFER_LEN, sizeof(char));
    snprintf(e->row_list[e->row_count - 1], BUFFER_LEN, "%s", row);
}

void
explorer_row_free(Explorer* e) {
    int i;
    for (i = 0; i < e->row_count; i++) {
        free(e->row_list[i]);
    }
    e->row_count = 0;
}

void
explorer_position(WindowData* data) {
    explorer_row_free(&data->explorer);
    Board b = game_move_get(&data->game)->board;
    board_fen_export(&b, data->explorer.fen);
    data->explorer.fen_changed = 1;
}

void
explorer_event(WindowData* data, int event, int clear) {
    if (!data->explorer.running) {
        return;
    }
    if (clear) {
        explorer_row_free(&data->explorer);
    }
    data->explorer.event = event;
}

void
explorer_parse_str(Explorer* e, char* str) {
    char *tmp, *saveptr;
    unsigned int len = strlen(str);
    char* buffer = malloc(sizeof(char) * len);
    snprintf(buffer, len, "%s", str);
    tmp = strtok_r(buffer, "\n", &saveptr);
    while (tmp != NULL) {
        explorer_row_add(e, tmp);
        tmp = strtok_r(NULL, "\n", &saveptr);
    }
    free(buffer);
}
