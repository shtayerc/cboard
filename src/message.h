#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "libs.h"
#include "window.h"

unsigned int SDL_Event_timestamp(SDL_Event* event);
void message_add(WindowData* data, SDL_Event* event, char* msg);
void message_clear(WindowData* data, SDL_Event* event);

#endif
