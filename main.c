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
#include "normal.h"

#define CHESS_UTILS_IMPLEMENTATION
#include "chess_utils.h"

#define VERSION "0.3"

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

    window_open(&data);
    window_resize(&data, data.conf.default_width, data.conf.default_height);
    snprintf(data.status.mode, data.conf.status_max_len, "%s",
            data.conf.normal_status);
    machine_position(&data.notation);
    piece_load(&data);

    mode_normal(&data);

    if(strlen(output_type))
        output_game(&data, output_type);
    machine_stop(0);
    machine_stop(1);
    piece_unload();
    window_data_free(&data);
    return 0;
}
