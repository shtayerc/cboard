#include "status.h"

void
mode_filename_edit(WindowData *data)
{
    int loop = 1;
    SDL_Event event;
    FILE *f;
    char last[data->conf.status_max_len];
    snprintf(last, data->conf.status_max_len, "%s", data->filename);
    int pos = U8_strlen(data->filename);
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.filename_status);
    cursor_add(&pos, data->filename, data->conf.status_max_len, data);
    data->draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                loop = 0;
                data->loop = 0;
                break;

            case SDL_WINDOWEVENT:
                handle_resize(data, &event);
                data->draw_render(data);
                break;

            case SDL_MOUSEMOTION:
                data->mouse.x = event.button.x;
                data->mouse.y = event.button.y;
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    textedit_escape(&pos, data->filename);
                    f = fopen(data->filename, "r");
                    if(f == NULL)
                        snprintf(data->filename, data->conf.status_max_len,
                                "%s", last);
                    else
                        fclose(f);
                    data->draw_render(data);
                    break;

                case SDLK_BACKSPACE:
                    textedit_backspace(&pos, data->filename,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;

                case SDLK_DELETE:
                    textedit_delete(&pos, data->filename);
                    data->draw_render(data);
                    break;

                case SDLK_LEFT:
                    textedit_left(&pos, data->filename,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;

                case SDLK_RIGHT:
                    textedit_right(&pos, data->filename,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;
                }
                break;

            case SDL_TEXTINPUT:
                textedit_input(&pos, data->filename, data->conf.status_max_len,
                        data, event.text.text);
                data->draw_render(data);
                break;
            }
        }
    }
}

void
mode_number_edit(WindowData *data)
{
    SDL_Event event;
    int loop = 1;
    char last[data->conf.number_len];
    char *non_int;
    int pos = U8_strlen(data->number);
    snprintf(last, data->conf.number_len, "%s", data->number);
    snprintf(data->status.mode, data->conf.status_max_len,
            "%s", data->conf.number_status);
    cursor_add(&pos, data->number, data->conf.number_len, data);
    data->draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                loop = 0;
                data->loop = 0;
                break;

            case SDL_WINDOWEVENT:
                handle_resize(data, &event);
                data->draw_render(data);
                break;

            case SDL_MOUSEMOTION:
                data->mouse.x = event.button.x;
                data->mouse.y = event.button.y;
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    textedit_escape(&pos, data->number);
                    strtol(data->number, &non_int, 10);
                    if((strcmp(non_int, "") && strcmp(data->number, "a"))
                            || U8_strlen(data->number) == 0){
                        snprintf(data->number, data->conf.number_len, "%s",
                                last);
                    }
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    data->draw_render(data);
                    break;

                case SDLK_BACKSPACE:
                    textedit_backspace(&pos, data->number,
                            data->conf.number_len, data);
                    data->draw_render(data);
                    break;

                case SDLK_DELETE:
                    textedit_delete(&pos, data->number);
                    data->draw_render(data);
                    break;

                case SDLK_LEFT:
                    textedit_left(&pos, data->number,
                            data->conf.number_len, data);
                    data->draw_render(data);
                    break;

                case SDLK_RIGHT:
                    textedit_right(&pos, data->number,
                            data->conf.number_len, data);
                    data->draw_render(data);
                    break;
                }
                break;

            case SDL_TEXTINPUT:
                if(strchr("0123456789", event.text.text[0]) ||
                        (U8_strlen(data->number) == U8_strlen(data->conf.cursor)
                         && event.text.text[0] == 'a')){
                    textedit_input(&pos, data->number, data->conf.number_len,
                            data, event.text.text);
                    data->draw_render(data);
                }
                break;
            }
        }
    }

}

void
status_draw(WindowData *data)
{
    SDL_SetRenderDrawColor(data->renderer, STATUS_BACKGROUND);
    SDL_RenderFillRect(data->renderer, &data->layout.status);
    snprintf(data->status.str, data->conf.status_max_len, "%s %s[%s] %s",
            data->status.mode, data->filename, data->number,
            data->status.info);
    FC_DrawColor(data->font, data->renderer, data->layout.status.x
            + STATUS_PADDING_LEFT, data->layout.status.y,
            data->conf.status_font_color, data->status.str);
}
