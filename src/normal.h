#ifndef NORMAL_H_
#define NORMAL_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"
#include "chessboard.h"
#include "event.h"
#include "game_list.h"
#include "machine.h"
#include "message.h"
#include "nag.h"
#include "notation.h"
#include "status.h"
#include "window.h"

int write_game(WindowData* data);
void mode_normal(WindowData* data);

#endif
