#ifndef TRAINING_H_
#define TRAINING_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"

typedef struct {
    int vs_index;
    int vs_current;
    int vs_count;
    int gl_index;
} TrainingStat;

#include "window.h"
#include "chessboard.h"
#include "message.h"
#include "textedit.h"

void ts_init(TrainingStat* ts);
void training_repeat(WindowData* data, Variation* v, int move_number);
void training_next(WindowData* data, Variation* v, int move_number, Color color);
void mode_training(WindowData* data);
void training_draw(WindowData* data);

#endif
