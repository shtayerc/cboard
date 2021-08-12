#include "normal.h"

void
write_game(WindowData *data){
    FILE *f;
    int number;

    if(!strcmp(data->number, "a")){
        f = fopen(data->filename, "a");
        pgn_write_file(f, &data->notation);
        fclose(f);

        f = fopen(data->filename, "r");
        number = pgn_count_games(f) -1;
        if(number >= 0)
            snprintf(data->number, data->conf.number_len, "%d", number);
        return;
    }
    pgn_replace_game(data->filename, &data->notation,
            strtol(data->number, NULL, 10));
}

void
mode_normal(WindowData *data)
{
    int tmp;
    Square square_dst;
    Piece prom_piece;
    Status status;
    SDL_Event event;

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
                        : notation_move_get(
                                &data->notation)->board.position[data->hidden];
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if(data->piece != Empty){
                        square_dst = chessboard_mouse_square(data);
                        status = square_dst != none ?
                            notation_move_status(&data->notation, data->hidden,
                                    square_dst, Empty) : Invalid;
                        switch(status) {
                        case Invalid:
                            data->hidden = none;
                            break;

                        case Promotion:
                            snprintf(data->status.mode, data->conf.status_max_len,
                                    "%s", data->conf.promotion_status);
                            prom_piece = mode_promotion(data,
                                    notation_move_get(
                                        &data->notation)->board.turn);
                            if(prom_piece == Empty){
                                snprintf(data->status.mode, data->conf.status_max_len,
                                        "%s", data->conf.normal_status);
                                break;
                            }
                            chessboard_move_do(data, data->hidden, square_dst,
                                    prom_piece, status);
                            snprintf(data->status.mode, data->conf.status_max_len,
                                    "%s", data->conf.normal_status);
                            break;

                        default:
                            chessboard_move_do(data, data->hidden, square_dst,
                                    Empty, status);
                            break;
                        }
                        data->piece = Empty;
                        draw_render(data);
                    }


                    if (notation_click(data)) {
                        int ind = notation_coord_index_click(data);
                        if(ind != -1){
                            data->notation.line_current = nt_move_coords[ind].variation;
                            notation_move_index_set(&data->notation,
                                    nt_move_coords[ind].index);
                            data->hidden = none;
                            machine_position(&data->notation);
                            draw_render(data);
                        }
                    }
                }
                break;

            //TODO better shortcuts
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_e:
                    mode_editor(data);
                    break;

                case SDLK_f:
                    mode_filename_edit(data);
                    break;

                case SDLK_c:
                    mode_clipboard(data);
                    break;

                case SDLK_a:
                    undo_add(data);
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        mode_annotate(data,
                                &data->notation.line_current->move_list[
                                data->notation.line_current->move_current-1]);
                    }else{
                        mode_annotate(data, notation_move_get(
                                &data->notation));
                    }
                    break;

                case SDLK_r:
                    if(event.key.keysym.mod & KMOD_CTRL){
                        redo_do(data);
                        draw_render(data);
                    }
                    break;

                case SDLK_l:
                case SDLK_RIGHT:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(!notation_move_index_set(&data->notation,
                                data->notation.line_current->move_count-1))
                            break;
                    }else{
                        if(!variation_move_next(data->notation.line_current))
                            break;
                    }
                    notation_focus_current_move(data);
                    machine_position(&data->notation);
                    data->hidden = none;
                    draw_render(data);
                    break;

                case SDLK_h:
                case SDLK_LEFT:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(!notation_move_index_set(&data->notation,
                                !notation_line_is_main(&data->notation)))
                            break;
                    }else{
                        if(notation_move_index_get(&data->notation) == 1
                                && !notation_line_is_main(&data->notation)){
                            tmp = variation_index_find(
                                    data->notation.line_current,
                                    data->notation.line_current->prev);
                            data->notation.line_current = data->notation.line_current->prev;
                            if(tmp != -1)
                                data->notation.line_current->move_current = tmp;
                        }
                        else{
                            if(!variation_move_prev(data->notation.line_current))
                                break;
                        }
                    }
                    notation_focus_current_move(data);
                    machine_position(&data->notation);
                    data->hidden = none;
                    draw_render(data);
                    break;

                case SDLK_k:
                case SDLK_UP:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(notation_line_is_main(&data->notation))
                            break;
                        tmp = move_variation_find(variation_move_get(
                                    data->notation.line_current->prev),
                                data->notation.line_current);
                        if(tmp == -1 || tmp < 1)
                            break;
                        data->notation.line_current = variation_move_get(
                                data->notation.line_current->prev)->variation_list[tmp-1];
                        data->notation.line_current->move_current = 1;
                    }else{
                        if(notation_line_is_main(&data->notation))
                            break;
                        tmp = variation_index_find(data->notation.line_current,
                                data->notation.line_current->prev);
                        data->notation.line_current = data->notation.line_current->prev;
                        if(tmp != -1)
                            data->notation.line_current->move_current = tmp;
                    }
                    notation_focus_current_move(data);
                    machine_position(&data->notation);
                    data->hidden = none;
                    draw_render(data);
                    break;

                case SDLK_j:
                case SDLK_DOWN:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(notation_line_is_main(&data->notation))
                            break;
                        tmp = move_variation_find(variation_move_get(
                                    data->notation.line_current->prev),
                                data->notation.line_current);
                        if(tmp == -1 || tmp + 1 >= variation_move_get(
                                    data->notation.line_current->prev)->variation_count){
                            break;
                        }
                            data->notation.line_current = variation_move_get(
                                    data->notation.line_current->prev)->variation_list[tmp+1];
                            data->notation.line_current->move_current = 1;
                    }else{
                        if(notation_move_get(
                                &data->notation)->variation_count == 0)
                            break;
                        data->notation.line_current = notation_move_get(
                                &data->notation)->variation_list[0];
                        data->notation.line_current->move_current = 1;
                    }
                    notation_focus_current_move(data);
                    machine_position(&data->notation);
                    data->hidden = none;
                    draw_render(data);
                    break;


                case SDLK_PAGEUP:
                    notation_scroll_up(data);
                    draw_render(data);
                    break;

                case SDLK_PAGEDOWN:
                    notation_scroll_down(data);
                    draw_render(data);
                    break;

                case SDLK_p:
                    undo_add(data);
                    //save current move from sub variation so we can find it
                    //later
                    int sub_current = data->notation.line_current->move_current;
                    notation_variation_promote(&data->notation);
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        int curr = data->notation.line_current->move_current
                            - sub_current;
                        if(curr >= 0){
                            Move *m = &data->notation.line_current->move_list[curr];
                            //we can always use 0 index because variations are
                            //reordered after delete
                            while(m->variation_count){
                                move_variation_delete(m, m->variation_list[0]);
                            }
                        }
                    }
                    draw_render(data);
                    break;

                case SDLK_u:
                    undo_do(data);
                    draw_render(data);
                    break;

                case SDLK_d:
                    undo_add(data);
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        variation_delete_next_moves(data->notation.line_current);
                    }else{
                        notation_variation_delete(&data->notation);
                        machine_position(&data->notation);
                    }
                    data->hidden = none;
                    draw_render(data);
                    break;

                case SDLK_v:
                    if(event.key.keysym.mod & KMOD_SHIFT)
                        mode_position(data);
                    else
                        mode_move(data);
                    break;

                case SDLK_s:
                    mode_san(data);
                    break;

                case SDLK_t:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        mode_training(data);
                    }else{
                        mode_tag(data);
                    }
                    break;

                case SDLK_g:
                    data->hidden = none;
                    mode_game_list(data);
                    break;

                case SDLK_m:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(machine_list[1].running){
                            machine_stop(1);
                            machine_list[1].output[0] = '\0';
                        }else{
                            machine_start(data, 1);
                        }
                    }else{
                        if(machine_list[0].running){
                            machine_stop(0);
                            machine_list[0].output[0] = '\0';
                        }else{
                            machine_start(data, 0);
                        }
                    }
                    draw_render(data);
                    break;

                case SDLK_n:
                    mode_number_edit(data);
                    break;

                case SDLK_SPACE:
                    tmp = (event.key.keysym.mod & KMOD_SHIFT);
                    Square src, dst;
                    if(machine_list[tmp].running){
                        src = machine_list[tmp].line[0].move_list[1].src;
                        dst = machine_list[tmp].line[0].move_list[1].dst;
                        prom_piece = machine_list[tmp].line[0].move_list[1].prom_piece;
                        status = notation_move_status(&data->notation, src,
                                dst, prom_piece);
                        if(status != Invalid){
                            chessboard_move_do(data, src, dst, prom_piece,
                                    status);
                            draw_render(data);
                        }
                    }
                    break;

                case SDLK_b:
                    data->machine_hidden = !data->machine_hidden;
                    draw_render(data);
                    break;
                }
                break;

            case SDL_TEXTINPUT:
                switch(event.text.text[0]){
                case 'W':
                    write_game(data);
                    message_add(data, &event, "Game written");
                    break;
                }
                break;
            }
        }
    }
}
