#include "message.h"

void
message_add(WindowData* data, char* msg) {
    TTF_SetTextString(data->status_info, msg, 0);
    data->message = 1;
    data->message_timer = SDL_AddTimer(
        data->conf.message_duration,
        (SDL_TimerCallback)message_clear,
        data);
    draw_render(data);
}

Uint32
message_clear(void* userdata, SDL_TimerID timerID, Uint32 interval) {
    (void)timerID;
    (void)interval;
    WindowData* data = (WindowData*)userdata;
    TTF_SetTextString(data->status_info, "", 0);
    data->message = 0;
    draw_render(data);
    return 0;
}
