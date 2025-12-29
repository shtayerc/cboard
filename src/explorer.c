#include "explorer.h"

void
explorer_draw(WindowData* data) {
    notation_background_draw(data);
    SDL_Rect rect = pad_layout(&data->layout.notation);
    for (int i = 0; i < data->explorer.row_count; i++) {
        rect = draw_text(data, &data->layout.notation, rect, 0, TextElementExplorerRow, data->explorer.row_list[i]);
    }
}

void
explorer_init(Explorer* e) {
    e->fen_changed = 0;
    e->row_count = 0;
    e->row_list = NULL;
    e->sp.running = 0;
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
    if (!data->explorer.sp.running) {
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

int
explorer_read(void* p) {
    WindowData* data = (WindowData*)p;
    Explorer* e = &data->explorer;
    char* joined_str = malloc(sizeof(char) * EXPLORER_BUFFER_LEN);
    SDL_IOStream* iostream = SDL_GetProcessOutput(e->sp.process);
    while (e->sp.running) {
        memset(joined_str, 0, EXPLORER_BUFFER_LEN);
        if (SDL_ReadIO(iostream, joined_str, EXPLORER_BUFFER_LEN)) {
            explorer_parse_str(e, joined_str);
            push_user_event();
        }
        SDL_Delay(100);
    }
    free(joined_str);
    SDL_CloseIO(iostream);
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

    e->sp.running = 1;
    e->sp.read_thread = SDL_CreateThread(explorer_read, NULL, (void*)data);
    SDL_IOStream* iostream = SDL_GetProcessInput(e->sp.process);
    while (e->sp.running) {
        if (strcmp(fen, e->fen) && e->fen_changed) {
            e->fen_changed = 0;
            snprintf(fen, FEN_LEN, "fen %s\n", e->fen);
            SDL_WriteIO(iostream, fen, strlen(fen) + 1);
        } else if (e->event > -1) {
            snprintf(cmd, 100, "event %d\n", e->event);
            e->event = -1;
            SDL_WriteIO(iostream, cmd, strlen(cmd) + 1);
        }
        SDL_Delay(400);
    }
    SDL_CloseIO(iostream);
    return 1;
}

int
explorer_start(WindowData* data, int index) {
    Explorer* e = &data->explorer;
    if (e->sp.running) {
        return 0;
    }

    machine_config_free(data);
    machine_config_load(data);

    if (data->conf.explorer_exe_list[index] == NULL) {
        return 0;
    }

    e->fen_changed = 1;

    char* args[] = {data->conf.explorer_exe_list[index], NULL};
    subprocess_start(&e->sp, args);
    e->sp.write_thread = SDL_CreateThread(explorer_write, NULL, (void*)data);
    return 1;
}

void
explorer_stop(WindowData* data) {
    Explorer* e = &data->explorer;
    subprocess_stop(&e->sp);
    explorer_row_free(e);
}
