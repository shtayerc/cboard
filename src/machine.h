#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "window.h"
#include "subprocess.h"

typedef struct WindowData WindowData;

typedef struct {
    int index;
    WindowData* data;
} MachineData;

typedef struct Machine Machine;

struct Machine {
    char output[MACHINE_OUTPUT_LEN];
    Variation* line;
    int line_count;
    int* depth;
    UciScoreType* type;
    int* score;
    char fen[FEN_LEN];
    Subprocess sp;
    Board board;
    MachineData md;
    int fen_changed;
};

void machine_draw(WindowData* data);
void machine_position(WindowData* data);
void machine_line_init(Machine* m, Board* b);
void machine_line_free(Machine* m);
void machine_line_parse(WindowData* data, int index);
void machine_config_load(WindowData* data);
void machine_config_free(WindowData* data);
void machine_set_line_count(WindowData* data, int index);
void machine_resize(WindowData* data, int index);
int machine_running_count(WindowData* data);
int machine_read(void* p);
int machine_write(void* p);
void machine_start(WindowData* data, int index);
void machine_stop(WindowData* data, int index);

#endif
