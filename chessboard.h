#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include "window_data.h"
#include "machine.h"
#include "notation.h"
#include "event.h"

enum { TextureBlackPawn, TextureBlackKnight, TextureBlackBishop,
    TextureBlackRook, TextureBlackQueen, TextureBlackKing, TextureWhitePawn,
    TextureWhiteKnight, TextureWhiteBishop, TextureWhiteRook,
    TextureWhiteQueen, TextureWhiteKing };

extern Square cb_hidden;
extern int cb_drag;

SDL_Texture *cb_piece_texture[12];
void piece_load(WindowData *data);
void piece_unload();
void piece_draw(WindowData *data, int file, int rank,
        SDL_Texture *texture);
void rotation_toggle(WindowData *data);
int rotation_convert(WindowData *data, int n); //rank or file
void position_draw(WindowData *data);
void background_draw(WindowData *data);
void foreground_draw(WindowData *data);
void piece_mouse_position(WindowData *data, Piece piece);
int promotion_hover_position(WindowData *data, Square sq);
void promotion_selection_draw(WindowData *data, Square sq, Color color);
int promotion_selected_piece(WindowData *data, Square sq, Color color);
void promotion_draw(WindowData *data, Square sq, Color color);
int mode_promotion(WindowData *data, Color color);
void editor_draw(WindowData *data, Piece piece);
void mode_editor(WindowData *data);
void drag_draw(WindowData *data, int piece);
void mode_san(WindowData *data);
void mode_training(WindowData *data);
void chessboard_focus_present(WindowData *data, Square src, Square dst,
        Piece prom_piece);
void chessboard_focus_random(WindowData *data);
void chessboard_move_do(WindowData *data, Square src, Square dst,
        Piece prom_piece, Status status);

#endif
