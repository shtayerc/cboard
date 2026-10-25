#include "game_list.h"

void
mode_game_search(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    GameList new_gl;
    int pos = 0;
    data->message = 0;
    text_null(data->status_info);
    text_cursor_set(data->status_info, pos);
    SDL_StartTextInput(data->window);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if (event.type != SDL_EVENT_KEY_UP || event.key.key != SDLK_ESCAPE) {
                handle_input_events_new(&event, data, &loop, data->status_info);
            }
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            loop = 0;
                            text_null(data->status_info);
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            game_list_search_str(&data->game_list, &new_gl, data->status_info->text);
                            game_list_free(&data->game_list);
                            data->game_list = new_gl;
                            game_list_current_init(data);
                            loop = 0;
                            text_null(data->status_info);
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
    text_null(data->status_info);
    text_cursor_set(data->status_info, pos);
    SDL_StartTextInput(data->window);
    data->mode = ModeTagFilter;
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if (event.type != SDL_EVENT_KEY_UP || event.key.key != SDLK_ESCAPE) {
                handle_input_events_new(&event, data, &loop, data->status_info);
            }
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            loop = 0;
                            text_null(data->status_info);
                            data->mode = ModeGameList;
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            snprintf(key, TAG_LEN, "%s", strtok_r(data->status_info->text, " ", &saveptr));
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
                                        text_null(data->status_info);
                                        loop = 0;
                                        break;

                                    default:
                                        TTF_SetTextString(data->status_info, key, 0);
                                        TTF_AppendTextString(data->status_info, " ", 0);
                                        TTF_AppendTextString(data->status_info, (char[]){ tfo2char(op), '\0' }, 0);
                                        tmp = NULL;
                                        break;
                                }
                            } else if (tmp != NULL && tmp != NULL && !strcmp(tmp, "sort")) {
                                snprintf(data->game_list_sort_tag, TAG_LEN, "%s", key);
                                text_null(data->status_info);
                                mode_game_sort_edit(data);
                                loop = 0;
                            }

                            if (tmp == NULL) {
                                pos = SDL_utf8strlen(data->status_info->text ?: "");
                                text_cursor_set(data->status_info, pos);
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
    data->edit_pointer = data->game_list_sort_direction;
    ttf_text_null(data->edit_text);
    ttf_text_pre(data->edit_text, "[");
    ttf_text_pre(data->edit_text, data->game_list_sort_tag);
    ttf_text_pre(data->edit_text, " sort \"");
    ttf_text_set(data->edit_text, data->game_list_sort_direction);
    ttf_text_post(data->edit_text, "\"]");
    SDL_StartTextInput(data->window);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events_new(&event, data, &loop, data->edit_text);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            if (str2sorting(ttf_text_get(data->edit_text) ?: "") == SortNone) {
                                ttf_text_set(data->edit_text, old);
                            }

                            snprintf(data->game_list_sort_direction, TAG_LEN, "%s", ttf_text_get(data->edit_text));
                            game_list_sort(&data->game_list, data->game_list_sort_tag,
                                           str2sorting(data->game_list_sort_direction));
                            ttf_text_null(data->edit_text);
                            data->edit_pointer = NULL;
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
    data->edit_pointer = tag_filter->tag.value;
    ttf_text_null(data->edit_text);
    ttf_text_pre(data->edit_text, "[");
    ttf_text_pre(data->edit_text, tag_filter->tag.key);
    ttf_text_pre(data->edit_text, " ");
    ttf_text_pre(data->edit_text, (char[]){ tfo2char(tag_filter->op), '\0' });
    ttf_text_pre(data->edit_text, " \"");
    ttf_text_set(data->edit_text, tag_filter->tag.value);
    ttf_text_post(data->edit_text, "\"]");
    SDL_StartTextInput(data->window);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events_new(&event, data, &loop, data->edit_text);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            if (ttf_text_get(data->edit_text) == NULL) {
                                game_list_filter_delete(&data->game_list, tag_filter->tag.key, tag_filter->op);
                            } else {
                                snprintf(tag_filter->tag.value, TAG_LEN, "%s", ttf_text_get(data->edit_text));
                            }
                            for (int i = 0; i < data->game_list.filter_list->ai.count; i++) {
                                game_list_filter(&data->game_list, &new_gl);
                                game_list_free(&data->game_list);
                                data->game_list = new_gl;
                                game_list_current_init(data);
                            }
                            ttf_text_null(data->edit_text);
                            data->edit_pointer = NULL;
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
    data->notation_mode = NotationModeGameList;
    int index = 0;
    char* comment;
    SDL_Event event;
    TagFilterList* filter_list = NULL;
    data->mode = ModeGameList;
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
                            data->notation_mode = NotationModeMoves;
                            data->mode = ModeNormal;
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
                            f = fopen(data->filename->text, "r");
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
                            f = fopen(data->filename->text, "r");
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
                                data->notation_mode = NotationModeMoves;
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
                            data->notation_mode = NotationModeMoves;
                            data->mode = ModeNormal;
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
    char title[GAMETITLE_LEN];
    Tag* tag;
    notation_background_draw(data);
    int i, color, i_count;
    LayoutRect layout = data->layout.notation;
    layout.rect.h -= data->font_height;
    SDL_Rect game_current = pad_layout(&layout);
    game_current.y += data->game_list_scroll.value;
    game_current.h = data->font_height;
    i = -1; //let game_list_loop know that we want pre loop init

    while (game_list_loop(data, &i, &i_count)) {
        tag = tag_list_get(data->game_list.list[i].tag_list, "Color");
        color = tag != NULL && !strcmp(tag->value, "1");
        tag_list_title(data->game_list.list[i].tag_list, title);
        if (i == data->game_list_current) {
            game_current = draw_text(data, &layout, game_current, TextWrapRow, color ? TextElementGameListRowColorCurrent : TextElementGameListRowCurrent, "%s", title);
        } else {
            game_current = draw_text(data, &layout, game_current, TextWrapRow, color ? TextElementGameListRowColor : TextElementGameListRowNormal, "%s", title);
        }
        if (is_null_rect(game_current)) {
            break;
        }
    }
    game_current = draw_text(data, &data->layout.notation, game_current, TextWrapRow, TextElementGameListStatus, "#%d", data->game_list.ai.count);
    //hack to draw non wraping line to get background and keep wrap behavior
    game_current.x += game_current.w;
    game_current.y -= game_current.h;

    if (data->edit_pointer == data->game_list_sort_direction) {
        game_current = draw_ttf_text(data, &data->layout.notation, game_current, TextWrapNewLine, TextElementGameListStatus, data->edit_text);
    } else {
        game_current = draw_text(data, &data->layout.notation, game_current, TextWrapNewLine, TextElementGameListStatus, " [%s sort \"%s\"]",
                                 data->game_list_sort_tag,
                                 data->game_list_sort_direction);
    }
    if (data->game_list.filter_list != NULL) {
        for (i = 0; i < data->game_list.filter_list->ai.count; i++) {
            if (data->edit_pointer == data->game_list.filter_list->list[i].tag.value) {
                game_current = draw_ttf_text(data, &data->layout.notation, game_current, TextWrapNewLine, TextElementGameListStatus, data->edit_text);
            } else {
                game_current = draw_text(data, &data->layout.notation, game_current, TextWrapNewLine, TextElementGameListStatus, " [%s %c \"%s\"]",
                                         data->game_list.filter_list->list[i].tag.key,
                                         tfo2char(data->game_list.filter_list->list[i].op),
                                         data->game_list.filter_list->list[i].tag.value);
            }
        }
    }
    //we set scroll length without scroll_set_length, because game_list length
    //is predictable
    data->game_list_scroll.length = data->font_height * data->game_list.ai.count;
    scroll_set_max(&data->game_list_scroll, data->font_height);
}

void
game_list_focus_current_game(WindowData* data) {
    SDL_Rect padded = pad_layout(&data->layout.notation);
    int y = padded.y + data->game_list_scroll.value;
    y += data->font_height * game_list_current_relative(data);
    int top = padded.y;
    int bot = game_list_get_max_y(data);
    if (y + data->font_height > bot) {
        data->game_list_scroll.value -= data->font_height;
    } else if (y < top) {
        data->game_list_scroll.value -= y - data->layout.notation.padding.top;
    }
}

int
game_list_get_max_y(WindowData* data) {
    return data->layout.notation.rect.y + data->layout.notation.rect.h - data->font_height + 1;
}

void
game_list_game_load(WindowData* data, int index) {
    Board tmp_b = game_move_get(&data->game)->board;
    int gl_index = index == -1 ? data->game_list_current : index;
    index = data->game_list.list[gl_index].index;
    game_free(&data->game);
    game_init(&data->game, NULL);
    char tmp[data->conf.number_len];
    tmp[0] = '\0';
    snprintf(tmp, data->conf.number_len, "%d", index);
    ttf_text_set(data->number, tmp);
    FILE* f = fopen(data->filename->text, "r");
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
