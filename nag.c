#include "nag.h"

const char nag_map[][NAG_LEN] = {
    "",    "!", "?", "!!", "??", "!?", "?!", "□", "", "", "=", "=", "", "∞", "+/=", "=/+", "±", "∓", "+-", "-+", "", "",
    "⨀",   "⨀", "",  "",   "",   "",   "",   "",  "", "", "↻", "↻", "", "",  "↑",   "↑",   "",  "",  "→",  "→",  "", "",
    "=/∞", "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "⇆",   "⇆", "",  "",   "",   "",   "",   "",  "", "", "N", "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "", "",  "",    "",    "",  "",  "",   "",   "", "",
    "",    "",  "",  "",   "",   "",   "",   "",  "", "", "",  "",  "",
};

void
nag_move_next(Game* g) {
    int index = game_move_get(g)->nag_move;
    int i = index;
    while (!strcmp(nag_map[index], nag_map[i]) || strlen(nag_map[i]) == 0) {
        i = (i + 1) % 10;
    }
    game_move_get(g)->nag_move = i;
}

void
nag_move_prev(Game* g) {
    int index = game_move_get(g)->nag_move;
    int i = index;
    while (!strcmp(nag_map[index], nag_map[i]) || strlen(nag_map[i]) == 0) {
        i--;
        if (i < 1) {
            i = 9;
        }
    }
    game_move_get(g)->nag_move = i;
}

void
nag_position_next(Game* g) {
    int index = game_move_get(g)->nag_position;
    int i = index < 10 ? 10 : index;
    while (!strcmp(nag_map[index], nag_map[i]) || strlen(nag_map[i]) == 0) {
        i = (i + 1) % 255;
    }
    if (i < 10) {
        i = 10;
    }
    game_move_get(g)->nag_position = i;
}

void
nag_position_prev(Game* g) {
    int index = game_move_get(g)->nag_position;
    int i = index < 10 ? 254 : index;
    while (!strcmp(nag_map[index], nag_map[i]) || (strlen(nag_map[i]) == 0)) {
        i--;
        if (i < 10) {
            i = 254;
        }
    }
    game_move_get(g)->nag_position = i;
}
