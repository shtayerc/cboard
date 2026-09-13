#ifndef TEXTEDIT_H_
#define TEXTEDIT_H_
#include "libs.h"
#include "window.h"

typedef struct WindowData WindowData;

void cursor_remove(int* pos, char* str);
void cursor_add(int* pos, char* str, int len, WindowData* data);
void text_cursor_set(TTF_Text* text, int pos);
void text_cursor_remove(TTF_Text* text);
int text_cursor_get(TTF_Text* text);
void textedit_escape(int* pos, char* str);
void textedit_delete(int* pos, char* str);
void textedit_backspace(int* pos, char* str, int len, WindowData* data);
void textedit_left(int* pos, char* str, int len, WindowData* data);
void textedit_right(int* pos, char* str, int len, WindowData* data);
void textedit_input(int* pos, char* str, int len, WindowData* data, const char* val);
void textedit_delete_all(int* pos, char* str, int len, WindowData* data);

#endif
