#ifdef _WIN32
#ifndef _MACHINE_WIN_H_
#define _MACHINE_WIN_H_

#include <windows.h>
#include "window.h"

typedef struct WindowData WindowData;

int machine_read(void *data);
int machine_write(void *data);
void machine_start(WindowData *data, int index);
void machine_stop(int index);

#endif
#endif
