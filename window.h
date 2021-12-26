#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"
#include "config.h"
#include "explorer.h"
#include "machine.h"
#include "notation.h"
#include "status.h"
#include "game_list.h"

typedef struct {
    SDL_Rect board;
    SDL_Rect square;
    SDL_Rect notation;
    SDL_Rect status;
    SDL_Rect machine;
} Layout;

typedef enum{
    ColorSquareWhite,
    ColorSquareBlack,
    ColorSquareInactive,
    ColorSquareActive,
    ColorSquareWhiteLast,
    ColorSquareBlackLast,
    ColorStatusBackground,
    ColorStatusFont,
    ColorNotationBackground,
    ColorNotationFont,
    ColorNotationActiveBackground,
    ColorNotationActiveFont,
    ColorCommentFont,
    ColorVariationFont,
    ColorMachineBackground,
    ColorMachineFont,
    ColorCount, //this should always be the last
} ColorIndex;

typedef struct {
    int path_max_len;
    int status_max_len;
    int number_len;
    int square_size;
    int scroll_step;
    int default_width;
    int default_height;
    int minimal_width;
    int minimal_height;
    int minimal_square;
    char *window_title;
    char *normal_status;
    char *edit_status;
    char *annotate_status;
    char *promotion_status;
    char *move_annotation_status;
    char *position_annotation_status;
    char *tag_status;
    char *filename_status;
    char *number_status;
    char *clipboard_status;
    char *san_status;
    char *training_status;
    char *game_list_status;
    char *cursor;
    char *piece_path;
    char *font_path;
    char *config_path;
    char *rotate_str;
    char *default_filename;
    int font_size;
    int message_duration;
    char **machine_cmd_list[2];
    char **machine_uci_list[2];
    SDL_Color colors[ColorCount];
    char *explorer_exe;
} Config;

typedef struct {
    char *str;
    char *mode;
    char *info;
} StatusLine;

typedef enum {
    RotationWhite = 0, RotationBlack = 7
} Rotation;

typedef enum {
    ModeMoves, ModeGameList, ModeExplorer, ModeCustomText,
} NotationMode;

typedef enum {
    Ascending, Descending
} Sorting;

typedef struct WindowData WindowData;
typedef struct Machine Machine;

struct WindowData {
    SDL_Window *window;
    int window_width;
    int window_height;
    char *filename;
    char *number;
    char *info;
    char *custom_text;
    SDL_Renderer *renderer;
    SDL_Point mouse;
    Layout layout;
    FC_Font *font;
    int font_height;
    int notation_scroll;
    int notation_hidden;
    NotationMode notation_mode;
    int game_list_scroll;
    int machine_hidden;
    int loop;
    int from_game_list;
    StatusLine status;
    Config conf;
    Game game;
    GameList game_list;
    int game_list_current;
    Sorting game_list_sorting;
    Game *undo_list[UNDO_COUNT];
    int undo_current;
    Game *redo_list[UNDO_COUNT];
    int redo_current;
    Rotation rotation;
    int message;
    unsigned int message_timestamp;
    Piece piece;
    Square hidden;
    Explorer explorer;
    Machine *machine_list[MACHINE_COUNT];
};

int file_exists(const char *filename);
Config config_init();
void window_data_init(WindowData *data);
void window_open(WindowData *data);
void window_data_free(WindowData *data);
void window_resize(WindowData *data, int width, int height);
void draw(WindowData *data);
void draw_render(WindowData *data);

#endif
