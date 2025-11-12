#include "game_list.h"

void
mode_game_search(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    GameList new_gl;
    int pos = 0;
    data->message = 0;
    data->status.info[0] = '\0';
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if (event.type != SDL_EVENT_KEY_UP || event.key.key != SDLK_ESCAPE) {
                handle_input_events(&event, data, &loop, &pos, data->status.info, data->conf.status_max_len);
            }
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            loop = 0;
                            data->status.info[0] = '\0';
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            cursor_remove(&pos, data->status.info);
                            game_list_search_str(&data->game_list, &new_gl, data->status.info);
                            game_list_free(&data->game_list);
                            data->game_list = new_gl;
                            game_list_current_init(data);
                            loop = 0;
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
mode_game_filter(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    int pos = 0;
    char key[TAG_LEN];
    TagFilterOperator op;
    TagFilter* tag_filter;
    char* saveptr;
    char* tmp;
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.tag_filter_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if (event.type != SDL_EVENT_KEY_UP || event.key.key != SDLK_ESCAPE) {
                handle_input_events(&event, data, &loop, &pos, data->status.info, data->conf.status_max_len);
            }
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            loop = 0;
                            data->status.info[0] = '\0';
                            snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.game_list_status);
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            cursor_remove(&pos, data->status.info);
                            snprintf(key, TAG_LEN, "%s", strtok_r(data->status.info, " ", &saveptr));
                            tmp = strtok_r(NULL, " ", &saveptr);
                            if (tmp != NULL && strcmp(tmp, "sort")) {
                                op = char2tfo(tmp[0]);
                                switch (op) {
                                    case OperatorEquals:
                                    case OperatorContains:
                                    case OperatorGreater:
                                    case OperatorLower:
                                        tag_filter = game_list_filter_get(&data->game_list, key, op);
                                        if (tag_filter == NULL) {
                                            game_list_filter_set(&data->game_list, key, op, "");
                                            tag_filter = game_list_filter_get(&data->game_list, key, op);
                                        }
                                        mode_game_filter_edit(data, tag_filter);
                                        data->status.info[0] = '\0';
                                        loop = 0;
                                        break;

                                    default:
                                        snprintf(
                                            data->status.info,
                                            data->conf.status_max_len,
                                            "%s %c",
                                            key,
                                            tfo2char(op)
                                        );
                                        tmp = NULL;
                                        break;
                                }
                            } else if (tmp != NULL && tmp != NULL && !strcmp(tmp, "sort")) {
                                snprintf(data->game_list_sort_tag, TAG_LEN, "%s", key);
                                mode_game_sort_edit(data);
                                data->status.info[0] = '\0';
                                loop = 0;
                            }

                            if (tmp == NULL) {
                                pos = U8_strlen(data->status.info);
                                cursor_add(&pos, data->status.info, TAG_LEN, data);
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
mode_game_sort_edit(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    char old[TAG_LEN];
    snprintf(old, TAG_LEN, "%s", data->game_list_sort_direction);
    int pos = U8_strlen(data->game_list_sort_direction);
    cursor_add(&pos, data->game_list_sort_direction, TAG_LEN, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, data->game_list_sort_direction, TAG_LEN);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            cursor_remove(&pos, data->game_list_sort_direction);
                            if (str2sorting(data->game_list_sort_direction) == SortNone) {
                                snprintf(data->game_list_sort_direction, TAG_LEN, "%s", old);
                            }
                            game_list_sort(&data->game_list, data->game_list_sort_tag,
                                           str2sorting(data->game_list_sort_direction));
                            draw_render(data);
                            break;
                    }
                    break;
            }
        }
    }
}

