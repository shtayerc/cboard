#include "scroll.h"
#include <stdio.h>
#include <stdlib.h>

void
scroll_init(Scroll* scroll) {
    scroll->value = 0;
    scroll->step = 0;
    scroll->length = 0;
}

void
scroll_up(Scroll* scroll) {
    scroll->value += scroll->step;
    if (scroll->value > 0) {
        scroll->value = 0;
    }
}

void
scroll_down(Scroll* scroll) {
    scroll->value -= scroll->step;
    if (scroll->length != 0 && scroll->value < scroll->max) {
        scroll->value = scroll->max;
    }
    if (scroll->length < scroll->shown) {
        scroll->value = 0;
    }
}

void
scroll_set_length(Scroll* scroll, int length) {
    //Scroll content length is calculated from current scroll value, because
    //we start drawing at y=0.
    scroll->length = abs(length - scroll->value);
}

void
scroll_set_max(Scroll* scroll, int font_size) {
    scroll->max = -(scroll->length - scroll->shown) - font_size;
}
