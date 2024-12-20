#include "normal.h"

int
write_game(WindowData* data) {
    FILE* f;
    int number;

    if (!strcmp(data->number, "a")) {
        f = fopen(data->filename, "a");
        if (f == NULL) {
            return 0;
        }
        pgn_write_file(f, &data->game);
        fclose(f);

        f = fopen(data->filename, "r");
        number = pgn_count_games(f) - 1;
        fclose(f);
        if (number >= 0) {
            snprintf(data->number, data->conf.number_len, "%d", number);
        }
        return 1;
    }
    return pgn_replace_game(data->filename, &data->game, strtol(data->number, NULL, 10));
}

void
mode_normal(WindowData* data) {
    int tmp;
    Square square_dst;
    Piece prom_piece;
    Status status;
    SDL_Event event;
    Machine* mc;

    while (data->loop) {
        //WaitEvent is using way less CPU (on PoolEvent was 100%)
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, NULL, 1);
            handle_non_input_events(&event, data, NULL);
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        data->hidden = chessboard_mouse_square(data);
                        data->piece = data->hidden & 0x88 ? Empty
                                                          : game_move_get(&data->game)->board.position[data->hidden];
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (data->piece != Empty) {
                            square_dst = chessboard_mouse_square(data);
                            status = square_dst != none ? game_move_status(&data->game, data->hidden, square_dst, Empty)
                                                        : Invalid;
                            switch (status) {
                                case Invalid: break;

                                case Promotion:
                                    snprintf(data->status.mode, data->conf.status_max_len, "%s",
                                             data->conf.promotion_status);
                                    prom_piece = mode_promotion(data, game_move_get(&data->game)->board.turn);
                                    if (prom_piece == Empty) {
                                        snprintf(data->status.mode, data->conf.status_max_len, "%s",
                                                 data->conf.normal_status);
                                        break;
                                    }
                                    chessboard_move_do(data, data->hidden, square_dst, prom_piece, status);
                                    snprintf(data->status.mode, data->conf.status_max_len, "%s",
                                             data->conf.normal_status);
                                    break;

                                default: chessboard_move_do(data, data->hidden, square_dst, Empty, status); break;
                            }
                            data->hidden = none;
                            data->piece = Empty;
                            draw_render(data);
                        }

                        if (notation_click(data)) {
                            int ind = notation_coord_index_click(data);
                            if (ind != -1) {
                                data->game.line_current = nt_move_coords[ind].variation;
                                game_move_index_set(&data->game, nt_move_coords[ind].index);
                                data->hidden = none;
                                handle_position_change(data);
                                draw_render(data);
                            }
                        }
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) { //scroll up
                        if (game_move_index_get(&data->game) == 1 && !game_line_is_main(&data->game)) {
                            tmp = variation_index_find(data->game.line_current, data->game.line_current->prev);
                            data->game.line_current = data->game.line_current->prev;
                            if (tmp != -1) {
                                data->game.line_current->move_current = tmp;
                            }
                        } else if (!variation_move_prev(data->game.line_current)) {
                            break;
                        }
                        notation_focus_current_move(data);
                        handle_position_change(data);
                        data->hidden = none;
                        draw_render(data);
                    } else if (event.wheel.y < 0) { //scroll down
                        if (variation_move_next(data->game.line_current)) {
                            notation_focus_current_move(data);
                            handle_position_change(data);
                            data->hidden = none;
                            draw_render(data);
                        }
                    }
                    break;

                //TODO better shortcuts
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_e: mode_editor(data); break;

                        case SDLK_f: mode_filename_edit(data); break;

                        case SDLK_c: mode_clipboard(data); break;

                        case SDLK_a:
                            undo_add(data);
                            if (is_keymod_shift(event)) {
                                mode_annotate(
                                    data,
                                    &data->game.line_current->move_list[data->game.line_current->move_current - 1]);
                            } else if (is_keymod(event, KMOD_NONE)) {
                                mode_annotate(data, game_move_get(&data->game));
                            }
                            break;

                        case SDLK_r:
                            if (is_keymod_ctrl(event)) {
                                redo_do(data);
                                draw_render(data);
                            }
                            break;

                        case SDLK_l:
                        case SDLK_RIGHT:
                            if (is_keymod_shift(event)) {
                                if (!game_move_index_set(&data->game, data->game.line_current->move_count - 1)) {
                                    break;
                                }
                            } else if (is_keymod(event, KMOD_NONE)) {
                                if (!variation_move_next(data->game.line_current)) {
                                    break;
                                }
                            }
                            notation_focus_current_move(data);
                            handle_position_change(data);
                            data->hidden = none;
                            draw_render(data);
                            break;

                        case SDLK_h:
                        case SDLK_LEFT:
                            if (is_keymod_shift(event)) {
                                if (!game_move_index_set(&data->game, !game_line_is_main(&data->game))) {
                                    break;
                                }
                            } else if (is_keymod(event, KMOD_NONE)) {
                                if (game_move_index_get(&data->game) == 1 && !game_line_is_main(&data->game)) {
                                    tmp = variation_index_find(data->game.line_current, data->game.line_current->prev);
                                    data->game.line_current = data->game.line_current->prev;
                                    if (tmp != -1) {
                                        data->game.line_current->move_current = tmp;
                                    }
                                } else {
                                    if (!variation_move_prev(data->game.line_current)) {
                                        break;
                                    }
                                }
                            }
                            notation_focus_current_move(data);
                            handle_position_change(data);
                            data->hidden = none;
                            draw_render(data);
                            break;

                        case SDLK_k:
                        case SDLK_UP:
                            if (is_keymod_shift(event)) {
                                if (game_line_is_main(&data->game)) {
                                    break;
                                }
                                tmp = move_variation_find(variation_move_get(data->game.line_current->prev),
                                                          data->game.line_current);
                                if (tmp == -1 || tmp < 1) {
                                    break;
                                }
                                data->game.line_current =
                                    variation_move_get(data->game.line_current->prev)->variation_list[tmp - 1];
                                data->game.line_current->move_current = 1;
                            } else if (is_keymod(event, KMOD_NONE)) {
                                if (game_line_is_main(&data->game)) {
                                    break;
                                }
                                tmp = variation_index_find(data->game.line_current, data->game.line_current->prev);
                                data->game.line_current = data->game.line_current->prev;
                                if (tmp != -1) {
                                    data->game.line_current->move_current = tmp;
                                }
                            }
                            notation_focus_current_move(data);
                            handle_position_change(data);
                            data->hidden = none;
                            draw_render(data);
                            break;

                        case SDLK_j:
                        case SDLK_DOWN:
                            if (is_keymod_shift(event)) {
                                if (game_line_is_main(&data->game)) {
                                    break;
                                }
                                tmp = move_variation_find(variation_move_get(data->game.line_current->prev),
                                                          data->game.line_current);
                                if (tmp == -1
                                    || tmp + 1 >= variation_move_get(data->game.line_current->prev)->variation_count) {
                                    break;
                                }
                                data->game.line_current =
                                    variation_move_get(data->game.line_current->prev)->variation_list[tmp + 1];
                                data->game.line_current->move_current = 1;
                            } else if (is_keymod(event, KMOD_NONE)) {
                                if (game_move_get(&data->game)->variation_count == 0) {
                                    break;
                                }
                                data->game.line_current = game_move_get(&data->game)->variation_list[0];
                                data->game.line_current->move_current = 1;
                            }
                            notation_focus_current_move(data);
                            handle_position_change(data);
                            data->hidden = none;
                            draw_render(data);
                            break;

                        case SDLK_PAGEUP:
                            scroll_up(&data->notation_scroll);
                            draw_render(data);
                            break;

                        case SDLK_PAGEDOWN:
                            scroll_down(&data->notation_scroll);
                            draw_render(data);
                            break;

                        case SDLK_p:
                            undo_add(data);
                            //save current move from sub variation so we can find it
                            //later
                            int sub_current = data->game.line_current->move_current;
                            game_variation_promote(&data->game);
                            if (is_keymod_shift(event)) {
                                int curr = data->game.line_current->move_current - sub_current;
                                if (curr >= 0) {
                                    Move* m = &data->game.line_current->move_list[curr];
                                    //we can always use 0 index because variations are
                                    //reordered after delete
                                    while (m->variation_count) {
                                        move_variation_delete(m, m->variation_list[0]);
                                    }
                                }
                            }
                            draw_render(data);
                            break;

                        case SDLK_i:
                            undo_add(data);
                            if (!game_line_is_main(&data->game)) {
                                variation_delete_next_moves(data->game.line_current);
                                game_variation_insert(&data->game);
                                draw_render(data);
                            }
                            break;

                        case SDLK_u:
                            if (is_keymod_ctrl(event)) {
                                scroll_up(&data->notation_scroll);
                            } else if (is_keymod(event, KMOD_NONE)) {
                                undo_do(data);
                            }
                            draw_render(data);
                            break;

                        case SDLK_d:
                            undo_add(data);
                            if (is_keymod_shift(event)) {
                                variation_delete_next_moves(data->game.line_current);
                            } else if (is_keymod_ctrl(event)) {
                                scroll_down(&data->notation_scroll);
                            } else if (is_keymod(event, KMOD_NONE)) {
                                game_variation_delete(&data->game);
                                handle_position_change(data);
                            }
                            data->hidden = none;
                            draw_render(data);
                            break;

                        case SDLK_v:
                            if (is_keymod_shift(event)) {
                                mode_position(data);
                            } else if (is_keymod(event, KMOD_NONE)) {
                                mode_move(data);
                            }
                            break;

                        case SDLK_s: mode_san(data); break;

                        case SDLK_t:
                            if (is_keymod_shift(event)) {
                                data->from_game_list = 0;
                                mode_training(data);
                            } else if (is_keymod(event, KMOD_NONE)) {
                                mode_tag(data);
                            }
                            break;

                        case SDLK_g:
                            data->hidden = none;
                            if (is_keymod_shift(event)) {
                                if (data->notation_mode != ModeGameListStat) {
                                    game_list_stat_position(data);
                                    data->notation_mode = ModeGameListStat;
                                } else {
                                    data->notation_mode = ModeMoves;
                                }
                                draw_render(data);
                            } else {
                                mode_game_list(data);
                            }
                            break;

                        case SDLK_m:
                            tmp = is_keymod_shift(event);
                            mc = data->machine_list[tmp];
                            if (mc->running) {
                                machine_stop(data, tmp);
                                mc->output[0] = '\0';
                            } else {
                                machine_start(data, tmp);
                                SDL_DisableScreenSaver();
                            }
                            if (!machine_running_count(data)) {
                                SDL_EnableScreenSaver();
                            }
                            draw_render(data);
                            break;

                        case SDLK_n: mode_number_edit(data); break;

                        case SDLK_SPACE:
                            tmp = is_keymod_shift(event);
                            Square src, dst;
                            mc = data->machine_list[tmp];
                            if (mc->running) {
                                src = mc->line[0].move_list[1].src;
                                dst = mc->line[0].move_list[1].dst;
                                prom_piece = mc->line[0].move_list[1].prom_piece;
                                status = game_move_status(&data->game, src, dst, prom_piece);
                                if (status != Invalid) {
                                    chessboard_move_do(data, src, dst, prom_piece, status);
                                    draw_render(data);
                                }
                            }
                            break;

                        case SDLK_b:
                            data->machine_hidden = !data->machine_hidden;
                            draw_render(data);
                            break;

                        case SDLK_o:
                            tmp = is_keymod_shift(event);
                            if (data->notation_mode != ModeExplorer) {
                                if (data->explorer.running) {
                                    explorer_stop(data);
                                }
                                if (explorer_start(data, tmp)) {
                                    data->notation_mode = ModeExplorer;
                                }
                            } else {
                                data->notation_mode = ModeMoves;
                                explorer_stop(data);
                            }
                            draw_render(data);
                            break;
                        case SDLK_0: explorer_event(data, 0, 1); break;
                        case SDLK_1: explorer_event(data, 1, 0); break;
                        case SDLK_2: explorer_event(data, 2, 0); break;
                        case SDLK_3: explorer_event(data, 3, 0); break;
                        case SDLK_4: explorer_event(data, 4, 0); break;
                        case SDLK_5: explorer_event(data, 5, 0); break;
                        case SDLK_6: explorer_event(data, 6, 0); break;
                        case SDLK_7: explorer_event(data, 7, 0); break;
                        case SDLK_8: explorer_event(data, 8, 0); break;
                        case SDLK_9: explorer_event(data, 9, 0); break;
                    }
                    break;

                case SDL_TEXTINPUT:
                    switch (event.text.text[0]) {
                        case 'W':
                            if (write_game(data)) {
                                message_add(data, &event, "Game written");
                            } else {
                                message_add(data, &event, "Error while writing file");
                            }
                            break;
                    }
                    break;
            }
        }
    }
}
