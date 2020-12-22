#ifndef _GAME_LIST_H_
#define _GAME_LIST_H_

#include "window_data.h"
#include "chess_utils.h"
#include "event.h"

void game_list_draw(WindowData *data);
void mode_game_search(WindowData *data);
void mode_game_list(WindowData *data);
void game_list_reverse(GameList *gl);
#endif
