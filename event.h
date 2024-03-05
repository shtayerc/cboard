#ifndef _EVENT_H_
#define _EVENT_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils_define.h"
#include "window.h"
#include "textedit.h"
#include "message.h"
#include "chessboard.h"

void handle_global_events(SDL_Event *event, WindowData *data, int *loop,
        int draw);
void handle_input_events(SDL_Event *event, WindowData *data, int *loop,
        int *pos, char *str, int len);
void handle_resize(WindowData *data, SDL_Event *e);
void handle_non_input_events(SDL_Event *event, WindowData *data, int *loop);
void handle_position_change(WindowData *data);
void push_user_event();
int is_keymod(SDL_Event event, int mod);
int is_keymod_shift(SDL_Event event);
int is_keymod_ctrl(SDL_Event event);

#endif
