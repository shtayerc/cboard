#ifndef _GAME_LIST_H_
#define _GAME_LIST_H_

#include "chess_utils.h"
#include "window.h"
#include "event.h"
#include "message.h"

void game_list_draw(WindowData *data);
void mode_game_search(WindowData *data);
void mode_game_list(WindowData *data);
void game_list_reverse(GameList *gl);
void game_list_focus_current_game(WindowData *data);

#endif
