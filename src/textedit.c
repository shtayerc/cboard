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
    text_set(text, "cursor", pos);
}

void
text_set(TTF_Text* text, const char* key, int value) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    SDL_SetNumberProperty(props, key, value);
}

int
text_get(TTF_Text* text, const char* key) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    return SDL_GetNumberProperty(props, key, -1);
}

int
text_cursor_get(TTF_Text* text) {
    return text_get(text, "cursor");
}

void
text_cursor_remove(TTF_Text* text) {
    text_cursor_set(text, -1);
}

void
text_null(TTF_Text* text) {
    TTF_SetTextString(text, "", 0);
    text_cursor_remove(text);
}

void
ttf_text_pre(TTF_Text* text, const char* str) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    int start = SDL_GetNumberProperty(props, "start", 0);
    TTF_InsertTextString(text, start, str, 0);
    SDL_SetNumberProperty(props, "start", SDL_strlen(text->text));
}

void ttf_text_post(TTF_Text* text, const char* str) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    SDL_SetNumberProperty(props, "end", SDL_strlen(text->text));
    TTF_AppendTextString(text, str, 0);
}

void
ttf_text_set(TTF_Text* text, const char* str) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    int start = SDL_GetNumberProperty(props, "start", 0);
    int end = SDL_GetNumberProperty(props, "end", start);
    if (end != start) {
        TTF_DeleteTextString(text, start, end - start);
    }
    TTF_InsertTextString(text, start, str, 0);
    SDL_SetStringProperty(props, "value", str);
    SDL_SetNumberProperty(props, "end", start + SDL_strlen(str));
    if (!SDL_GetNumberProperty(props, "start", 0)) {
        SDL_SetNumberProperty(props, "cursor", start + SDL_strlen(str));
    }
}

const char*
ttf_text_get(TTF_Text* text) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    const char* str = SDL_GetStringProperty(props, "value", NULL);
    if (str == NULL) {
        int start = SDL_GetNumberProperty(props, "start", 0);
        int end = SDL_GetNumberProperty(props, "end", start);
        char* tmp = SDL_strndup(text->text + start, end - start);
        SDL_SetStringProperty(props, "value", tmp);
        SDL_free(tmp);
        str = SDL_GetStringProperty(props, "value", NULL);
    }
    return str;
}

void
ttf_text_null(TTF_Text* text) {
    TTF_SetTextString(text, "", 0);
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    SDL_SetNumberProperty(props, "cursor", -1);
    SDL_SetNumberProperty(props, "start", 0);
    SDL_SetNumberProperty(props, "end", 0);
    SDL_SetStringProperty(props, "value", NULL);
}

void
ttf_text_left(TTF_Text* text) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    int cursor = SDL_GetNumberProperty(props, "cursor", -1);
    int start = SDL_GetNumberProperty(props, "start", 0);
    int length = text_cursor_prev_length(text, cursor);
    if (cursor - length >= start) {
        SDL_SetNumberProperty(props, "cursor", cursor - length);
    }
}

void
ttf_text_right(TTF_Text* text) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    int cursor = SDL_GetNumberProperty(props, "cursor", -1);
    int end = SDL_GetNumberProperty(props, "end", SDL_strlen(text->text));
    int length = text_cursor_next_length(text, cursor);
    if (cursor + length <= end) {
        SDL_SetNumberProperty(props, "cursor", cursor + length);
    }
}

void
ttf_text_delete(TTF_Text* text) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    int cursor = SDL_GetNumberProperty(props, "cursor", -1);
    int end = SDL_GetNumberProperty(props, "end", SDL_strlen(text->text));
    int length = text_cursor_next_length(text, cursor);
    if (cursor + length <= end) {
        TTF_DeleteTextString(text, cursor, length);
        SDL_SetNumberProperty(props, "end", end - length);
        SDL_SetStringProperty(props, "value", NULL);
    }
}

void
ttf_text_backspace(TTF_Text* text) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    int cursor = SDL_GetNumberProperty(props, "cursor", -1);
    int start = SDL_GetNumberProperty(props, "start", 0);
    int end = SDL_GetNumberProperty(props, "end", 0);
    int length = text_cursor_prev_length(text, cursor);
    if (cursor - length >= start) {
        TTF_DeleteTextString(text, cursor - length, length);
        SDL_SetNumberProperty(props, "cursor", cursor - length);
        SDL_SetNumberProperty(props, "end", end - length);
        SDL_SetStringProperty(props, "value", NULL);
    }
}

void
ttf_text_insert(TTF_Text* text, const char* str) {
    int length = SDL_strlen(str);
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    int cursor = SDL_GetNumberProperty(props, "cursor", -1);
    int end = SDL_GetNumberProperty(props, "end", 0);
    TTF_InsertTextString(text, cursor, str, 0);
    SDL_SetNumberProperty(props, "cursor", cursor + length);
    SDL_SetNumberProperty(props, "end", end + length);
    SDL_SetStringProperty(props, "value", NULL);
}

void
ttf_text_cursor(TTF_Text* text, int pos) {
    SDL_PropertiesID props = TTF_GetTextProperties(text);
    if (pos == -1) {
        SDL_SetNumberProperty(props, "manual_cursor", 1);
    }
    if (pos == -2) {
        pos = SDL_GetNumberProperty(props, "end", SDL_strlen(text->text));
    }
    SDL_SetNumberProperty(props, "cursor", pos);
}

void
ttf_box_pre(TTF_Text* text, SDL_Rect* bounds, SDL_Rect* pos) {
    int h;
    TTF_SetTextWrapWidth(text, bounds->w); //width has to be set before calculating
    TTF_GetTextSize(text, NULL, &h);
    if (h <= TTF_GetFontLineSkip(TTF_GetTextFont(text))) {
        return;
    }
    int w = pos->x - bounds->x;
    pos->x = bounds->x;
    int space;
    TTF_GetGlyphMetrics(TTF_GetTextFont(text), ' ', NULL, NULL, NULL, NULL, &space);
    int n = w / space;
    char tmp[n + 1];
    memset(tmp, ' ', n);
    tmp[n] = '\0';
    ttf_text_pre(text, tmp);
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
