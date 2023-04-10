#ifndef _EXPLORER_H_
#define _EXPLORER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "SDL_FontCache.h"
#include "chess_utils_define.h"
#include "config.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <signal.h>
#endif

typedef struct{
    char output[BUFFER_LEN];
    int running;
    char fen[FEN_LEN];
    char **row_list;
    int row_count;
    int fen_changed;
    int event;
    #ifdef _WIN32
    HANDLE fd_input;
    HANDLE fd_output;
    HANDLE pid;
    #else
    int fd_input;
    int fd_output;
    int pid;
    #endif
} Explorer;

#ifdef _WIN32
#include "explorer_win.h"
#else
#include "explorer_unix.h"
#endif
#include "window.h"

typedef struct WindowData WindowData;

void explorer_draw(WindowData *data);
void explorer_init(Explorer *e);
void explorer_free(Explorer *e);
void explorer_row_add(Explorer *e, char *row);
void explorer_row_free(Explorer *e);
void explorer_position(WindowData *data);
void explorer_event(WindowData *data, int event, int clear);
void explorer_parse_str(Explorer *e, char *str);

#endif
