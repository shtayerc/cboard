#ifndef _NAG_H_
#define _NAG_H_
#include "chess_utils.h"
#define NAG_LEN 10
#define NAG_MAX 256

extern const char nag_map[][NAG_LEN];

void nag_move_next(Notation *n);
void nag_move_prev(Notation *n);
void nag_position_next(Notation *n);
void nag_position_prev(Notation *n);
#endif
