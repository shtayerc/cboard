#include "libs.h"
#include <SDL3/SDL_main.h>
#include <stdio.h>

#include "event.h"
#include "explorer.h"
#include "game_list.h"
#include "machine.h"
#include "message.h"
#include "nag.h"
#include "normal.h"
#include "notation.h"
#include "status.h"
#include "window.h"

#define CHESS_UTILS_IMPLEMENTATION
#include "chess_utils.h"

#define VERSION "0.3"

char fen[FEN_LEN];

void
output_game(WindowData* data, char* type) {
    if (!strcmp(type, "pgn")) {
        pgn_write_file(stdout, &data->game);
    } else if (!strcmp(type, "fen")) {
        board_fen_export(&game_move_get(&data->game)->board, fen);
        fprintf(stdout, "%s\n", fen);
    }
}

void
usage() {
    fprintf(stdout, "Usage: cboard [OPTIONS] [file.pgn]\n"
                    "\nOPTIONS\n"
                    " -c --config <file>      Config to use (default ~/.config/cboard/config)\n"
                    " -f --font <file>        Font to use\n"
                    "    --FEN-start <fen>    Starting position\n"
                    "    --FEN-find <fen>     Set current move for position\n"
                    " -h --help               Print help\n"
                    " -n --number <number>    Game number\n"
                    " -o --output fen|pgn     Output to stdout\n"
                    " -p --pieces <dir>       Pieces to use\n"
                    " -v --version            Print version\n");
}

int
main(int argc, char* argv[]) {
    snprintf(fen, FEN_LEN, "%s", FEN_DEFAULT);
    char fen_find[FEN_LEN];
    fen_find[0] = '\0';
    WindowData data;
    window_data_init(&data);
    Board board;
    SDL_Event* tmp_event = NULL;
    FILE* file = NULL;
    char output_type[4] = "";
    int i, number;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--number") || !strcmp(argv[i], "-n")) {
            if (++i == argc) {
                usage();
                return 1;
            }
            snprintf(data.number, data.conf.number_len, "%s", argv[i]);
        } else if (!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o")) {
            if (++i == argc) {
                usage();
                return 1;
            }
            snprintf(output_type, 4, "%s", argv[i]);
        } else if (!strcmp(argv[i], "--FEN-start")) {
            if (++i == argc) {
                usage();
                return 1;
            }
            snprintf(fen, FEN_LEN, "%s", argv[i]);
        } else if (!strcmp(argv[i], "--FEN-find")) {
            if (++i == argc) {
                usage();
                return 1;
            }
            snprintf(fen_find, FEN_LEN, "%s", argv[i]);
        } else if (!strcmp(argv[i], "--font") || !strcmp(argv[i], "-f")) {
            if (++i == argc) {
                usage();
                return 1;
            }
            data.conf.font_path = argv[i];
        } else if (!strcmp(argv[i], "--pieces") || !strcmp(argv[i], "-p")) {
            if (++i == argc) {
                usage();
                return 1;
            }
            data.conf.piece_path = argv[i];
        } else if (!strcmp(argv[i], "--config") || !strcmp(argv[i], "-c")) {
            if (++i == argc) {
                usage();
                return 1;
            }
            data.conf.config_path = argv[i];
        } else if (!strcmp(argv[i], "-")) {
            snprintf(data.filename, data.conf.status_max_len, "-");
            file = stdin;
        } else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
            fprintf(stdout, "cboard %s\n", VERSION);
            return 0;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            usage();
            return 0;
        } else if (i == argc - 1) {
            snprintf(data.filename, data.conf.status_max_len, "%s", argv[i]);
        } else {
            usage();
            return 1;
        }
    }

    board_fen_import(&board, fen);
    game_init_default(&data.game, &board);

    if (file == NULL) {
        file = fopen(data.filename, "r");
    }
    if (isubstr(data.filename, data.conf.rotate_str)) {
        data.rotation = RotationBlack;
    }
    if (strcmp(data.number, "a") || !strcmp(data.filename, "-")) {
        number = strtol(data.number, NULL, 10);
        if (file != NULL && !pgn_read_file(file, &data.game, number)) {
            fprintf(stderr, "Invalid pgn\n");
            return 1;
        }
    } else {
        if (file != NULL) {
            game_list_init(&data.game_list);
            game_list_read_pgn(&data.game_list, file);
            game_list_sort(&data.game_list, data.game_list_sort_tag, str2sorting(data.game_list_sort_direction));
            game_list_current_init(&data);
            if (data.game_list.ai.count == 1) {
                snprintf(data.number, data.conf.number_len, "%s", "0");
                fseek(file, 0, SEEK_SET);
                if (!pgn_read_file(file, &data.game, 0)) {
                    fprintf(stderr, "Invalid pgn\n");
                    return 1;
                }
            } else {
                tmp_event = (SDL_Event*)malloc(sizeof(SDL_Event));
                SDL_zero(*tmp_event);
                tmp_event->type = SDL_EVENT_KEY_UP;
                tmp_event->key.key = SDLK_G;
                tmp_event->key.mod = SDL_KMOD_NONE;
            }
        }
    }

    if (strcmp(data.filename, "-") && file != NULL) {
        fclose(file);
    }

    window_open(&data);
    window_set_title(&data);
    window_update_title(&data);
    if (tmp_event != NULL) {
        SDL_PushEvent(tmp_event);
        SDL_free(tmp_event);
    } else if (strlen(fen_find) > 0) {
        board_fen_import(&board, fen_find);
        game_board_find(&data.game, &board);
    }
    SDL_MaximizeWindow(data.window);
    window_calculate_content_size(&data);
    snprintf(data.status.mode, data.conf.status_max_len, "%s", data.conf.normal_status);
    handle_position_change(&data);
    piece_load(&data);

    mode_normal(&data);

    if (strlen(output_type)) {
        output_game(&data, output_type);
    }
    machine_stop(&data, 0);
    machine_stop(&data, 1);
    explorer_stop(&data);
    piece_unload();
    window_data_free(&data);
    return 0;
}
