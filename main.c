#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>

#include "SDL_FontCache.h"
#include "window.h"
#include "chessboard.h"
#include "notation.h"
#include "status.h"
#include "machine.h"
#include "nag.h"
#include "game_list.h"
#include "event.h"
#include "message.h"

#define CHESS_UTILS_IMPLEMENTATION
#include "chess_utils.h"

#define VERSION "0.1"

char fen[FEN_LEN];

void
output_game(WindowData *data, char *type){
    if(!strcmp(type, "pgn")){
        pgn_write_file(stdout, &data->notation);
    }else if(!strcmp(type, "fen")){
        board_fen_export(&notation_move_get(&data->notation)->board, fen);
        fprintf(stdout, "%s\n", fen);
    }
}

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
usage()
{
    fprintf(stdout,
        "Usage: cboard [OPTIONS] [file.pgn]\n"
        "\nOPTIONS\n"
        " -c --config <file>      Config to use (default ~/.config/cboard/config)\n"
        " -f --font <file>        Font to use\n"
        "    --FEN <fen string>   Starting position\n"
        " -h --help               Print help\n"
        " -n --number <number>    Game number\n"
        " -o --output fen|pgn     Output to stdout\n"
        " -p --pieces <dir>       Pieces to use\n"
        " -v --version            Print version\n");
}

