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

void
handle_resize(WindowData *data, SDL_Event *e)
{
    switch(e->window.event){
    case SDL_WINDOWEVENT_SIZE_CHANGED:
        machine_resize(data, -1);
        window_resize(data, e->window.data1, e->window.data2);
        break;

    case SDL_WINDOWEVENT_EXPOSED:
        //fix broken font after lid open
        FC_ResetFontFromRendererReset(data->font, data->renderer, 0);
        window_resize(data, data->window_width, data->window_height);
        break;
    }
}

void
handle_non_input_events(SDL_Event *event, WindowData *data, int *loop)
{
    switch (event->type) {
    case SDL_KEYUP:
        switch(event->key.keysym.sym){
        case SDLK_q:
            data->loop = 0;
            if(loop != NULL)
                *loop = 0;
            break;
        }
        break;

    case SDL_TEXTINPUT:
        switch(event->text.text[0]){
        case 'z':
            data->conf.square_size += 10;
            window_resize(data, data->window_width, data->window_height);
            data->draw_render(data);
            break;

        case 'Z':
            data->conf.square_size -= 10;
            if(data->conf.square_size < data->conf.minimal_square)
                data->conf.square_size = data->conf.minimal_square;
            window_resize(data, data->window_width, data->window_height);
            data->draw_render(data);
            break;
        }
        break;
    }
}
