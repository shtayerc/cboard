#include "message.h"

unsigned int
SDL_Event_timestamp(SDL_Event* event) {
    switch (event->type) {
        case SDL_WINDOWEVENT: return event->window.timestamp;

        case SDL_KEYDOWN:
        case SDL_KEYUP: return event->key.timestamp;

        case SDL_TEXTEDITING: return event->edit.timestamp;

        case SDL_TEXTINPUT: return event->text.timestamp;

        case SDL_MOUSEMOTION: return event->motion.timestamp;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: return event->button.timestamp;

        default: return 0;
    }
}

void
message_add(WindowData* data, SDL_Event* event, char* msg) {
    snprintf(data->status.info, data->conf.status_max_len, "%s", msg);
    data->message = 1;
    data->message_timestamp = SDL_Event_timestamp(event);
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