int
main(int argc, char *argv[])
{
    srand(time(NULL));
    snprintf(fen, FEN_LEN, "%s", FEN_DEFAULT);
    WindowData data;
    window_data_init(&data);
    Board board;
    FILE *file = NULL;
    char output_type[4] = "";
    int i, number;

    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "--number") || !strcmp(argv[i], "-n")){
            if(++i == argc){
                usage();
                return 1;
            }
            snprintf(data.number, data.conf.number_len, "%s", argv[i]);
        }else if(!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o")){
            if(++i == argc){
                usage();
                return 1;
            }
            snprintf(output_type, 4, "%s", argv[i]);
        }else if(!strcmp(argv[i], "--FEN")){
            if(++i == argc){
                usage();
                return 1;
            }
            snprintf(fen, FEN_LEN, "%s", argv[i]);
        }else if(!strcmp(argv[i], "--font") || !strcmp(argv[i], "-f")){
            if(++i == argc){
                usage();
                return 1;
            }
            data.conf.font_path = argv[i];
        }else if(!strcmp(argv[i], "--pieces") || !strcmp(argv[i], "-p")){
            if(++i == argc){
                usage();
                return 1;
            }
            data.conf.piece_path = argv[i];
        }else if(!strcmp(argv[i], "--config") || !strcmp(argv[i], "-c")){
            if(++i == argc){
                usage();
                return 1;
            }
            data.conf.config_path = argv[i];
        }else if(!strcmp(argv[i], "-")){
            snprintf(data.filename, data.conf.status_max_len, "-");
            file = stdin;
        }else if(!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")){
            fprintf(stdout, "cboard %s\n", VERSION);
            return 0;
        }else if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
            usage();
            return 0;
        }else if(i == argc - 1){
            snprintf(data.filename, data.conf.status_max_len, "%s", argv[i]);
        }else{
            usage();
            return 1;
        }
    }

    board_fen_import(&board, fen);
    game_init(&data.notation, &board);

    if(file == NULL){
        file = fopen(data.filename, "r");
    }
    if(isubstr(data.filename, data.conf.rotate_str))
        data.rotation = RotationBlack;
    if(strcmp(data.number, "a") || !strcmp(data.filename, "-")){
        number = strtol(data.number, NULL, 10);
        if(file != NULL && !pgn_read_file(file, &data.notation, number)){
            fprintf(stderr, "Invalid pgn\n");
            return 1;
        }
    }else{
        if(file != NULL){
            game_list_read_pgn(&data.game_list, file);
            game_list_reverse(&data.game_list);
            if(data.game_list.count == 1){
                snprintf(data.number, data.conf.number_len, "%s", "0");
                fseek(file, 0, SEEK_SET);
                if(!pgn_read_file(file, &data.notation, 0)){
                    fprintf(stderr, "Invalid pgn\n");
                    return 1;
                }
            }else{
                SDL_Event tmp_event;
                tmp_event.type = SDL_KEYUP;
                tmp_event.key.keysym.sym = SDLK_g;
                SDL_PushEvent(&tmp_event);
            }
        }
    }

    if(strcmp(data.filename, "-") && file != NULL)
        fclose(file);

    int tmp;
    Square square_dst;
    Piece prom_piece;
    Status status;

    SDL_Event event;
    window_open(&data);
    window_resize(&data, data.conf.default_width, data.conf.default_height);
    snprintf(data.status.mode, data.conf.status_max_len, "%s",
            data.conf.normal_status);
    machine_position(&data.notation);
    piece_load(&data);

    while (data.loop) {
        //WaitEvent is using way less CPU (on PoolEvent was 100%)
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, &data, NULL, 1);
            handle_non_input_events(&event, &data, NULL);
            switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    cb_hidden = chessboard_mouse_square(&data);
                    data.piece = cb_hidden & 0x88 ? 0
                        : notation_move_get(
                                &data.notation)->board.position[cb_hidden];
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if(data.piece != Empty){
                        square_dst = chessboard_mouse_square(&data);
                        status = notation_move_status(&data.notation, cb_hidden,
                                square_dst, Empty);
                        switch(status) {
                        case Invalid:
                            cb_hidden = none;
                            break;

                        case Promotion:
                            snprintf(data.status.mode, data.conf.status_max_len,
                                    "%s", data.conf.promotion_status);
                            prom_piece = mode_promotion(&data,
                                    notation_move_get(
                                        &data.notation)->board.turn);
                            if(prom_piece == Empty){
                                snprintf(data.status.mode, data.conf.status_max_len,
                                        "%s", data.conf.normal_status);
                                break;
                            }
                            chessboard_move_do(&data, cb_hidden, square_dst,
                                    prom_piece, status);
                            snprintf(data.status.mode, data.conf.status_max_len,
                                    "%s", data.conf.normal_status);
                            break;

                        default:
                            chessboard_move_do(&data, cb_hidden, square_dst,
                                    Empty, status);
                            break;
                        }
                        data.piece = Empty;
                        draw_render(&data);
                    }


                    if (notation_click(&data)) {
                        int ind = notation_coord_index_click(&data);
                        if(ind != -1){
                            data.notation.line_current = nt_move_coords[ind].variation;
                            notation_move_index_set(&data.notation,
                                    nt_move_coords[ind].index);
                            cb_hidden = none;
                            machine_position(&data.notation);
                            draw_render(&data);
                        }
                    }
                }
                break;

            //TODO better shortcuts
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_e:
                    mode_editor(&data);
                    break;

                case SDLK_f:
                    mode_filename_edit(&data);
                    break;

                case SDLK_c:
                    mode_clipboard(&data);
                    break;

                case SDLK_a:
                    undo_add(&data);
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        mode_annotate(&data,
                                &data.notation.line_current->move_list[
                                data.notation.line_current->move_current-1]);
                    }else{
                        mode_annotate(&data, notation_move_get(
                                &data.notation));
                    }
                    break;

                case SDLK_r:
                    if(event.key.keysym.mod & KMOD_CTRL){
                        redo_do(&data);
                        draw_render(&data);
                    }
                    break;

                case SDLK_l:
                case SDLK_RIGHT:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(!notation_move_index_set(&data.notation,
                                data.notation.line_current->move_count-1))
                            break;
                    }else{
                        if(!variation_move_next(data.notation.line_current))
                            break;
                    }
                    notation_focus_current_move(&data);
                    machine_position(&data.notation);
                    cb_hidden = none;
                    draw_render(&data);
                    break;

                case SDLK_h:
                case SDLK_LEFT:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(!notation_move_index_set(&data.notation,
                                !notation_line_is_main(&data.notation)))
                            break;
                    }else{
                        if(notation_move_index_get(&data.notation) == 1
                                && !notation_line_is_main(&data.notation)){
                            tmp = variation_index_find(
                                    data.notation.line_current,
                                    data.notation.line_current->prev);
                            data.notation.line_current = data.notation.line_current->prev;
                            if(tmp != -1)
                                data.notation.line_current->move_current = tmp;
                        }
                        else{
                            if(!variation_move_prev(data.notation.line_current))
                                break;
                        }
                    }
                    notation_focus_current_move(&data);
                    machine_position(&data.notation);
                    cb_hidden = none;
                    draw_render(&data);
                    break;

                case SDLK_k:
                case SDLK_UP:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(notation_line_is_main(&data.notation))
                            break;
                        tmp = move_variation_find(variation_move_get(
                                    data.notation.line_current->prev),
                                data.notation.line_current);
                        if(tmp == -1 || tmp < 1)
                            break;
                        data.notation.line_current = variation_move_get(
                                data.notation.line_current->prev)->variation_list[tmp-1];
                        data.notation.line_current->move_current = 1;
                    }else{
                        if(notation_line_is_main(&data.notation))
                            break;
                        tmp = variation_index_find(data.notation.line_current,
                                data.notation.line_current->prev);
                        data.notation.line_current = data.notation.line_current->prev;
                        if(tmp != -1)
                            data.notation.line_current->move_current = tmp;
                    }
                    notation_focus_current_move(&data);
                    machine_position(&data.notation);
                    cb_hidden = none;
                    draw_render(&data);
                    break;

                case SDLK_j:
                case SDLK_DOWN:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(notation_line_is_main(&data.notation))
                            break;
                        tmp = move_variation_find(variation_move_get(
                                    data.notation.line_current->prev),
                                data.notation.line_current);
                        if(tmp == -1 || tmp + 1 >= variation_move_get(
                                    data.notation.line_current->prev)->variation_count){
                            break;
                        }
                            data.notation.line_current = variation_move_get(
                                    data.notation.line_current->prev)->variation_list[tmp+1];
                            data.notation.line_current->move_current = 1;
                    }else{
                        if(notation_move_get(
                                &data.notation)->variation_count == 0)
                            break;
                        data.notation.line_current = notation_move_get(
                                &data.notation)->variation_list[0];
                        data.notation.line_current->move_current = 1;
                    }
                    notation_focus_current_move(&data);
                    machine_position(&data.notation);
                    cb_hidden = none;
                    draw_render(&data);
                    break;


                case SDLK_PAGEUP:
                    notation_scroll_up(&data);
                    draw_render(&data);
                    break;

                case SDLK_PAGEDOWN:
                    notation_scroll_down(&data);
                    draw_render(&data);
                    break;

                case SDLK_p:
                    undo_add(&data);
                    notation_variation_promote(&data.notation);
                    draw_render(&data);
                    break;

                case SDLK_u:
                    undo_do(&data);
                    draw_render(&data);
                    break;

                case SDLK_d:
                    undo_add(&data);
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        variation_delete_next_moves(data.notation.line_current);
                    }else{
                        notation_variation_delete(&data.notation);
                        machine_position(&data.notation);
                    }
                    cb_hidden = none;
                    draw_render(&data);
                    break;

                case SDLK_v:
                    if(event.key.keysym.mod & KMOD_SHIFT)
                        mode_position(&data);
                    else
                        mode_move(&data);
                    break;

                case SDLK_s:
                    mode_san(&data);
                    break;

                case SDLK_t:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        mode_training(&data);
                    }else{
                        mode_tag(&data);
                    }
                    break;

                case SDLK_g:
                    cb_hidden = none;
                    mode_game_list(&data);
                    break;

                case SDLK_m:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        if(machine_list[1].running){
                            machine_stop(1);
                            machine_list[1].output[0] = '\0';
                        }else{
                            machine_start(&data, 1);
                        }
                    }else{
                        if(machine_list[0].running){
                            machine_stop(0);
                            machine_list[0].output[0] = '\0';
                        }else{
                            machine_start(&data, 0);
                        }
                    }
                    draw_render(&data);
                    break;

                case SDLK_n:
                    mode_number_edit(&data);
                    break;
                }
                break;

            case SDL_TEXTINPUT:
                switch(event.text.text[0]){
                case 'W':
                    write_game(&data);
                    message_add(&data, &event, "Game written");
                    break;
                }
                break;
            }
        }
    }

    if(strlen(output_type))
        output_game(&data, output_type);
    machine_stop(0);
    machine_stop(1);
    piece_unload();
    window_data_free(&data);
}
