#ifndef _NAG_H_
#define _NAG_H_
#include "libs.h"
#define NAG_LEN 10
#define NAG_MAX 256

extern const char nag_map[][NAG_LEN];

typedef enum {
    NagNone = 0,
    NagMoveGood = 1, NagMoveBad = 2, NagMoveVeryGood = 3, NagMoveVeryBad = 4, NagMoveSpeculative = 5, NagMoveQuestionable = 6, NagMoveOnly = 7,
    NagPosDrawish = 10, NagPosUnclear = 13, NagPosWhiteLowAdv = 14, NagPosBlackLowAdv = 15, NagPosWhiteMidAdv = 16, NagPosBlackMidAdv = 17,
    NagPosWhiteHighAdv = 18, NagPosBlackHighAdv = 19
} NagIndex;

void nag_move_next(Game* g);
void nag_move_prev(Game* g);
void nag_position_next(Game* g);
void nag_position_prev(Game* g);
#endif
