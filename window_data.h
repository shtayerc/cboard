#ifndef _WINDOW_DATA_H_
#define _WINDOW_DATA_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"
#include "config.h"

typedef struct {
    SDL_Rect board;
    SDL_Rect square;
    SDL_Rect notation;
    SDL_Rect status;
    SDL_Rect machine;
} Layout;

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
    int font_size;
    char **machine_cmd_list[2];
    char **machine_uci_list[2];
    SDL_Color status_font_color;
    SDL_Color notation_font_color;
    SDL_Color notation_current_fg;
    SDL_Color comment_font_color;
    SDL_Color variation_font_color;
} Config;

typedef struct {
    char *str;
    char *mode;
    char *info;
} StatusLine;

typedef enum {
    RotationWhite = 0, RotationBlack = 7
} Rotation;

typedef struct WindowData WindowData;

struct WindowData {
    SDL_Window *window;
    int window_width;
    int window_height;
    char *filename;
    char *number;
    char *info;
    SDL_Renderer *renderer;
    SDL_Point mouse;
    Layout layout;
    FC_Font *font;
    int notation_scroll;
    int notation_hidden;
    int game_list_show;
    int game_list_scroll;
    int loop;
    StatusLine status;
    Config conf;
    Board board;
    Notation notation;
    GameList game_list;
    int game_list_current;
    Notation *undo_list[UNDO_COUNT];
    int undo_current;
    Notation *redo_list[UNDO_COUNT];
    int redo_current;
    Rotation rotation;
    void (*draw)(WindowData *);
    void (*draw_render)(WindowData *);
};

int file_exists(const char *filename);
Config config_init();
void window_data_init(WindowData *data);
void window_open(WindowData *data, const char *fen);
void window_data_free(WindowData *data);
void window_resize(WindowData *data, int width, int height);
void handle_resize(WindowData *data, SDL_Event *event);
void undo_init(Notation *list[]);
void undo_add(WindowData *data);
void undo_do(WindowData *data);
void redo_add(WindowData *data);
void redo_do(WindowData *data);
void undo_free(Notation *list[]);
void load_machine_config(WindowData *data);
void free_machine_config(WindowData *data);

#endif
