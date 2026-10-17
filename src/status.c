#include "status.h"

void
mode_filename_edit(WindowData* data) {
    int loop = 1;
    SDL_Event event;
    FILE* f;
    char last[data->conf.status_max_len];
    snprintf(last, data->conf.status_max_len, "%s", data->filename->text);
    int pos = SDL_utf8strlen(data->filename->text);
    text_cursor_set(data->filename, pos);
    data->mode = ModeFilename;
    SDL_StartTextInput(data->window);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events_new(&event, data, &loop, data->filename);
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            f = fopen(data->filename->text, "r");
                            if (f == NULL) {
                                TTF_SetTextString(data->filename, last, 0);
                            } else {
                                fclose(f);
                                if (isubstr(data->filename->text, data->conf.rotate_str)) {
                                    data->rotation = RotationBlack;
                                } else {
                                    data->rotation = RotationWhite;
                                }
                                window_set_title(data);
                                window_update_title(data);
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
    char* non_int;
    char last[data->conf.number_len];
    snprintf(last, data->conf.number_len, "%s", data->number->text);
    int pos = SDL_utf8strlen(data->number->text);
    text_cursor_set(data->number, pos);
    data->mode = ModeNumber;
    SDL_StartTextInput(data->window);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            //textinput event needs special treatment
            if (event.type != SDL_EVENT_TEXT_INPUT) {
                handle_input_events_new(&event, data, &loop, data->number);
            }
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            strtol(data->number->text, &non_int, 10);
                            if ((strcmp(non_int, "") && strcmp(data->number->text, "a")) || SDL_utf8strlen(data->number->text) == 0) {
                                TTF_SetTextString(data->number, last, 0);
                            }
                            draw_render(data);
                            break;
                    }
                    break;

                case SDL_EVENT_TEXT_INPUT:
                    if (strchr("0123456789", event.text.text[0])
                        || (SDL_utf8strlen(data->number->text) == SDL_utf8strlen(data->conf.cursor) && event.text.text[0] == 'a')) {
                        TTF_AppendTextString(data->number, event.text.text, 0);
                        text_cursor_set(data->number, pos + text_cursor_next_length(data->number, pos));
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
    TTF_SetTextString(data->status_info, msg, 0);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_Y:
                        case SDLK_Z: return 1;

                        default: loop = 0; break;
                    }
                    break;
            }
        }
    }
    TTF_SetTextString(data->status_info, "", 0);
    draw_render(data);
    return 0;
}

void
status_draw(WindowData* data) {
    SDL_Rect rect;
    rect = draw_ttf_text(data, &data->layout.status, data->layout.status.rect, TextWrapRow, TextElementStatus, data->conf.status_name[data->mode]);
    rect.x += rect.w;
    rect.y -= rect.h;
    rect = draw_ttf_text(data, &data->layout.status, rect, TextWrapNewLine, TextElementStatus, data->filename);
    rect = draw_text(data, &data->layout.status, rect, TextWrapNewLine, TextElementStatus, "[");
    rect = draw_ttf_text(data, &data->layout.status, rect, TextWrapNewLine, TextElementStatus, data->number);
    rect = draw_text(data, &data->layout.status, rect, TextWrapNewLine, TextElementStatus, "]");
    rect = draw_ttf_text(data, &data->layout.status, rect, TextWrapNewLine, TextElementStatus, data->status_info);
}
