#ifndef _EVENT_H_
#define _EVENT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"
#include "window_data.h"
#include "textedit.h"

void handle_global_events(SDL_Event *event, WindowData *data, int *loop,
        int draw);
void handle_input_events(SDL_Event *event, WindowData *data, int *loop,
        int *pos, char *str, int len);
void message_add(WindowData *data, SDL_Event *event, char *msg);
void message_clear(WindowData*data, SDL_Event *event);
unsigned int SDL_Event_timestamp(SDL_Event *event);

#endif