void
mode_game_filter_edit(WindowData* data, TagFilter* tag_filter) {
    int loop = 1;
    SDL_Event event;
    GameList new_gl;
    int pos = U8_strlen(tag_filter->tag.value);
    cursor_add(&pos, tag_filter->tag.value, TAG_LEN, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, tag_filter->tag.value, TAG_LEN);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            cursor_remove(&pos, tag_filter->tag.value);
                            if (strlen(tag_filter->tag.value) == 0) {
                                game_list_filter_delete(&data->game_list, tag_filter->tag.key, tag_filter->op);
                            }
                            for (int i = 0; i < data->game_list.filter_list->ai.count; i++) {
                                game_list_filter(&data->game_list, &new_gl);
                                game_list_free(&data->game_list);
                                data->game_list = new_gl;
                                game_list_current_init(data);
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
mode_game_list(WindowData* data) {
    int loop = 1;
    FILE* f;
    GameList new_gl;
    data->notation_mode = ModeGameList;
    int index = 0;
    char* comment;
    SDL_Event event;
    TagFilterList* filter_list = NULL;
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.game_list_status);
    machine_stop(data, 0);
    machine_stop(data, 1);
    game_list_sort(&data->game_list, data->game_list_sort_tag,
                   str2sorting(data->game_list_sort_direction));
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if (event.type == SDL_EVENT_KEY_UP && event.key.key != SDLK_R) {
                handle_non_input_events(&event, data, &loop);
            }
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (SDL_GetKeyFromScancode(event.key.scancode, event.key.mod, false)) {
                        case '/':
                            mode_game_search(data);
                            break;
                    }

                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            loop = 0;
                            data->from_game_list = 0;
                            data->notation_mode = ModeMoves;
                            snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.normal_status);
                            draw_render(data);
                            break;

                        case SDLK_DOWN:
                        case SDLK_J:
                            game_list_current_next(data);
                            game_list_focus_current_game(data);
                            draw_render(data);
                            break;

                        case SDLK_PAGEUP:
                            scroll_up(&data->game_list_scroll);
                            draw_render(data);
                            break;

                        case SDLK_PAGEDOWN:
                            scroll_down(&data->game_list_scroll);
                            draw_render(data);
                            break;

                        case SDLK_UP:
                        case SDLK_K:
                            game_list_current_prev(data);
                            game_list_focus_current_game(data);
                            draw_render(data);

                        case SDLK_LEFT: break;

                        case SDLK_RIGHT: break;

                        case SDLK_R:
                            f = fopen(data->filename, "r");
                            if (f == NULL) {
                                message_add(data, "Error reading file");
                                break;
                            }
                            if (data->game_list.filter_list != NULL && data->game_list.filter_list->ai.count > 0) {
                                filter_list = tfl_clone(data->game_list.filter_list);
                            }
                            game_list_free(&data->game_list);
                            game_list_init(&data->game_list);
                            if (filter_list != NULL) {
                                data->game_list.filter_list = filter_list;
                            }
                            game_list_read_pgn(&data->game_list, f);
                            fclose(f);
                            game_list_sort(&data->game_list, data->game_list_sort_tag, str2sorting(data->game_list_sort_direction));
                            game_list_current_init(data);
                            draw_render(data);
                            break;

                        case SDLK_P:
                            f = fopen(data->filename, "r");
                            game_list_search_board(&data->game_list, &new_gl, f, &game_move_get(&data->game)->board);
                            fclose(f);
                            game_list_free(&data->game_list);
                            data->game_list = new_gl;
                            game_list_sort(&data->game_list, data->game_list_sort_tag, str2sorting(data->game_list_sort_direction));
                            game_list_current_init(data);
                            draw_render(data);
                            break;

                        case SDLK_C:
                            comment = game_move_get(&data->game)->comment;
                            if (comment != NULL) {
                                free(comment);
                            }
                            comment = malloc(sizeof(char) * GAMETITLE_LEN);
                            index = data->game_list_current;
                            tag_list_title(data->game_list.list[index].tag_list, comment);
                            game_move_get(&data->game)->comment = comment;
                            draw_render(data);
                            break;

                        case SDLK_T:
                            if (is_keymod_shift(event)) {
                                game_list_game_load(data, 0);
                                loop = 0;
                                data->notation_mode = ModeMoves;
                                data->from_game_list = 1;
                                mode_training(data);
                            }
                            break;

                        case SDLK_D:
                            if (is_keymod_ctrl(event)) {
                                scroll_down(&data->game_list_scroll);
                                draw_render(data);
                            }
                            break;

                        case SDLK_U:
                            if (is_keymod_ctrl(event)) {
                                scroll_up(&data->game_list_scroll);
                                draw_render(data);
                            }
                            break;

                        case SDLK_F:
                            mode_game_filter(data);
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            game_list_game_load(data, -1);
                            loop = 0;
                            data->notation_mode = ModeMoves;
                            snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.normal_status);
                            draw_render(data);
                            break;
                    }
                    break;
            }
        }
    }
}

void
game_list_current_init(WindowData* data) {
    data->game_list_current = 0;
}

void
game_list_current_next(WindowData* data) {
    if (data->game_list_current + 1 < data->game_list.ai.count) {
        data->game_list_current++;
    }
}

void
game_list_current_prev(WindowData* data) {
    if (data->game_list_current - 1 >= 0) {
        data->game_list_current--;
    }
}

int
game_list_current_relative(WindowData* data) {
    return data->game_list_current;
}

int
game_list_loop(WindowData* data, int* i, int* i_count) {
    if (*i == -1) {
        *i = 0;
        *i_count = data->game_list.ai.count;
    } else {
        *i += 1;
    }
    return *i < *i_count;
}

