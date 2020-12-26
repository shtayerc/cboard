#ifndef _NOTATION_H_
#define _NOTATION_H_

#include "window_data.h"
#include "chess_utils.h"
#include "nag.h"
#include "event.h"

typedef struct {
    int x;
    int y;
    int w;
    int h;
    int index;
    Move *move;
    Variation *variation;
} MoveCoord;

extern MoveCoord *nt_move_coords;
extern int nt_move_coord_len;
extern int nt_move_coord_index;

void notation_handle_line_break(WindowData *data, int *x, int *y,
        int word_width, int x_start);
void comment_draw(WindowData *data, Move *m, int *x, int *y, int x_start);
void variation_draw(WindowData *data, Variation *v,  int *x, int *y,
        int x_start, int i);
int notation_click(WindowData *data);
int notation_move_find(WindowData *data);
void notation_draw_tags(WindowData *data, int *x, int *y, int x_start);
void notation_draw(WindowData *data);
void notation_scroll_up(WindowData *data);
void notation_scroll_down(WindowData *data);
void mode_annotate(WindowData *data, Move *move);
void mode_move(WindowData *data);
void mode_position(WindowData *data);
void mode_tag_edit(WindowData *data, Tag *tag);
void mode_tag(WindowData *data);
void mode_clipboard(WindowData *data);
void game_init(Notation *n, Board *b);

#endif
