#ifndef _MACHINE_H_
#define _MACHINE_H_

#include <unistd.h>
#include <signal.h>
#include "window_data.h"

#define MACHINE_COUNT 2
#define LINE_COUNT 3

typedef struct{
    char output[MACHINE_OUTPUT_LEN];
    int running;
    Variation *line;
    int line_count;
    int *depth;
    UciScoreType *type;
    int *score;
    char fen[FEN_LEN];
    int fd_input[2];
    int fd_output[2];
    int pid;
} Machine;

typedef struct{
    int index;
    WindowData *data;
} MachineData;

extern Machine machine_list[2];

void push_user_event();
int machine_control(void *data);
void machine_start(WindowData *data, int index);
void machine_stop(int index);
void machine_draw(WindowData *data);
void machine_position(Board *b);
void machine_line_init(Machine *m, Board *b);
void machine_line_free(Machine *m);
void machine_init(Board *b);
void machine_free();

#endif
