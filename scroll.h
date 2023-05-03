#ifndef _SCROLL_H_
#define _SCROLL_H_

#include <stdlib.h>

typedef struct {
    int value;
    int step;
    int length;
    int shown;
    int max;
} Scroll;

void scroll_init(Scroll *scroll);
void scroll_up(Scroll *scroll);
void scroll_down(Scroll *scroll);
void scroll_set_length(Scroll *scroll, int length);
void scroll_set_max(Scroll *scroll, int font_size);

#endif
