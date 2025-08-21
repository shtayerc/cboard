#include "subprocess.h"

int
subprocess_start(Subprocess* sp, char** args) {
    if (sp->running) {
        return 0;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetPointerProperty(props, SDL_PROP_PROCESS_CREATE_ARGS_POINTER, (void*)args);
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDOUT_NUMBER, SDL_PROCESS_STDIO_APP);
    SDL_SetNumberProperty(props, SDL_PROP_PROCESS_CREATE_STDIN_NUMBER, SDL_PROCESS_STDIO_APP);
    SDL_SetBooleanProperty(props, SDL_PROP_PROCESS_CREATE_BACKGROUND_BOOLEAN, true);
    sp->process = SDL_CreateProcessWithProperties(props);
    if (!sp->process) {
        return 0;
    }
    return 1;
}

void
subprocess_stop(Subprocess* sp) {
    if (sp->running) {
        sp->running = 0;
        if (!SDL_KillProcess(sp->process, false)) {
            SDL_KillProcess(sp->process, true);
        }
        SDL_WaitThread(sp->read_thread, NULL);
        SDL_WaitThread(sp->write_thread, NULL);
        SDL_DestroyProcess(sp->process);
    }
}

