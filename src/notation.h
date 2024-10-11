#ifndef _NOTATION_H_
#define _NOTATION_H_

#include "chess_utils.h"
#include "event.h"
#include "nag.h"
#include "window.h"

typedef struct WindowData WindowData;

typedef struct {
    int x;
    int y;
    int w;
    int h;
    int index;
    Move* move;
    Variation* variation;
} MoveCoord;

extern MoveCoord* nt_move_coords;
extern int nt_move_coord_len;
extern int nt_move_coord_index;

void notation_handle_line_break(WindowData* data, int* x, int* y, int word_width, int x_start);
void comment_draw(WindowData* data, Move* m, int* x, int* y, int x_start);
void variation_draw(WindowData* data, Variation* v, int* x, int* y, int x_start, int i, int recursive);
int notation_click(WindowData* data);
int notation_coord_index_click(WindowData* data);
int notation_coord_index_move(WindowData* data, Move* m);
void notation_draw_tags(WindowData* data, int* x, int* y, int x_start);
void notation_background_draw(WindowData* data);
void notation_draw(WindowData* data);
void mode_annotate(WindowData* data, Move* move);
void mode_move(WindowData* data);
void mode_position(WindowData* data);
void mode_tag_edit(WindowData* data, Tag* tag);
void mode_tag(WindowData* data);
void mode_clipboard(WindowData* data);
void game_init_default(Game* g, Board* b);
void undo_init(Game* list[]);
void undo_add(WindowData* data);
void undo_do(WindowData* data);
void redo_add(WindowData* data);
void redo_do(WindowData* data);
void undo_free(Game* list[]);
void notation_focus_current_move(WindowData* data);
void custom_text_draw(WindowData* data);
void game_list_stat_position(WindowData* data);
void game_list_stat_draw(WindowData* data);

#endif
