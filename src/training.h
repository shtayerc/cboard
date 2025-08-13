#ifndef TRAINING_H_
#define TRAINING_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"
#include "chessboard.h"
#include "message.h"
#include "textedit.h"
#include "window.h"

void training_repeat(WindowData* data, Variation* v, int move_number, int* vs_index);
void training_next(WindowData* data, Variation* v, int move_number, int* vs_index, Color color, int* gl_index);
void mode_training(WindowData* data);

#endif
