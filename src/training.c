#include "training.h"

void
ts_init(TrainingStat* ts) {
    ts->vs_index = 0;
    ts->vs_current = 1;
    ts->vs_count = 0;
    ts->gl_index = 0;
    ts->mistake_count = 0;
    ts->mistake_sum = 0;
}

void
training_repeat(WindowData* data, Variation* v, int move_number) {
    data->game.line_current = v;
    variation_move_current_reset(v);
    game_move_index_set(&data->game, move_number);
    data->ts.vs_index = 0;
    data->ts.mistake_sum -= data->ts.mistake_count;
    data->ts.mistake_count = 0;
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
        data->ts.mistake_sum += data->ts.mistake_count;
        data->ts.mistake_count = 0;
        data->ts.vs_index = 0;
        vs_tmp = data->vs;
        vs_init(&data->vs);
        vs_generate_next(&data->vs, v, &vs_tmp, color);
        vs_free(&vs_tmp);
        data->ts.vs_current++;
    } else if (data->from_game_list && (data->ts.gl_index + 1) < data->game_list.ai.count) {
        data->ts.mistake_sum += data->ts.mistake_count;
        data->ts.mistake_count = 0;
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

int
training_move_try(WindowData* data, Square src, Square dst, Piece prom_piece)
{
    int is_present = game_move_is_present(&data->game, src, dst, prom_piece);
    if (is_present) {
        chessboard_vs_focus(data, &data->ts.vs_index, src, dst, prom_piece);
        chessboard_vs_next(data, &data->ts.vs_index);
    } else if (src != dst) {
        data->ts.mistake_count++;
        data->ts.mistake_sum++;
    }
    return is_present;
}

void
mode_training(WindowData* data) {
    Square src, dst;
    Piece prom_piece;
    Status status;
    SDL_Event event;

    ts_init(&data->ts);
    data->notation_mode = NotationModeTraining;
    Variation* v = data->game.line_current;
    Color color = game_move_get(&data->game)->board.turn;
    vs_free(&data->vs);
    vs_init(&data->vs);
    vs_generate_first(&data->vs, v, color);
    data->ts.vs_count = variation_vs_count(v);
    int move_number = data->game.line_current->move_current;
    int loop = 1;
    int pos = 0;
    int not_move = 0;
    data->message = 0;
    TTF_SetTextString(data->status_info, "", 0);
    text_cursor_set(data->status_info, pos);
    SDL_StartTextInput(data->window);
    data->mode = ModeTraining;
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events_new(&event, data, &loop, data->status_info);
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
                            training_move_try(data, data->hidden, dst, Empty);
                        }
                        data->hidden = none;
                        data->piece = Empty;
                        draw_render(data);
                    }
                    break;

                case SDL_EVENT_MOUSE_WHEEL:
                    if (event.wheel.integer_y > 0) { //scroll up
                        training_repeat(data, v, move_number);
                    } else if (event.wheel.integer_y < 0) { //scroll down
                        training_next(data, v, move_number, color);
                    }
                    draw_render(data);
                    break;
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            data->notation_mode = NotationModeMoves;
                            TTF_SetTextString(data->status_info, "", 0);
                            draw_render(data);
                            break;

                        case SDLK_RETURN:
                            if (!strcmp(data->status_info->text, "Restart")) {
                                not_move = 1;
                                vs_free(&data->vs);
                                vs_init(&data->vs);
                                data->ts.vs_index = 0;
                                data->ts.gl_index = 0;
                                data->ts.vs_current = 1;
                                data->ts.mistake_count = 0;
                                data->ts.mistake_sum = 0;
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

                            if (!strcmp(data->status_info->text, "Repeat")) {
                                not_move = 1;
                                training_repeat(data, v, move_number);
                            }

                            if (!strcmp(data->status_info->text, "Next")) {
                                not_move = 1;
                                training_next(data, v, move_number, color);
                            }

                            if (not_move) {
                                not_move = 0;
                                TTF_SetTextString(data->status_info, "", 0);
                                pos = 0;
                                data->hidden = none;
                                draw_render(data);
                                break;
                            }

                            status = game_move_san_status(&data->game, data->status_info->text, &src, &dst, &prom_piece);
                            if (status == Invalid) {
                                break;
                            }

                            if (training_move_try(data, src, dst, prom_piece)) {
                                TTF_SetTextString(data->status_info, "", 0);
                                pos = 0;
                                text_cursor_set(data->status_info, pos);
                            }
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
    SDL_Rect rect = pad_layout(&data->layout.notation);
    rect = draw_text(data, &data->layout.notation, rect, TextWrapRow, TextElementTraining, "[Training]");
    rect = draw_text(data, &data->layout.notation, rect, TextWrapRow, TextElementTraining, "Variation: %d/%d",
              data->ts.vs_current,
              data->ts.vs_count);
    rect = draw_text(data, &data->layout.notation, rect, TextWrapRow, TextElementTraining, "Mistakes: %d/%d",
              data->ts.mistake_count,
              data->ts.mistake_sum);

    rect.y += data->font_height;
    if (game_move_is_last(&data->game)) {
        variation_draw(data, data->game.line_current, &rect, 0, 0);
    }
}

