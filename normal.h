#ifndef NORMAL_H_
#define NORMAL_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"
#include "window.h"
#include "chessboard.h"
#include "notation.h"
#include "status.h"
#include "machine.h"
#include "nag.h"
#include "game_list.h"
#include "event.h"
#include "message.h"

void write_game(WindowData *data);
void mode_normal(WindowData *data);

#endif
