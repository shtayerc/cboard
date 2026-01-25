#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "libs.h"
#include "config.h"
#include "explorer.h"
#include "game_list.h"
#include "machine.h"
#include "notation.h"
#include "scroll.h"
#include "status.h"
#include "training.h"

typedef enum {
    ColorNone,
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
    int top;
    int right;
    int bottom;
    int left;
} PaddingRect;

typedef struct {
    PaddingRect padding;
    SDL_Rect rect;
} LayoutRect;

typedef struct {
    LayoutRect board;
    LayoutRect square;
    LayoutRect notation;
    LayoutRect status;
    LayoutRect machine;
} Layout;

typedef struct {
    PaddingRect padding;
    ColorIndex fg_color;
    ColorIndex bg_color;
} TextElement;

typedef enum {
    TextElementStatus,
    TextElementGameTag,
    TextElementTraining,
    TextElementMoveMainline,
    TextElementMoveVariation,
    TextElementMoveCurrent,
    TextElementMoveComment,
    TextElementCustomText,
    TextElementGameStatRow,
    TextElementGameListRowNormal,
    TextElementGameListRowCurrent,
    TextElementGameListRowColor,
    TextElementGameListRowColorCurrent,
    TextElementGameListStatus,
    TextElementMachineRow,
    TextElementMachineComment,
    TextElementExplorerRow,
    TextElementBoardCoordRowWhite,
    TextElementBoardCoordRowBlack,
    TextElementBoardCoordFileWhite,
    TextElementBoardCoordFileBlack,
    TextElementCount,  //this should always be the last
} TextElementIndex;

typedef enum {
    TextWrapRow,
    TextWrapNewLine,
    TextWrapCutoff,
    TextWrapRight,
    TextWrapBottom,
} TextWrapType;

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
    char window_title[WINDOW_TITLE_LEN];
    char* normal_status;
    char* edit_status;
    char* annotate_status;
    char* promotion_status;
    char* move_annotation_status;
    char* position_annotation_status;
    char* tag_status;
    char* filename_status;
    char* number_status;
    char* clipboard_status;
    char* san_status;
    char* training_status;
    char* game_list_status;
    char* tag_filter_status;
    char* cursor;
    char* piece_path;
    char* font_path;
    char* config_path;
    char* rotate_str;
    char* default_filename;
    int message_duration;
    char** machine_cmd_list[MACHINE_COUNT];
    char** machine_uci_list[MACHINE_COUNT];
    SDL_Color colors[ColorCount];
    TextElement text_elements[TextElementCount];
    char* explorer_exe_list[EXPLORER_EXE_COUNT];
} Config;

typedef struct {
    char* mode;
    char* info;
} StatusLine;

typedef enum { RotationWhite = 0, RotationBlack = 7 } Rotation;

typedef enum {
    ModeMoves,
    ModeGameList,
    ModeExplorer,
    ModeGameListStat,
    ModeTraining,
    ModeCustomText,
} NotationMode;

typedef enum {
    ModeMachine,
    ModeHidden,
    ModeComment,
} MachineMode;

typedef struct WindowData WindowData;
typedef struct Machine Machine;

struct WindowData {
    SDL_Window* window;
    int window_width;
    int window_height;
    char* filename;
    char* number;
    char* info;
    char* custom_text;
    SDL_Renderer* renderer;
    SDL_Point mouse;
    Layout layout;
    FC_Font* font;
    int font_height;
    Scroll notation_scroll;
    NotationMode notation_mode;
    Scroll game_list_scroll;
    MachineMode machine_mode;
    int loop;
    int from_game_list;
    StatusLine status;
    Config conf;
    Game game;
    GameList game_list;
    GameListStat game_list_stat;
    int game_list_current;
    char* game_list_sort_tag;
    char* game_list_sort_direction;
    Game* undo_list[UNDO_COUNT];
    int undo_current;
    Game* redo_list[UNDO_COUNT];
    int redo_current;
    Rotation rotation;
    int font_size;
    int message;
    int board_coord;
    SDL_TimerID message_timer;
    Piece piece;
    Square hidden;
    Explorer explorer;
    Machine* machine_list[MACHINE_COUNT];
    VariationSequence vs;
    TrainingStat ts;
};

int file_exists(const char* filename);
Config config_init();
Layout layout_init();
void window_data_init(WindowData* data);
void window_open(WindowData* data);
void window_data_free(WindowData* data);
void window_resize(WindowData* data, int width, int height);
void window_set_title(WindowData* data);
void window_update_title(WindowData* data);
void window_calculate_content_size(WindowData* data);
void font_init(WindowData* data);
void font_free(WindowData* data);
void font_resize(WindowData* data, int step);
void draw(WindowData* data);
SDL_Rect draw_text(WindowData* data, LayoutRect* bounds, SDL_Rect pos, TextWrapType wrap, TextElementIndex eli, const char* fmt_text, ...);
void draw_background(WindowData* data, SDL_Rect rect, ColorIndex color_index);
void draw_render(WindowData* data);
SDL_Rect pad_layout(LayoutRect *lrect);
int is_null_rect(SDL_Rect rect);

#endif
