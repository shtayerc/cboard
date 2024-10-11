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
            if (event.type != SDL_KEYUP || event.key.keysym.sym != SDLK_ESCAPE) {
                handle_input_events(&event, data, &loop, &pos, data->status.info, data->conf.status_max_len);
            }
            switch (event.type) {
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
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
mode_game_list(WindowData* data) {
    int loop = 1;
    FILE* f;
    GameList new_gl;
    data->notation_mode = ModeGameList;
    int index = 0;
    char* comment;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.game_list_status);
    machine_stop(data, 0);
    machine_stop(data, 1);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if (event.type == SDL_KEYUP && event.key.keysym.sym != SDLK_r) {
                handle_non_input_events(&event, data, &loop);
            }
            switch (event.type) {
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            loop = 0;
                            data->from_game_list = 0;
                            data->notation_mode = ModeMoves;
                            snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.normal_status);
                            draw_render(data);
                            break;

                        case SDLK_DOWN:
                        case SDLK_j:
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
                        case SDLK_k:
                            game_list_current_prev(data);
                            game_list_focus_current_game(data);
                            draw_render(data);

                        case SDLK_LEFT: break;

                        case SDLK_RIGHT: break;

                        case SDLK_r:
                            f = fopen(data->filename, "r");
                            if (f == NULL) {
                                message_add(data, &event, "Error reading file");
                                break;
                            }
                            game_list_read_pgn(&data->game_list, f);
                            fclose(f);
                            data->game_list_sorting = Descending;
                            if (is_keymod_shift(event)) {
                                data->game_list_sorting = Ascending;
                            }
                            game_list_current_init(data);
                            draw_render(data);
                            break;

                        case SDLK_p:
                            f = fopen(data->filename, "r");
                            game_list_search_board(&data->game_list, &new_gl, f, &game_move_get(&data->game)->board);
                            fclose(f);
                            game_list_free(&data->game_list);
                            data->game_list = new_gl;
                            game_list_current_init(data);
                            draw_render(data);
                            break;

                        case SDLK_c:
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

                        case SDLK_t:
                            if (is_keymod_shift(event)) {
                                game_list_game_load(data, 0);
                                loop = 0;
                                data->notation_mode = ModeMoves;
                                data->from_game_list = 1;
                                mode_training(data);
                            }
                            break;

                        case SDLK_d:
                            if (is_keymod_ctrl(event)) {
                                scroll_down(&data->game_list_scroll);
                                draw_render(data);
                            }
                            break;

                        case SDLK_u:
                            if (is_keymod_ctrl(event)) {
                                scroll_up(&data->game_list_scroll);
                                draw_render(data);
                            }
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

                case SDL_TEXTINPUT:
                    switch (event.text.text[0]) {
                        case '/': mode_game_search(data); break;
                    }
                    break;
            }
        }
    }
}

void
game_list_current_init(WindowData* data) {
    data->game_list_current = data->game_list_sorting == Ascending ? 0 : data->game_list.ai.count - 1;
}

void
game_list_current_next(WindowData* data) {
    if (data->game_list_sorting == Ascending) {
        if (data->game_list_current + 1 < data->game_list.ai.count) {
            data->game_list_current++;
        }
    } else if (data->game_list_sorting == Descending) {
        if (data->game_list_current - 1 >= 0) {
            data->game_list_current--;
        }
    }
}

void
game_list_current_prev(WindowData* data) {
    if (data->game_list_sorting == Ascending) {
        if (data->game_list_current - 1 >= 0) {
            data->game_list_current--;
        }
    } else if (data->game_list_sorting == Descending) {
        if (data->game_list_current + 1 < data->game_list.ai.count) {
            data->game_list_current++;
        }
    }
}

int
game_list_current_relative(WindowData* data) {
    if (data->game_list_sorting == Descending) {
        return data->game_list.ai.count - data->game_list_current;
    }
    return data->game_list_current;
}

int
game_list_loop(WindowData* data, int* i, int* i_count) {
    if (data->game_list_sorting == Ascending) {
        if (*i == -1) {
            *i = 0;
            *i_count = data->game_list.ai.count;
        } else {
            *i += 1;
        }
        return *i < *i_count;
    } else if (data->game_list_sorting == Descending) {
        if (*i == -1) {
            *i = data->game_list.ai.count - 1;
            *i_count = 0;
        } else {
            *i -= 1;
        }
        return *i >= *i_count;
    }
    return 0;
}

void
game_list_draw(WindowData* data) {
    SDL_Rect game_current;
    SDL_Color c;
    char title[GAMETITLE_LEN];
    Tag* tag;
    notation_background_draw(data);
    int i, x, y, color, i_count;
    x = data->layout.notation.x + NOTATION_PADDING_LEFT;
    y = data->layout.notation.y + NOTATION_PADDING_TOP + data->game_list_scroll.value;
    if (data->game_list.ai.count == 0) {
        FC_DrawColor(data->font, data->renderer, x, y, data->conf.colors[ColorNotationFont], "No games");
        return;
    }
    i = -1; //let game_list_loop know that we want pre loop init

    while (game_list_loop(data, &i, &i_count)) {
        tag = tag_list_get(data->game_list.list[i].tag_list, "Color");
        color = tag != NULL && !strcmp(tag->value, "1");
        tag_list_title(data->game_list.list[i].tag_list, title);
        if (i == data->game_list_current) {
            c = data->conf.colors[color ? ColorCommentFont : ColorNotationActiveBackground];
            game_current.x = x;
            game_current.y = y;
            game_current.w = data->layout.notation.w;
            game_current.h = data->font_height;
            SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(data->renderer, &game_current);
            FC_DrawColor(data->font, data->renderer, x, y, data->conf.colors[ColorNotationActiveFont], title);
        } else {
            FC_DrawColor(data->font, data->renderer, x, y,
                         data->conf.colors[color ? ColorCommentFont : ColorNotationFont], title);
        }
        y += data->font_height;
        if (y > data->layout.notation.y + data->layout.notation.h) {
            break;
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
    int bot = data->layout.notation.y + data->layout.notation.h;
    if (y + data->font_height > bot) {
        data->game_list_scroll.value -= y - bot;
        data->game_list_scroll.value -= data->font_height;
    } else if (y < top) {
        data->game_list_scroll.value -= y - NOTATION_PADDING_TOP;
    }
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
