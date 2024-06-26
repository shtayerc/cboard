#ifdef _WIN32
#ifndef _EXPLORER_WIN_H_
#define _EXPLORER_WIN_H_

#include "window.h"

typedef struct WindowData WindowData;

int explorer_read(void* p);
int explorer_write(void* p);
int explorer_start(WindowData* data, index int);
void explorer_stop(WindowData* data);

#endif
#endif
