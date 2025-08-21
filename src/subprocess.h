#ifndef _SUBPROCESS_H_
#define _SUBPROCESS_H_

#include "libs.h"
#include "config.h"

typedef struct {
    int running;
    SDL_Process* process;
    SDL_Thread* read_thread;
    SDL_Thread* write_thread;
} Subprocess;

int subprocess_start(Subprocess* sp, char** args);
void subprocess_stop(Subprocess* sp);

#endif
