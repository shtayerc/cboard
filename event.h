#ifndef _EVENT_H_
#define _EVENT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils.h"
#include "window.h"
#include "textedit.h"
#include "message.h"

void handle_global_events(SDL_Event *event, WindowData *data, int *loop,
        int draw);
void handle_input_events(SDL_Event *event, WindowData *data, int *loop,
        int *pos, char *str, int len);
void handle_resize(WindowData *data, SDL_Event *e);

#endif
