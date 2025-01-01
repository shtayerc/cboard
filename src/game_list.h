#ifndef _GAME_LIST_H_
#define _GAME_LIST_H_

#include "chess_utils.h"
#include "event.h"
#include "message.h"
#include "window.h"

void game_list_draw(WindowData* data);
void mode_game_search(WindowData* data);
void mode_game_filter(WindowData* data);
void mode_game_filter_edit(WindowData* data, TagFilter* tag_filter);
void mode_game_sort_edit(WindowData* data);
void mode_game_list(WindowData* data);
void game_list_current_init(WindowData* data);
void game_list_current_next(WindowData* data);
void game_list_current_prev(WindowData* data);
int game_list_current_relative(WindowData* data);
void game_list_focus_current_game(WindowData* data);
void game_list_game_load(WindowData* data, int index);
void tag_list_title(TagList* tl, char * title);
char tfo2char(TagFilterOperator op);
TagFilterOperator char2tfo(char ch);
char* sorting2str(SortDirection sort);
SortDirection str2sorting(char* sort);

#endif
