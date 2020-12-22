#include "event.h"

void
handle_global_events(SDL_Event *event, WindowData *data, int *loop, int draw)
{
    switch (event->type) {
    case SDL_QUIT:
        data->loop = 0;
        if(loop != NULL)
            *loop = 0;
        break;

    case SDL_WINDOWEVENT:
        handle_resize(data, event);
        if(draw)
            data->draw_render(data);
        break;

    case SDL_MOUSEMOTION:
        data->mouse.x = event->button.x;
        data->mouse.y = event->button.y;
        break;

    case SDL_MOUSEBUTTONUP:
    case SDL_KEYUP:
        message_clear(data, event);
        break;
    }
}

void
handle_input_events(SDL_Event *event, WindowData *data, int *loop, int *pos,
        char *str, int len)
{
    switch (event->type) {
    case SDL_KEYUP:
        switch (event->key.keysym.sym) {
        case SDLK_ESCAPE:
            *loop = 0;
            textedit_escape(pos, str);
            snprintf(data->status.mode, data->conf.status_max_len, "%s",
                    data->conf.normal_status);
            break;

        case SDLK_BACKSPACE:
            textedit_backspace(pos, str, len, data);
            data->draw_render(data);
            break;

        case SDLK_DELETE:
            textedit_delete(pos, str);
            data->draw_render(data);
            break;

        case SDLK_LEFT:
            textedit_left(pos, str, len, data);
            data->draw_render(data);
            break;

        case SDLK_RIGHT:
            textedit_right(pos, str, len, data);
            data->draw_render(data);
            break;

        case SDLK_u:
            if(event->key.keysym.mod & KMOD_CTRL){
                textedit_delete_all(pos, str, len, data);
                data->draw_render(data);
            }
            break;
        }
        break;

    case SDL_TEXTINPUT:
        textedit_input(pos, str, len, data, event->text.text);
        data->draw_render(data);
        break;
    }
}

unsigned int
SDL_Event_timestamp(SDL_Event *event)
{
    switch(event->type){
    case SDL_WINDOWEVENT:
        return event->window.timestamp;

    case SDL_KEYDOWN:
    case SDL_KEYUP:
        return event->key.timestamp;

    case SDL_TEXTEDITING:
        return event->edit.timestamp;

    case SDL_TEXTINPUT:
        return event->text.timestamp;

    case SDL_MOUSEMOTION:
        return event->motion.timestamp;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        return event->button.timestamp;

    default:
        return 0;
    }
}

void
message_add(WindowData *data, SDL_Event *event, char *msg)
{
    snprintf(data->status.info, data->conf.status_max_len, "%s", msg);
    data->message = 1;
    data->message_timestamp = SDL_Event_timestamp(event);
    data->draw_render(data);
}

void
message_clear(WindowData*data, SDL_Event *event)
{
    int diff = SDL_Event_timestamp(event) - data->message_timestamp;
    if(data->message == 1 && abs(diff) > data->conf.message_duration){
        data->status.info[0] = '\0';
        data->message = 0;
        data->draw_render(data);
    }
}
