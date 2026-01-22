#ifndef TRAINING_H_
#define TRAINING_H_

#include "libs.h"

typedef struct {
    int vs_index;
    int vs_current;
    int vs_count;
    int gl_index;
    int mistake_count;
    int mistake_sum;
} TrainingStat;

#include "window.h"
#include "chessboard.h"
#include "message.h"
#include "textedit.h"

void ts_init(TrainingStat* ts);
void training_repeat(WindowData* data, Variation* v, int move_number);
void training_next(WindowData* data, Variation* v, int move_number, Color color);
int training_move_try(WindowData* data, Square src, Square dst, Piece prom_piece);
void mode_training(WindowData* data);
void training_draw(WindowData* data);

#endif
