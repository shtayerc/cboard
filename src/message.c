#include "message.h"

unsigned int
SDL_Event_timestamp(SDL_Event* event) {
    return event->common.timestamp;
}

void
message_add(WindowData* data, SDL_Event* event, char* msg) {
    snprintf(data->status.info, data->conf.status_max_len, "%s", msg);
    data->message = 1;
    data->message_timestamp = event == NULL ? SDL_GetTicks() : SDL_Event_timestamp(event);
    draw_render(data);
}

void
message_clear(WindowData* data, SDL_Event* event) {
    int diff = SDL_Event_timestamp(event) - data->message_timestamp;
    if (data->message == 1 && abs(diff) > data->conf.message_duration) {
        data->status.info[0] = '\0';
        data->message = 0;
        draw_render(data);
    }
}
