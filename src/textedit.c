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
    U8_strinsert(str, *pos == U8_strlen(str) ? -1 : *pos, data->conf.cursor, len);
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
