#include "notation.h"

MoveCoord* nt_move_coords;
int nt_move_coord_len;
int nt_move_coord_index;

void
comment_draw(WindowData* data, Move* m, SDL_Rect* pos) {
    char comment[COMMENT_LEN];
    char* word = NULL;
    char* saveptr;

    snprintf(comment, COMMENT_LEN, "%s", m->comment);
    word = strtok_r(comment, " ", &saveptr);

    while (word != NULL) {
        *pos = draw_text(data, &data->layout.notation, *pos, 1, TextElementMoveComment, word);
        word = strtok_r(NULL, " ", &saveptr);
    }
}

void
variation_draw(WindowData* data, Variation* v, SDL_Rect* pos, int i, int recursive) {
    Move* m;
    char san[SAN_LEN + MOVENUM_LEN];
    int movelen = SAN_LEN + MOVENUM_LEN + 2 * NAG_LEN;
    nt_move_coord_len += v->move_count - 1;
    if (nt_move_coord_len == 0) {
        return;
    }
    nt_move_coords = realloc(nt_move_coords, sizeof(MoveCoord) * nt_move_coord_len);
    if (nt_move_coords == NULL) {
        return;
    }

    int x_start = pad_layout(&data->layout.notation).x;
    TextElementIndex tei;
    for (int j = 1; j < v->move_count; j++) {
        if (j == 1 && i) {
            if (v->move_count > 10) {
                pos->y += data->font_height;
                pos->x = x_start;
            }
            *pos = draw_text(data, &data->layout.notation, *pos, 1, TextElementMoveVariation, "(");
        }

        m = &v->move_list[j];
        if (v->move_list[0].comment != NULL && j == 1) {
            comment_draw(data, &v->move_list[0], pos);
        }

        variation_movenumber_export(v, j, i, san, movelen);
        if (j > 1 && v->move_list[j - 2].variation_count > 0 && m->board.turn == White) {
            snprintf(san, movelen, "%d...", (j / 2) + (i / 2));
        }
        concate(san, movelen, "%s%s%s", m->san, nag_map[m->nag_move], nag_map[m->nag_position]);

        if (j == data->game.line_current->move_current && data->game.line_current == v) {
            tei = TextElementMoveCurrent;
        } else {
            tei = i ? TextElementMoveVariation : TextElementMoveMainline;
        }
        *pos = draw_text(data, &data->layout.notation, *pos, 1, tei, san);

        nt_move_coords[nt_move_coord_index].x = pos->x - pos->w; //calculate start of drawn text
        nt_move_coords[nt_move_coord_index].y = pos->y;
        nt_move_coords[nt_move_coord_index].w = pos->w;
        nt_move_coords[nt_move_coord_index].h = pos->h;
        nt_move_coords[nt_move_coord_index].index = j;
        nt_move_coords[nt_move_coord_index].move = m;
        nt_move_coords[nt_move_coord_index].variation = v;
        nt_move_coord_index++;

        if (m->comment != NULL) {
            comment_draw(data, m, pos);
        }

        for (int l = 0; l < v->move_list[j - 1].variation_count && recursive; l++) {
            variation_draw(data, v->move_list[j - 1].variation_list[l], pos, i + j - 1, 1);
        }

        if (j + 1 == v->move_count && i) {
            *pos = draw_text(data, &data->layout.notation, *pos, 1, TextElementMoveVariation, ")");
        }
    }
}

int
notation_click(WindowData* data) {
    return data->notation_mode == ModeMoves && data->mouse.x > data->layout.notation.rect.x
           && data->mouse.x < data->layout.notation.rect.w + data->layout.notation.rect.x
           && data->mouse.y > data->layout.notation.rect.y
           && data->mouse.y < data->layout.notation.rect.h + data->layout.notation.rect.y;
}

int
notation_coord_index_click(WindowData* data) {
    for (int i = 0; i < nt_move_coord_len; i++) {
        if (nt_move_coords[i].x <= data->mouse.x && nt_move_coords[i].x + nt_move_coords[i].w >= data->mouse.x
            && nt_move_coords[i].y <= data->mouse.y && nt_move_coords[i].y + nt_move_coords[i].h >= data->mouse.y) {
            return i;
        }
    }
    return -1;
}

int
notation_coord_index_move(WindowData* data, Move* m) {
    (void)data; //TODO move nt_move_coord data in WindowData
    for (int i = 0; i < nt_move_coord_len; i++) {
        if (nt_move_coords[i].move == m) {
            return i;
        }
    }
    return -1;
}

void
notation_background_draw(WindowData* data) {
    draw_background(data, data->layout.notation.rect, ColorNotationBackground);
}

void
notation_draw_tags(WindowData* data, SDL_Rect* pos) {
    int i;
    for (i = 0; i < data->game.tag_list->ai.count; i++) {
        *pos = draw_text(data, &data->layout.notation, *pos, 1, TextElementGameTag, "[%s \"%s\"]",
                  data->game.tag_list->list[i].key,
                  data->game.tag_list->list[i].value);
    }
    pos->y += data->font_height * 2;
    pos->x = pad_layout(&data->layout.notation).x;
}

