#include "training.h"

void
ts_init(TrainingStat* ts) {
    ts->vs_index = 0;
    ts->vs_current = 1;
    ts->vs_count = 0;
    ts->gl_index = 0;
}

void
training_repeat(WindowData* data, Variation* v, int move_number) {
    data->game.line_current = v;
    variation_move_current_reset(v);
    game_move_index_set(&data->game, move_number);
    data->ts.vs_index = 0;
}

void
training_next(WindowData* data, Variation* v, int move_number, Color color) {
    VariationSequence vs_tmp;
    if (data->ts.vs_count == data->ts.vs_current) {
        message_add(data, "End of variation");
        return;
    }
    data->game.line_current = v;
    game_move_index_set(&data->game, move_number);
    if (vs_can_generate_next(&data->vs)) {
        data->ts.vs_index = 0;
        vs_tmp = data->vs;
        vs_init(&data->vs);
        vs_generate_next(&data->vs, v, &vs_tmp, color);
        vs_free(&vs_tmp);
        data->ts.vs_current++;
    } else if (data->from_game_list && (data->ts.gl_index + 1) < data->game_list.ai.count) {
        data->ts.vs_index = 0;
        vs_free(&data->vs);
        vs_init(&data->vs);
        (data->ts.gl_index)++;
        game_list_game_load(data, data->ts.gl_index);
        v = data->game.line_current;
        vs_generate_first(&data->vs, v, color);
        move_number = data->game.line_current->move_current;
        data->ts.vs_current++;
    }
}

void
mode_training(WindowData* data) {
    Square src, dst;
    Piece prom_piece;
    Status status;
    SDL_Event event;

    ts_init(&data->ts);
    data->notation_mode = ModeTraining;
    Variation* v = data->game.line_current;
    Color color = game_move_get(&data->game)->board.turn;
    vs_free(&data->vs);
    vs_init(&data->vs);
    vs_generate_first(&data->vs, v, color);
    data->ts.vs_count = variation_vs_count(v);
    int move_number = data->game.line_current->move_current;
    int loop = 1;
    int pos = 0;
    int old_pos;
    int not_move = 0;
    data->message = 0;
    data->status.info[0] = '\0';
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.training_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, data->status.info, data->conf.status_max_len);
            switch (event.type) {
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        data->hidden = chessboard_mouse_square(data);
                        data->piece = data->hidden & 0x88 ? Empty
                                                          : game_move_get(&data->game)->board.position[data->hidden];
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (data->piece != Empty) {
                            dst = chessboard_mouse_square(data);
                            if (game_move_is_present(&data->game, data->hidden, dst, Empty)) {
                                chessboard_vs_focus(data, &data->ts.vs_index, data->hidden, dst, Empty);
                                chessboard_vs_next(data, &data->ts.vs_index);
                            }
                        }
                        data->hidden = none;
                        data->piece = Empty;
                        draw_render(data);
                    }
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    if (event.wheel.y > 0) { //scroll up
                        training_repeat(data, v, move_number);
                    } else if (event.wheel.y < 0) { //scroll down
                        training_next(data, v, move_number, color);
                    }
                    draw_render(data);
                    break;
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            data->notation_mode = ModeMoves;
                            data->status.info[0] = '\0';
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            old_pos = pos;
                            cursor_remove(&pos, data->status.info);
                            if (!strcmp(data->status.info, "Restart")) {
                                not_move = 1;
                                vs_free(&data->vs);
                                vs_init(&data->vs);
                                data->ts.vs_index = 0;
                                data->ts.gl_index = 0;
                                data->ts.vs_current = 1;
                                data->game.line_current = v;
                                variation_move_current_reset(v);
                                game_move_index_set(&data->game, move_number);
                                if (data->from_game_list) {
                                    game_list_game_load(data, 0);
                                    v = data->game.line_current;
                                    move_number = data->game.line_current->move_current;
                                }
                                vs_generate_first(&data->vs, v, color);
                            }

                            if (!strcmp(data->status.info, "Repeat")) {
                                not_move = 1;
                                training_repeat(data, v, move_number);
                            }

                            if (!strcmp(data->status.info, "Next")) {
                                not_move = 1;
                                training_next(data, v, move_number, color);
                            }

                            if (not_move) {
                                not_move = 0;
                                data->status.info[0] = '\0';
                                pos = 0;
                                data->hidden = none;
                                cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
                                draw_render(data);
                                break;
                            }

                            status = game_move_san_status(&data->game, data->status.info, &src, &dst, &prom_piece);
                            if (status == Invalid) {
                                break;
                            }

                            if (game_move_is_present(&data->game, src, dst, prom_piece)) {
                                chessboard_vs_focus(data, &data->ts.vs_index, src, dst, prom_piece);
                                chessboard_vs_next(data, &data->ts.vs_index);
                                data->status.info[0] = '\0';
                                old_pos = 0;
                            }
                            pos = old_pos;
                            cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
                            draw_render(data);
                            break;
                    }
                    break;

                case SDL_EVENT_USER: draw_render(data); break;
            }
        }
    }
}

void
training_draw(WindowData* data) {
    notation_background_draw(data);
    draw_text(data, &data->layout.notation, data->layout.notation.rect, TextWrapRow, TextElementTraining, "%d/%d",
              data->ts.vs_current,
              data->ts.vs_count);

    SDL_Rect padded = pad_layout(&data->layout.notation);
    padded.y += data->font_height;
    if (game_move_is_last(&data->game)) {
        variation_draw(data, data->game.line_current, &padded, 0, 0);
    }
}

