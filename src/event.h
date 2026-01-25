#ifndef _EVENT_H_
#define _EVENT_H_

#include "libs.h"
#include "chessboard.h"
#include "message.h"
#include "textedit.h"
#include "window.h"

//1024 is next gui modifier which we will ignore
//this is used as mask to filter out higher modifiers
#define SUPPORTED_MODS 1023

void handle_global_events(SDL_Event* event, WindowData* data, int* loop, int draw);
void handle_input_events(SDL_Event* event, WindowData* data, int* loop, int* pos, char* str, int len);
void handle_resize(WindowData* data, SDL_Event* e);
void handle_non_input_events(SDL_Event* event, WindowData* data, int* loop);
void handle_position_change(WindowData* data);
void push_user_event();
int is_keymod_none(SDL_Event event);
int is_keymod_shift(SDL_Event event);
int is_keymod_ctrl(SDL_Event event);

#endif
