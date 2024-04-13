#ifndef _WIN32
#ifndef _MACHINE_UNIX_H_
#define _MACHINE_UNIX_H_

//#include <unistd.h>
#include <signal.h>
#include "window.h"
#include "unix.h"

typedef struct WindowData WindowData;

int machine_read(void *p);
int machine_write(void *p);
void machine_start(WindowData *data, int index);
void machine_stop(WindowData *data, int index);

#endif
#endif
