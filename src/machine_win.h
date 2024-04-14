#ifdef _WIN32
#ifndef _MACHINE_WIN_H_
#define _MACHINE_WIN_H_

#include <windows.h>
#include "window.h"

typedef struct WindowData WindowData;

int machine_read(void* p);
int machine_write(void* p);
void machine_start(WindowData* data, int index);
void machine_stop(WindowData* data, int index);

#endif
#endif