void
game_list_draw(WindowData* data) {
    SDL_FRect game_current;
    SDL_Color c;
    char title[GAMETITLE_LEN];
    Tag* tag;
    notation_background_draw(data);
    int i, color, i_count;
    game_current.x = data->layout.notation.x + NOTATION_PADDING_LEFT;
    game_current.y = data->layout.notation.y + NOTATION_PADDING_TOP + data->game_list_scroll.value;
    game_current.w = data->layout.notation.w;
    game_current.h = data->font_height;
    i = -1; //let game_list_loop know that we want pre loop init

    while (game_list_loop(data, &i, &i_count)) {
        tag = tag_list_get(data->game_list.list[i].tag_list, "Color");
        color = tag != NULL && !strcmp(tag->value, "1");
        tag_list_title(data->game_list.list[i].tag_list, title);
        if (i == data->game_list_current) {
            c = data->conf.colors[color ? ColorCommentFont : ColorNotationActiveBackground];
            SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(data->renderer, &game_current);
            FC_DrawColor(data->font, data->renderer, game_current.x, game_current.y,
                         data->conf.colors[ColorNotationActiveFont], title);
        } else {
            FC_DrawColor(data->font, data->renderer, game_current.x, game_current.y,
                         data->conf.colors[color ? ColorCommentFont : ColorNotationFont], title);
        }
        game_current.y += data->font_height;
        if (game_current.y + data->font_height > game_list_get_max_y(data)) {
            break;
        }
    }
    c = data->conf.colors[ColorStatusBackground];
    SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(data->renderer, &game_current);
    FC_Rect rect;
    rect = FC_DrawColor(data->font, data->renderer, game_current.x, game_current.y,
                 data->conf.colors[ColorStatusFont], "#%d", data->game_list.ai.count);
    game_current.x += rect.w;

    rect = FC_DrawColor(data->font, data->renderer, game_current.x, game_current.y,
                        data->conf.colors[ColorStatusFont], " [%s sort \"%s\"]",
                        data->game_list_sort_tag,
                        data->game_list_sort_direction);
    game_current.x += rect.w;
    if (data->game_list.filter_list != NULL) {
        for (i = 0; i < data->game_list.filter_list->ai.count; i++) {
            rect = FC_DrawColor(data->font, data->renderer, game_current.x, game_current.y,
                                data->conf.colors[ColorStatusFont], " [%s %c \"%s\"]", 
                                data->game_list.filter_list->list[i].tag.key,
                                tfo2char(data->game_list.filter_list->list[i].op),
                                data->game_list.filter_list->list[i].tag.value
                                );
            game_current.x += rect.w;
        }
    }
    //we set scroll length without scroll_set_length, because game_list length
    //is predictable
    data->game_list_scroll.length = data->font_height * data->game_list.ai.count;
    scroll_set_max(&data->game_list_scroll, data->font_height);
}

void
game_list_focus_current_game(WindowData* data) {
    int y = data->layout.notation.y + NOTATION_PADDING_TOP + data->game_list_scroll.value;
    y += data->font_height * game_list_current_relative(data);
    int top = data->layout.notation.y + NOTATION_PADDING_TOP;
    int bot = game_list_get_max_y(data);
    if (y + data->font_height > bot) {
        data->game_list_scroll.value -= data->font_height;
    } else if (y < top) {
        data->game_list_scroll.value -= y - NOTATION_PADDING_TOP;
    }
}

int
game_list_get_max_y(WindowData* data) {
    return data->layout.notation.y + data->layout.notation.h - data->font_height + 1;
}

void
game_list_game_load(WindowData* data, int index) {
    Board tmp_b = game_move_get(&data->game)->board;
    int gl_index = index == -1 ? data->game_list_current : index;
    index = data->game_list.list[gl_index].index;
    game_free(&data->game);
    game_init(&data->game, NULL);
    snprintf(data->number, data->conf.number_len, "%d", index);
    FILE* f = fopen(data->filename, "r");
    pgn_read_file(f, &data->game, index);
    fclose(f);
    data->notation_scroll.value = 0;
    game_board_find(&data->game, &tmp_b);
}

void
tag_list_title(TagList* tl, char* title) {
    snprintf(title, GAMETITLE_LEN, "%s-%s/%s[%s]/%s (%s)", tag_list_get(tl, "White")->value,
             tag_list_get(tl, "Black")->value, tag_list_get(tl, "Event")->value, tag_list_get(tl, "Round")->value,
             tag_list_get(tl, "Date")->value, tag_list_get(tl, "Result")->value);
}

char
tfo2char(TagFilterOperator op) {
    switch (op) {
        case OperatorEquals:
            return '=';
        case OperatorContains:
            return '~';
        case OperatorGreater:
            return '>';
        case OperatorLower:
            return '<';
        case OperatorNone:
            return ' ';
    }
    return ' ';
}

TagFilterOperator
char2tfo(char ch) {
    switch (ch) {
        case '=':
            return OperatorEquals;
        case '~':
            return OperatorContains;
        case '>':
            return OperatorGreater;
        case '<':
            return OperatorLower;
    }
    return OperatorNone;
}

char*
sorting2str(SortDirection sort) {
    switch (sort) {
        case SortDescending:
            return "Desc";
        case SortAscending:
            return "Asc";
        default:
            break;
    }
    return "";
}

SortDirection
str2sorting(char* sort) {
    if (!strcmp(sort, "Asc")) {
        return SortAscending;
    }
    if (!strcmp(sort, "Desc")) {
        return SortDescending;
    }
    return SortNone;
}
