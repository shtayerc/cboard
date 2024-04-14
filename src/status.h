#ifndef _STATUS_H_
#define _STATUS_H_

#include "event.h"
#include "window.h"

void mode_filename_edit(WindowData* data);
void mode_number_edit(WindowData* data);
int mode_confirm(WindowData* data, const char* msg);
void status_draw(WindowData* data);

#endif
