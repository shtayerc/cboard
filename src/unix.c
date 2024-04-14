#include "unix.h"

void
msleep(int ms) {
    struct timespec ts;
    ts.tv_sec = (int)(ms / 1000);
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, &ts);
}
