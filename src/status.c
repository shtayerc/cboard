#include "status.h"

void
mode_filename_edit(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    FILE* f;
    char last[data->conf.status_max_len];
    snprintf(last, data->conf.status_max_len, "%s", data->filename);
    int pos = U8_strlen(data->filename);
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.filename_status);
    cursor_add(&pos, data->filename, data->conf.status_max_len, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, data->filename, data->conf.status_max_len);
            switch (event.type) {
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            f = fopen(data->filename, "r");
                            if (f == NULL) {
                                snprintf(data->filename, data->conf.status_max_len, "%s", last);
                            } else {
                                fclose(f);
                                if (isubstr(data->filename, data->conf.rotate_str)) {
                                    data->rotation = RotationBlack;
                                } else {
                                    data->rotation = RotationWhite;
                                }
                            }
                            draw_render(data);
                            break;
                    }
                    break;
            }
        }
    }
}

void
mode_number_edit(WindowData* data) {
    SDL_Event event;
    int loop = 1;
    char last[data->conf.number_len];
    char* non_int;
    int pos = U8_strlen(data->number);
    snprintf(last, data->conf.number_len, "%s", data->number);
    snprintf(data->status.mode, data->conf.status_max_len, "%s", data->conf.number_status);
    cursor_add(&pos, data->number, data->conf.number_len, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            //textinput event needs special treatment
            if (event.type != SDL_TEXTINPUT) {
                handle_input_events(&event, data, &loop, &pos, data->number, data->conf.number_len);
            }
            switch (event.type) {
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            strtol(data->number, &non_int, 10);
                            if ((strcmp(non_int, "") && strcmp(data->number, "a")) || U8_strlen(data->number) == 0) {
                                snprintf(data->number, data->conf.number_len, "%s", last);
                            }
                            draw_render(data);
                            break;
                    }
                    break;

                case SDL_TEXTINPUT:
                    if (strchr("0123456789", event.text.text[0])
                        || (U8_strlen(data->number) == U8_strlen(data->conf.cursor) && event.text.text[0] == 'a')) {
                        textedit_input(&pos, data->number, data->conf.number_len, data, event.text.text);
                        draw_render(data);
                    }
                    break;
            }
        }
    }
}

int
mode_confirm(WindowData* data, const char* msg) {
    SDL_Event event;
    int loop = 1;
    snprintf(data->status.info, data->conf.status_max_len, "%s", msg);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_y:
                        case SDLK_z: return 1;

                        default: loop = 0; break;
                    }
                    break;
            }
        }
    }
    data->status.info[0] = '\0';
    draw_render(data);
    return 0;
}

void
status_draw(WindowData* data) {
    SDL_Color c = data->conf.colors[ColorStatusBackground];
    SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(data->renderer, &data->layout.status);
    snprintf(data->status.str, data->conf.status_max_len, "%s %s[%s] %s", data->status.mode, data->filename,
             data->number, data->status.info);
    FC_DrawColor(data->font, data->renderer, data->layout.status.x + STATUS_PADDING_LEFT, data->layout.status.y,
                 data->conf.colors[ColorStatusFont], data->status.str);
}
