#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils_define.h"
#include "window.h"

unsigned int SDL_Event_timestamp(SDL_Event* event);
void message_add(WindowData* data, SDL_Event* event, char* msg);
void message_clear(WindowData* data, SDL_Event* event);

#endif
