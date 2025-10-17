#include "event.h"

void
handle_global_events(SDL_Event* event, WindowData* data, int* loop, int draw) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            data->loop = 0;
            if (loop != NULL) {
                *loop = 0;
            }
            break;

        case SDL_EVENT_WINDOW_SHOWN:
        case SDL_EVENT_WINDOW_EXPOSED:
        case SDL_EVENT_WINDOW_MOVED:
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        case SDL_EVENT_WINDOW_MINIMIZED:
        case SDL_EVENT_WINDOW_MAXIMIZED:
        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_FOCUS_GAINED:
        case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
            handle_resize(data, event);
            if (draw) {
                draw_render(data);
            }
            break;

        case SDL_EVENT_MOUSE_MOTION:
            data->mouse.x = event->button.x;
            data->mouse.y = event->button.y;
            if (data->piece != Empty) {
                draw_render(data);
            }
            break;

        case SDL_EVENT_USER:
            if (draw) {
                draw_render(data);
            }
            break;
    }
}

void
handle_input_events(SDL_Event* event, WindowData* data, int* loop, int* pos, char* str, int len) {
    switch (event->type) {
        case SDL_EVENT_KEY_UP:
            switch (event->key.key) {
                case SDLK_ESCAPE:
                    *loop = 0;
                    textedit_escape(pos, str);
                    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.normal_status);
                    SDL_StopTextInput(data->window);
                    break;

                case SDLK_BACKSPACE:
                    textedit_backspace(pos, str, len, data);
                    draw_render(data);
                    break;

                case SDLK_DELETE:
                    textedit_delete(pos, str);
                    draw_render(data);
                    break;

                case SDLK_LEFT:
                    textedit_left(pos, str, len, data);
                    draw_render(data);
                    break;

                case SDLK_RIGHT:
                    textedit_right(pos, str, len, data);
                    draw_render(data);
                    break;

                case SDLK_U:
                    if (event->key.mod & SDL_KMOD_CTRL) {
                        textedit_delete_all(pos, str, len, data);
                        draw_render(data);
                    }
                    break;
            }
            break;

        case SDL_EVENT_TEXT_INPUT:
            textedit_input(pos, str, len, data, event->text.text);
            draw_render(data);
            break;
    }
}

void
handle_resize(WindowData* data, SDL_Event* e) {
    switch (e->type) {
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            machine_resize(data, -1);
            window_resize(data, e->window.data1, e->window.data2);
            break;

        case SDL_EVENT_WINDOW_EXPOSED:
            //fix broken font after lid open
            FC_ResetFontFromRendererReset(data->font, data->renderer, 0);
            window_resize(data, data->window_width, data->window_height);
            break;
    }
}

void
handle_non_input_events(SDL_Event* event, WindowData* data, int* loop) {
    switch (event->type) {
        case SDL_EVENT_KEY_UP:
            switch (SDL_GetKeyFromScancode(event->key.scancode, event->key.mod, false)) {
                case '+':
                    font_resize(data, FONT_STEP);
                    window_resize(data, data->window_width, data->window_height);
                    draw_render(data);
                    break;

                case '-':
                    font_resize(data, -FONT_STEP);
                    window_resize(data, data->window_width, data->window_height);
                    draw_render(data);
                    break;
            }
            switch (event->key.key) {
                case SDLK_Q:
                    if (!mode_confirm(data, "Quit (y/N)")) {
                        break;
                    }
                    data->loop = 0;
                    if (loop != NULL) {
                        *loop = 0;
                    }
                    break;

                case SDLK_R:
                    if (event->key.mod == SDL_KMOD_NONE) {
                        rotation_toggle(data);
                        draw_render(data);
                    }
                    break;

                case SDLK_Z:
                    if (is_keymod_shift(*event)) {
                        data->conf.square_size -= 10;
                        if (data->conf.square_size < data->conf.minimal_square) {
                            data->conf.square_size = data->conf.minimal_square;
                        }
                    } else {
                        data->conf.square_size += 10;
                    }
                    window_resize(data, data->window_width, data->window_height);
                    draw_render(data);
            }
            break;
    }
}

void
handle_position_change(WindowData* data) {
    machine_position(data);
    explorer_position(data);
    if (data->notation_mode == ModeGameListStat) {
        game_list_stat_position(data);
    }
}

void
push_user_event() {
    SDL_Event event;
    SDL_zero(event);
    event.type = SDL_EVENT_USER;
    SDL_PushEvent(&event);
}

int
is_keymod(SDL_Event event, int mod) {
    return event.key.mod == mod;
}

int
is_keymod_shift(SDL_Event event) {
    return event.key.mod & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT);
}

int
is_keymod_ctrl(SDL_Event event) {
    return event.key.mod & (SDL_KMOD_LSHIFT | SDL_KMOD_RSHIFT);
}