void
notation_draw(WindowData* data) {
    notation_background_draw(data);
    nt_move_coord_index = 0;
    nt_move_coord_len = 0;
    SDL_Rect rect = pad_layout(&data->layout.notation);
    rect.y += data->notation_scroll.value;
    rect.w = 0;
    rect.h = 0;
    notation_draw_tags(data, &rect);
    variation_draw(data, data->game.line_main, &rect, 0, 1);
    scroll_set_length(&data->notation_scroll, rect.y);
    scroll_set_max(&data->notation_scroll, data->font_height);
}

void
mode_annotate(WindowData* data, Move* move) {
    int loop = 1;
    SDL_Event event;
    int pos;
    if (move->comment == NULL) {
        move->comment = calloc(COMMENT_LEN, sizeof(char));
        pos = 0;
    } else {
        pos = U8_strlen(move->comment);
    }
    cursor_add(&pos, move->comment, COMMENT_LEN, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.annotate_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, move->comment, COMMENT_LEN);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            if (U8_strlen(move->comment) == 0) {
                                free(move->comment);
                                move->comment = NULL;
                            }
                            draw_render(data);
                            break;
                    }
                    break;
            }
        }
    }
}

void
mode_move(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.move_annotation_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_non_input_events(&event, data, &loop);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE: loop = 0; break;

                        case SDLK_J:
                        case SDLK_DOWN:
                            nag_move_next(&data->game);
                            draw_render(data);
                            break;

                        case SDLK_K:
                        case SDLK_UP:
                            nag_move_prev(&data->game);
                            draw_render(data);
                            break;

                        case SDLK_X:
                        case SDLK_BACKSPACE:
                            game_move_get(&data->game)->nag_move = 0;
                            loop = 0;
                            break;
                    }
            }
        }
    }
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.normal_status);
    draw_render(data);
}

void
mode_position(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.position_annotation_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_non_input_events(&event, data, &loop);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE: loop = 0; break;

                        case SDLK_J:
                        case SDLK_DOWN:
                            nag_position_next(&data->game);
                            draw_render(data);
                            break;

                        case SDLK_K:
                        case SDLK_UP:
                            nag_position_prev(&data->game);
                            draw_render(data);
                            break;

                        case SDLK_X:
                        case SDLK_BACKSPACE:
                            game_move_get(&data->game)->nag_position = 0;
                            loop = 0;
                            break;
                    }
            }
        }
    }
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.normal_status);
    draw_render(data);
}

void
mode_tag_edit(WindowData* data, Tag* tag) {
    int loop = 1;
    SDL_Event event;
    int pos = U8_strlen(tag->value);
    data->notation_scroll.value = 0;
    cursor_add(&pos, tag->value, TAG_LEN, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, tag->value, TAG_LEN);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            if (U8_strlen(tag->value) == 0 && strcmp(tag->key, "Event") && strcmp(tag->key, "Site")
                                && strcmp(tag->key, "Date") && strcmp(tag->key, "Round") && strcmp(tag->key, "White")
                                && strcmp(tag->key, "Black") && strcmp(tag->key, "Result")) {
                                tag_list_delete(data->game.tag_list, tag->key);
                            }
                            if (U8_strlen(tag->value) == 0 && !strcmp(tag->key, "Date")) {
                                tag_list_init_date(data->game.tag_list);
                            }
                            data->status.info[0] = '\0';
                            draw_render(data);
                            break;
                    }
                    break;
            }
        }
    }
}

void
mode_tag(WindowData* data) {
    int loop = 1;
    int pos = 0;
    SDL_Event event;
    Tag* tag;
    data->message = 0;
    data->status.info[0] = '\0';
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.tag_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, data->status.info, data->conf.status_max_len);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            data->status.info[0] = '\0';
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            cursor_remove(&pos, data->status.info);
                            tag = tag_list_get(data->game.tag_list, data->status.info);
                            if (tag == NULL) {
                                tag_list_set(data->game.tag_list, data->status.info, "");
                                tag = tag_list_get(data->game.tag_list, data->status.info);
                            }
                            mode_tag_edit(data, tag);
                            loop = 0;
                            data->status.info[0] = '\0';
                            break;
                    }
                    break;
            }
        }
    }
}

void
mode_clipboard(WindowData* data) {
    int loop = 1;
    char fen[FEN_LEN];
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.clipboard_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_Q:
                            loop = 0;
                            data->loop = 0;
                            break;

                        case SDLK_F:
                            board_fen_export(&game_move_get(&data->game)->board, fen);
                            SDL_SetClipboardText(fen);
                            message_add(data, "FEN copied to clipboard");
                            break;

                        case SDLK_ESCAPE:
                        default:
                            data->status.info[0] = '\0';
                            snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.normal_status);
                            loop = 0;
                            draw_render(data);
                            break;
                    }
                    break;
            }
        }
    }
}

