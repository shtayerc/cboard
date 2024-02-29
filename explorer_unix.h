#ifndef _WIN32
#ifndef _EXPLORER_UNIX_H_
#define _EXPLORER_UNIX_H_

#include "window.h"

typedef struct WindowData WindowData;

int explorer_read(void *p);
int explorer_write(void *p);
int explorer_start(WindowData *data, int index);
void explorer_stop(WindowData *data);

#endif
#endif
