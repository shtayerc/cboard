#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include "window.h"
#include "machine.h"
#include "notation.h"
#include "event.h"

enum { TextureBlackPawn, TextureBlackKnight, TextureBlackBishop,
    TextureBlackRook, TextureBlackQueen, TextureBlackKing, TextureWhitePawn,
    TextureWhiteKnight, TextureWhiteBishop, TextureWhiteRook,
    TextureWhiteQueen, TextureWhiteKing };

extern SDL_Texture *cb_piece_texture[12];
void piece_load(WindowData *data);
void piece_unload();
void piece_draw(WindowData *data, int file, int rank,
        SDL_Texture *texture);
void rotation_toggle(WindowData *data);
int rotation_convert(WindowData *data, int n); //rank or file
Square square_rotation(WindowData *data, Square sq);
void position_draw(WindowData *data);
void background_draw(WindowData *data);
void foreground_draw(WindowData *data);
void piece_mouse_position(WindowData *data);
int promotion_hover_position(WindowData *data, Square sq);
void promotion_selection_draw(WindowData *data, Square sq, Color color);
int promotion_selected_piece(WindowData *data, Square sq, Color color);
void promotion_draw(WindowData *data, Square sq, Color color);
int mode_promotion(WindowData *data, Color color);
void mode_editor(WindowData *data);
void mode_san(WindowData *data);
void mode_training(WindowData *data);
void chessboard_focus_present(WindowData *data, Square src, Square dst,
        Piece prom_piece);
void chessboard_vs_next(WindowData *data, int *vs_index);
void chessboard_move_do(WindowData *data, Square src, Square dst,
        Piece prom_piece, Status status);
Square chessboard_mouse_square(WindowData *data);
int chessboard_mouse_is_inside(WindowData *data);

#endif
