#ifndef TEXTEDIT_H_
#define TEXTEDIT_H_
#include "libs.h"
#include "window.h"

typedef struct WindowData WindowData;

void cursor_remove(int* pos, char* str);
void cursor_add(int* pos, char* str, int len, WindowData* data);
void text_cursor_set(TTF_Text* text, int pos);
void text_cursor_remove(TTF_Text* text);
void text_set(TTF_Text* text, const char* key, int value);
int text_get(TTF_Text* text, const char* key);
int text_cursor_prev_length(TTF_Text* text, int pos);
int text_cursor_next_length(TTF_Text* text, int pos);
int text_cursor_get(TTF_Text* text);
void text_null(TTF_Text* text);

void ttf_text_pre(TTF_Text* text, const char* str);
void ttf_text_post(TTF_Text* text, const char* str);
void ttf_text_set(TTF_Text* text, const char* str);
const char* ttf_text_get(TTF_Text* text);
void ttf_text_null(TTF_Text* text);
void ttf_text_left(TTF_Text* text);
void ttf_text_right(TTF_Text* text);
void ttf_text_delete(TTF_Text* text);
void ttf_text_backspace(TTF_Text* text);
void ttf_text_insert(TTF_Text* text, const char* str);
void ttf_text_cursor(TTF_Text* text, int pos);
void ttf_box_pre(TTF_Text* text, SDL_Rect* bounds, SDL_Rect* pos);

void textedit_escape(int* pos, char* str);
void textedit_delete(int* pos, char* str);
void textedit_backspace(int* pos, char* str, int len, WindowData* data);
void textedit_left(int* pos, char* str, int len, WindowData* data);
void textedit_right(int* pos, char* str, int len, WindowData* data);
void textedit_input(int* pos, char* str, int len, WindowData* data, const char* val);
void textedit_delete_all(int* pos, char* str, int len, WindowData* data);

#endif