void
game_init_default(Game* g, Board* b) {
    char fen[FEN_LEN];
    board_fen_export(b, fen);
    game_init(g, b);
    if (strcmp(fen, FEN_DEFAULT)) {
        tag_list_set(g->tag_list, "FEN", fen);
    }
    tag_list_init_date(g->tag_list);
}

void tag_list_init_date(TagList* tl) {
    SDL_Time ticks;
    SDL_DateTime dt;
    SDL_GetCurrentTime(&ticks);
    SDL_TimeToDateTime(ticks, &dt, true);
    char date[20];
    snprintf(date, 20, "%d.%02d.%02d", dt.year, dt.month, dt.day);
    tag_list_set(tl, "Date", date);
}

void
undo_init(Game* list[]) {
    memset(list, 0, sizeof(Game*) * UNDO_COUNT);
}

void
undo_add(WindowData* data) {
    int i = (data->undo_current + 1) % UNDO_COUNT;
    if (data->undo_list[i] != NULL && data->undo_list[i]->line_main != data->game.line_main) {
        game_free(data->undo_list[i]);
        free(data->undo_list[i]);
    }
    data->undo_list[i] = game_clone(&data->game);
    data->undo_current = i;
}

void
undo_do(WindowData* data) {
    int i = data->undo_current;
    if (i < 0) {
        return;
    }
    if (data->undo_list[i] != NULL) {
        redo_add(data);
        game_free(&data->game);
        data->game = *data->undo_list[i];
        free(data->undo_list[i]);
        data->undo_list[i] = NULL;
        i--;
        if (i < 0) {
            i = UNDO_COUNT - 1;
        }
        data->undo_current = i;
    }
}

void
redo_add(WindowData* data) {
    int i = (data->redo_current + 1) % UNDO_COUNT;
    if (data->redo_list[i] != NULL && data->redo_list[i]->line_main != data->game.line_main) {
        game_free(data->redo_list[i]);
        free(data->redo_list[i]);
    }
    data->redo_list[i] = game_clone(&data->game);
    data->redo_current = i;
}

void
redo_do(WindowData* data) {
    int i = data->redo_current;
    if (i < 0) {
        return;
    }
    if (data->redo_list[i] != NULL) {
        game_free(&data->game);
        data->game = *data->redo_list[i];
        free(data->redo_list[i]);
        data->redo_list[i] = NULL;
        i--;
        if (i < 0) {
            i = UNDO_COUNT - 1;
        }
        data->redo_current = i;
    }
}

void
undo_free(Game* list[]) {
    int i;
    for (i = 0; i < UNDO_COUNT; i++) {
        if (list[i] != NULL) {
            game_free(list[i]);
            free(list[i]);
        }
    }
}

void
notation_focus_current_move(WindowData* data) {
    int index = notation_coord_index_move(data, game_move_get(&data->game));
    if (index < 0) {
        data->notation_scroll.value = 0;
        return;
    }
    int bot = data->layout.notation.rect.y + data->layout.notation.rect.h;
    if (nt_move_coords[index].y + data->font_height > bot) {
        data->notation_scroll.value -= nt_move_coords[index].y - bot;
        data->notation_scroll.value -= data->font_height;
    } else if (nt_move_coords[index].y < 0) {
        data->notation_scroll.value -= nt_move_coords[index].y;
    }
}

void
custom_text_draw(WindowData* data) {
    notation_background_draw(data);
    draw_text(data, &data->layout.notation, data->layout.notation.rect, 1, TextElementCustomText, data->custom_text);
}

void
game_list_stat_position(WindowData* data) {
    gls_free(&data->game_list_stat);
    gls_init(&data->game_list_stat);
    FILE* f = fopen(data->filename, "r");
    gls_read_pgn_sort(&data->game_list_stat, &data->game_list, f, &game_move_get(&data->game)->board);
    fclose(f);
}

void
game_list_stat_draw(WindowData* data) {
    notation_background_draw(data);
    GameListStatRow* row;

    SDL_Rect rect = data->layout.notation.rect;
    for (int i = 0; i < data->game_list_stat.ai.count; i++) {
        row = &data->game_list_stat.list[i];
        draw_text(data, &data->layout.notation, rect, 0, TextElementGameStatRow,
                "%s (%d) [%d%% | %d%% | %d%%]", row->san, row->count,
                row->white_win > 0 ? (int)SDL_round((float)row->white_win / (float)row->count_finished * 100) : 0,
                row->draw > 0 ? (int)SDL_round((float)row->draw / (float)row->count_finished * 100) : 0,
                row->black_win > 0 ? (int)SDL_round((float)row->black_win / (float)row->count_finished * 100) : 0);
        rect.y += data->font_height;
    }
}
