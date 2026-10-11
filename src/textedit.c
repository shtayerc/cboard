#include "textedit.h"

void
cursor_remove(int* pos, char* str) {
    if (*pos == -1) {
        return;
    }
    U8_strdel(str, *pos);
}

void
cursor_add(int* pos, char* str, int len, WindowData* data) {
    SDL_StartTextInput(data->window);
    U8_strinsert(str, *pos == U8_strlen(str) ? -1 : *pos, data->conf.cursor, len);
}

void
text_cursor_set(TTF_Text* text, int pos) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    SDL_SetNumberProperty(props, "cursor", pos);
}

int
text_cursor_get(TTF_Text* text) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    return SDL_GetNumberProperty(props, "cursor", -1);
}

void
text_cursor_remove(TTF_Text* text) {
    text_cursor_set(text, -1);
}

int
text_cursor_prev_length(TTF_Text* text, int pos) {
    const char* start = text->text + pos;
    const char* next = start;
    SDL_StepBackUTF8(text->text, &next);
    return (int)(uintptr_t)(start - next);
}

int
text_cursor_next_length(TTF_Text* text, int pos) {
    const char* start = text->text + pos;
    const char* next = start;
    size_t length = SDL_strlen(next);
    SDL_StepUTF8(&next, &length);
    return (next - start);
}

void
textedit_escape(int* pos, char* str) {
    cursor_remove(pos, str);
}

void
textedit_delete(int* pos, char* str) {
    if (*pos < U8_strlen(str) - 1) {
        U8_strdel(str, *pos + 1);
    }
}

void
textedit_backspace(int* pos, char* str, int len, WindowData* data) {
    if (*pos > 0) {
        cursor_remove(pos, str);
        (*pos)--;
        U8_strdel(str, *pos);
        cursor_add(pos, str, len, data);
    }
}

void
textedit_left(int* pos, char* str, int len, WindowData* data) {
    if (*pos > 0) {
        cursor_remove(pos, str);
        (*pos)--;
        cursor_add(pos, str, len, data);
    }
}

void
textedit_right(int* pos, char* str, int len, WindowData* data) {
    if (*pos < U8_strlen(str) - 1) {
        cursor_remove(pos, str);
        (*pos)++;
        cursor_add(pos, str, len, data);
    }
}

void
textedit_input(int* pos, char* str, int len, WindowData* data, const char* val) {
    cursor_remove(pos, str);
    U8_strinsert(str, (U8_strlen(str) == *pos ? -1 : *pos), val, len);
    (*pos)++;
    cursor_add(pos, str, len, data);
}

void
textedit_delete_all(int* pos, char* str, int len, WindowData* data) {
    str[0] = '\0';
    *pos = 0;
    cursor_add(pos, str, len, data);
}
