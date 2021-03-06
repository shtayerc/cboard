#include "nag.h"

const char nag_map[][NAG_LEN] = {
    "",
    "!",
    "?",
    "!!",
    "??",
    "!?",
    "?!",
    "□",
    "",
    "",
    "=",
    "=",
    "",
    "∞",
    "+/=",
    "=/+",
    "±",
    "∓",
    "+-",
    "-+",
    "",
    "",
    "⨀",
    "⨀",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "↻",
    "↻",
    "",
    "",
    "↑",
    "↑",
    "",
    "",
    "→",
    "→",
    "",
    "",
    "=/∞",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "⇆",
    "⇆",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "N",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
};

void
nag_move_next(Notation *n){
    int index = notation_move_get(n)->nag_move;
    int i=index;
    while(!strcmp(nag_map[index], nag_map[i]) || strlen(nag_map[i]) == 0){
        i = (i+1)%10;
    }
    notation_move_get(n)->nag_move = i;
}

void
nag_move_prev(Notation *n){
    int index = notation_move_get(n)->nag_move;
    int i = index;
    while(!strcmp(nag_map[index], nag_map[i]) || strlen(nag_map[i]) == 0){
        i--;
        if(i<1)
            i=9;
    }
    notation_move_get(n)->nag_move = i;
}

void
nag_position_next(Notation *n)
{
    int index = notation_move_get(n)->nag_position;
    int i = index < 10 ? 10: index;
    while(!strcmp(nag_map[index], nag_map[i]) || strlen(nag_map[i]) == 0){
        i = (i+1) % 255;
    }
    if(i<10)
        i=10;
    notation_move_get(n)->nag_position = i;
}

void
nag_position_prev(Notation *n)
{
    int index = notation_move_get(n)->nag_position;
    int i = index < 10 ? 254: index;
    while(!strcmp(nag_map[index], nag_map[i]) || (strlen(nag_map[i]) == 0)){
        i--;
        if(i<10)
            i = 254;
    }
    notation_move_get(n)->nag_position = i;
}
