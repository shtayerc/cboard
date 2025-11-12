#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "libs.h"
#include "window.h"

void message_add(WindowData* data, char* msg);
Uint32 message_clear(void* userdata, SDL_TimerID timerID, Uint32 interval);

#endif
