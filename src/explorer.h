#ifndef _EXPLORER_H_
#define _EXPLORER_H_

#include "libs.h"
#include "config.h"
#include "subprocess.h"

typedef struct {
    char output[BUFFER_LEN];
    char fen[FEN_LEN];
    char** row_list;
    int row_count;
    int fen_changed;
    int event;
    Subprocess sp;
} Explorer;

#include "window.h"

typedef struct WindowData WindowData;

void explorer_draw(WindowData* data);
void explorer_init(Explorer* e);
void explorer_free(Explorer* e);
void explorer_row_add(Explorer* e, char* row);
void explorer_row_free(Explorer* e);
void explorer_position(WindowData* data);
void explorer_event(WindowData* data, int event, int clear);
void explorer_parse_str(Explorer* e, char* str);
int explorer_read(void* p);
int explorer_write(void* p);
int explorer_start(WindowData* data, int index);
void explorer_stop(WindowData* data);

#endif
