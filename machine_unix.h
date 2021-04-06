#ifndef _MACHINE_UNIX_H_
#define _MACHINE_UNIX_H_

#include <unistd.h>
#include <signal.h>
#include "window.h"

typedef struct WindowData WindowData;

int machine_read(void *data);
int machine_write(void *data);
void machine_start(WindowData *data, int index);
void machine_stop(int index);

#endif
