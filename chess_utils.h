/*
chess_utils v0.7.11

Copyright (c) 2024 David Murko

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN Ae ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _CHESS_UTILS_H_
#define _CHESS_UTILS_H_

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TAG_LEN           512
#define SAN_LEN           10
#define FEN_LEN           512
#define BUFFER_LEN        1024
#define WORD_LEN          256
#define COMMENT_LEN       4096
#define MOVENUM_LEN       10
#define PGN_LINE_LEN      80
#define SQUARE_LEN        3
#define GAMETITLE_LEN     4096
//max difference of move number between transpositions
#define MOVENUM_OVERCHECK 6

//
//ENUMS
//

typedef enum {
    a8 = 0,
    b8 = 1,
    c8 = 2,
    d8 = 3,
    e8 = 4,
    f8 = 5,
    g8 = 6,
    h8 = 7,
    a7 = 16,
    b7 = 17,
    c7 = 18,
    d7 = 19,
    e7 = 20,
    f7 = 21,
    g7 = 22,
    h7 = 23,
    a6 = 32,
    b6 = 33,
    c6 = 34,
    d6 = 35,
    e6 = 36,
    f6 = 37,
    g6 = 38,
    h6 = 39,
    a5 = 48,
    b5 = 49,
    c5 = 50,
    d5 = 51,
    e5 = 52,
    f5 = 53,
    g5 = 54,
    h5 = 55,
    a4 = 64,
    b4 = 65,
    c4 = 66,
    d4 = 67,
    e4 = 68,
    f4 = 69,
    g4 = 70,
    h4 = 71,
    a3 = 80,
    b3 = 81,
    c3 = 82,
    d3 = 83,
    e3 = 84,
    f3 = 85,
    g3 = 86,
    h3 = 87,
    a2 = 96,
    b2 = 97,
    c2 = 98,
    d2 = 99,
    e2 = 100,
    f2 = 101,
    g2 = 102,
    h2 = 103,
    a1 = 112,
    b1 = 113,
    c1 = 114,
    d1 = 115,
    e1 = 116,
    f1 = 117,
    g1 = 118,
    h1 = 119,
    none = -1
} Square;

typedef enum { Rank_8, Rank_7, Rank_6, Rank_5, Rank_4, Rank_3, Rank_2, Rank_1, Rank_none = -1 } Rank;

typedef enum { File_a, File_b, File_c, File_d, File_e, File_f, File_g, File_h, File_none = -1 } File;

typedef enum { Black, White, NoColor = -1 } Color;

typedef enum {
    OffsetBlackPawn,
    OffsetWhitePawn,
    OffsetKnight,
    OffsetBishop,
    OffsetRook,
    OffsetQueen,
    OffsetKing,
    OffsetNone = -1
} OffsetIndex;

typedef enum {
    Empty,
    BlackPawn,
    BlackKnight,
    BlackBishop,
    BlackRook,
    BlackQueen,
    BlackKing,
    WhitePawn,
    WhiteKnight,
    WhiteBishop,
    WhiteRook,
    WhiteQueen,
    WhiteKing
} Piece;

typedef enum { QueenSide, KingSide, NoSide } Side;

typedef enum { Invalid, Valid, Castling, EnPassant, Promotion } Status;

typedef enum { Centipawn, Mate, NoType = -1 } UciScoreType;

//
//STRUCTS
//

typedef struct {
    char key[TAG_LEN];
    char value[TAG_LEN];
} Tag;

typedef struct {
    Piece position[128];
    int half_move;
    int move_number;
    Color turn;
    int castling[2][2];
    Square en_passant;
    Square kings[2];
} Board;

typedef struct Move Move;
typedef struct Variation Variation;

struct Variation {
    Move* move_list;
    int move_current;
    int move_count;
    Variation* prev;
};

struct Move {
    Square src;
    Square dst;
    Piece prom_piece;
    Board board;
    char san[SAN_LEN];
    char* comment;
    int nag_move;
    int nag_position;
    Variation** variation_list;
    int variation_count;
};

typedef struct {
    Variation* line_main;
    Variation* line_current;
    Tag* tag_list;
    int tag_count;
} Game;

typedef struct {
    char title[GAMETITLE_LEN];
#ifdef ADDITIONAL_TAG
    char tag_value[TAG_LEN];
#endif
    int index;
} GameRow;

typedef struct {
    GameRow* list;
    int count;
} GameList;

typedef struct {
    int index;
    int count;
    int searched;
} VariationBranch;

typedef struct {
    VariationBranch* list;
    int count;
} VariationSequence;

//
//GLOBAL VARIABLES
//

extern const char* FEN_DEFAULT;
extern const char* PIECE_STR;
extern const char* CASTLE_STR_SHORT;
extern const char* CASTLE_STR_LONG;
extern const char* piece_map[];
extern const OffsetIndex offset_map[];
extern const int piece_offset[][9];

//
//STRING UTILS
//

//standard strtok_r replacement
char* strtok_r(char* str, const char* delim, char** nextp);

//standard strdup replacement
char* strdup(const char* str);

//case insensitive check if substring exists
int isubstr(const char* haystack, const char* needle);

//append to the end of string - usage is similar to printf functions
void concate(char* str, int len, const char* fmt, ...);

//remove "+x#=()" characters from string and characters before first dot
//(including first dot)
void trimmove(char* str);

//remove "\r\n" characters from string
void trimendl(char* str);

//remove till and with first '{' and remove after and with first '}'
void trimcomment(char* str);

//remove character from string at given index
void charremove(char* str, int i);

//returns number of characters in string
int charcount(const char* str, char target);

//returns number of characters before given character in string
int charcount_before(const char* str, char target, char end);

//returns number of characters after given character in string
int charcount_after(const char* str, char target, char start);

//returns 1 if string contains only characters that are allowed in SAN
int str_is_move(const char* str);

//returns 1 if string is square [a-h][1-8]
int str_is_square(const char* str);

//returns 1 if string is valid FEN (does not validate checks)
int str_is_fen(const char* str);

//returns 1 if string is valid tag, given Tag struct is set
int tag_extract(const char* str, Tag* tag);

//Escape double quotes in str with backslash. If length of escaped string is
//too short, then escaped is set to empty string.
void tag_escape_value(const char* str, char* escaped, int len);

//
//BOARD UTILS
//

//returns Square for given File and Rank
Square filerank2square(File file, Rank rank);

//returns File/Rank for given Square
File square2file(Square sq);
Rank square2rank(Square sq);

//returns File/Rank/Piece for given character
File char2file(char ch);
Rank char2rank(char ch);
Piece char2piece(char ch); //'P' -> WhitePawn

//returns character for given File/Rank/Piece
char file2char(File file);
char rank2char(Rank rank);
char piece2char(Piece piece); //WhitePawn -> 'P'

//returns offset for given move and for given piece from piece_offset array
int move_offset_find(Square src, Square dst, OffsetIndex offset_index);

//UCI string is set for given move
void move_uci_export(Square src, Square dst, Piece prom_piece, char* uci,
                     int len); //e2,e4,Empty -> "e2e4"

//
//BOARD FUNCTIONS
//

//b->position array is set to Empty
void board_clear(Board* b);

//pretty print Board to standard output
void board_print(Board* b);

//returns Color for given Square
Color board_square_piece_color(Board* b, Square sq);

//b->position[sq] is set to given Piece
void board_square_set(Board* b, Square sq, Piece piece);

//returns 1 if Square is attacked by piece of given Color
int board_square_is_attacked(Board* b, Square sq, Color color);

//returns 1 if player on turn is in check
int board_is_check(Board* b);

//returns 1 if player on turn is in checkmate
int board_is_checkmate(Board* b);

//returns 1 if player on turn is in stalemate
int board_is_stalemate(Board* b);

//returns 1 if player on turn is out of legal moves
int board_is_out_of_moves(Board* b);

//returns 1 if b1 and b2 are equal, strict decides if move number and half move
//are compared or not
int board_is_equal(Board* b1, Board* b2, int strict);

//returns 1 if dst is different color and there are no pieces blocking path
int board_move_pattern_knight_is_valid(Board* b, Square src, Square dst, Color color);
int board_move_pattern_bishop_is_valid(Board* b, Square src, Square dst, Color color);
int board_move_pattern_rook_is_valid(Board* b, Square src, Square dst, Color color);

//returns 1 if pattern is valid and on src is correct Piece
int board_move_knight_is_valid(Board* b, Square src, Square dst, Color color);
int board_move_bishop_is_valid(Board* b, Square src, Square dst, Color color);
int board_move_rook_is_valid(Board* b, Square src, Square dst, Color color);
int board_move_queen_is_valid(Board* b, Square src, Square dst, Color color);

//returns Valid/EnPassant/Promotion if pattern is valid and on src is correct
//Piece else returns Invalid
Status board_move_pawn_status(Board* b, Square src, Square dst, Color color);

//returns Valid/Castling if pattern is valid and on src is correct Piece else
//returns Invalid
Status board_move_king_status(Board* b, Square src, Square dst, Color color);

//returns 1 if src is WhitePawn/BlackPawn and dst is Rank_8/Rank_1
int board_move_is_promotion(Board* b, Square src, Square dst);

//returns 1 if src and dst are not Empty
int board_move_is_capture(Board* b, Square src, Square dst);

//returns 1 if after move king of given Color is in check
int board_move_is_check(Board* b, Square src, Square dst, Piece prom_piece, Status status, Color color);

//returns 1 if after move there is checkmate
int board_move_is_checkmate(Board* b, Square src, Square dst, Piece prom_piece, Status status);

//creates string to choose between more same pieces that can go to same square
void board_move_which(Board* b, Square src, Square dst, char* which);

//returns Status of given move
Status board_move_status(Board* b, Square src, Square dst, Piece prom_piece);

//returns Status of given SAN move, also set parameters src, dst, prom_piece
Status board_move_san_status(Board* b, const char* san, Square* src, Square* dst, Piece* prom_piece);

//returns Status of given UCI move, also set parameters src, dst, prom_piece
Status board_move_uci_status(Board* b, const char* uci, Square* src, Square* dst, Piece* prom_piece);

//do given move on board
void board_move_do(Board* b, Square src, Square dst, Piece prom_piece, Status status);

//SAN string is set for given move
void board_move_san_export(Board* b, Square src, Square dst, Piece prom_piece, char* san, int len,
                           Status status); //g1,f3,Empty -> "Nf3"

//set board to given FEN string
void board_fen_import(Board* b, const char* FEN_str);

//FEN string is set for given board
void board_fen_export(Board* b, char* FEN_str);

//
//MOVE FUNCTIONS
//

void move_init(Move* m);

//free related Variations recursively and free comment
void move_free(Move* m);

//copies Move properties and clone Variations from variation_list
void move_copy(Move* src, Move* dst, Variation* prev);

//returns index of given variation in variation_list if not found returns -1
int move_variation_find(Move* m, Variation* v);

//remove given variation from variation_list, returns 1 if it is found and
//removed else returns 0
int move_variation_delete(Move* m, Variation* v);

//
//VARIATION FUNCTIONS
//

//initialize variation with 1 Move with given Board
//if b is NULL then FEN_DEFAULT Board is used
void variation_init(Variation* v, Board* b);

//free v->move_list and all related Variations recursively
void variation_free(Variation* v);

//delete all moves after current move
void variation_delete_next_moves(Variation* v);

//new Move is added to the end of v->move_list
void variation_move_add(Variation* v, Square src, Square dst, Piece prom_piece, Board* b, const char* san);

//returns 1 if v->move_current is increased for 1
int variation_move_next(Variation* v);

//returns 1 if v->move_current is decreased for 1
int variation_move_prev(Variation* v);

//returns index of move in prev which is parent of v if not found returns -1
int variation_index_find(Variation* v, Variation* prev);

//returns current Move in given variation
Move* variation_move_get(Variation* v);

//move number is set for white moves and first move of variation
//index is for move_list, prev_index is index from parent variation
void variation_movenumber_export(Variation* v, int index, int prev_index, char* str, int len);

//returns pointer to equivalent Variation of orig in cloned
Variation* variation_equivalent_find(Variation* orig, Variation* clone, Variation* v);

//returns pointer to deep copied Variation
Variation* variation_clone(Variation* v, Variation* prev);

//returns move index of given board found in given variation or sub variations.
//found variation from which index was returned
int variation_board_find(Variation* v, Board* b, Variation** found);

//
// VARIATION SEQUENCE FUNCTIONS
//

//initialize given VariationSequence
void vs_init(VariationSequence* vs);

//free give VariationSequence and each VariationBrach
void vs_free(VariationSequence* vs);

//add new VariationBranch at the end of VariationSequence list
void vs_add(VariationSequence* vs, int index, int count, int searched);

//remove last VariationBranch and resize VariationSequence
void vs_remove_last(VariationSequence* vs);

//returns 1 is both given VariationSequences are equal
int vs_is_equal(VariationSequence* vs1, VariationSequence* vs2);

//fill given VariationSequence with main line of given Variation. Variations of
//given Color are ignored - to not ignore anything pass NoColor.
void vs_generate_first(VariationSequence* vs, Variation* v, Color color);

//returns 1 if next VariationSequence can be generated
int vs_can_generate_next(VariationSequence* vs);

//fill given VariationSequence with next branch from given Variation. Last
//VariationBranch from prev VariationSequence is increased and its sub
//variations are added (if there are any). This function is meant to be used
//after vs_generate_first and before that one should check if it can be
//generated with function vs_can_generate_next. Color parameters has the same
//meaning as in vs_generate_first.
void vs_generate_next(VariationSequence* vs, Variation* v, VariationSequence* prev, Color color);

//print VariatonSequence list of VariationBranches to standard output
void vs_print(VariationSequence* vs);

//
//GAME FUNCTIONS
//

//initialize tag_list and create 7 required tags (Event, Site, Date, Round,
//White, Black, Result)
void game_tag_init(Game* g);

//free tag_list if not empty
void game_tag_free(Game* g);

//return index of g->tag_list for given key
int game_tag_index(Game* g, const char* key);

//given Tag is appended to g->tag_list
void game_tag_add(Game* g, Tag* tag);

//remove Tag with given key
void game_tag_remove(Game* g, const char* key);

//returns Tag for given key
Tag* game_tag_get(Game* g, const char* key);

//value of Tag with key is replaced - if key does not exists new Tag is created
void game_tag_set(Game* g, const char* key, const char* value);

//initialize game with 1 Variation with given Board and initialize 7 tags
//if b is NULL then FEN_DEFAULT Board is used
void game_init(Game* g, Board* b);

//free given Game and related Variations recursively
void game_free(Game* g);

//returns pointer to deep copied Game
Game* game_clone(Game* g);

//returns 1 if current move in current line is last
int game_move_is_last(Game* g);

//returns 1 if given move already exists after current or in variation_list
int game_move_is_present(Game* g, Square src, Square dst, Piece prom_piece);

//returns 1 if current line is main line
int game_line_is_main(Game* g);

//returns current Move in current line
Move* game_move_get(Game* g);

//returns index of current move in current line
int game_move_index_get(Game* g);

//index of current move is set in current line - returns 1 if index is changed
int game_move_index_set(Game* g, int index);

//returns Status of given move at current move in Game
Status game_move_status(Game* g, Square src, Square dst, Piece prom_piece);

//returns Status of given san move at current move in Game
Status game_move_san_status(Game* g, const char* san, Square* src, Square* dst, Piece* prom_piece);

//given move is added after current move
void game_move_add(Game* g, Square src, Square dst, Piece prom_piece, Status status);

//Variation with given move is created at current move
void game_variation_add(Game* g, Square src, Square dst, Piece prom_piece, Status status);

//if current line is not main it is deleted
void game_variation_delete(Game* g);

//if current line is not main it changes positions with main
void game_variation_promote(Game* g);

//add moves from parent variation to current line until they are valid
void game_variation_insert(Game* g);

//find move with given board, first the main variation is checked
//if not found function alse searches subvariations
void game_board_find(Game* g, Board* b);

//
//PGN FUNCTIONS
//

//move file position to next game, set tags to 1 if position is before tags
void pgn_read_next(FILE* f, int tags);

//returns 1 if PGN in given FILE at given index is valid, also Game is set
int pgn_read_file(FILE* f, Game* g, int index);

//handle PGN_LINE_LEN - output line to file before it is too long
void pgn_write_concate(FILE* f, char* line, int len, const char* fmt, ...);

//output given string as PGN incline comment
void pgn_write_comment(FILE* f, char* line, const char* str);

//output Variation - i is previous Variation move index
//if i == -1 no parentheses will be written
void pgn_write_variation(FILE* f, Variation* v, char* line, int i);

//output PGN for given Game
void pgn_write_file(FILE* f, Game* g);

//replace game at given index in file with given filename
int pgn_replace_game(const char* filename, Game* g, int index);

//returns number of games in given FILE, if PGN is incorrect returns -1
int pgn_count_games(FILE* f);

//
//UCI FUNCTIONS
//

//UCI moves from engine output are added to / replaced in given variation
//parameters b, v can be NULL. v->move_count can be higher than actual move
//count - extra moves have .san set to emtpy string
void uci_line_parse(const char* str, int len, Board* b, int* depth, int* multipv, UciScoreType* type, int* score,
                    Variation* v);

//
//GAME LIST FUNCTIONS
//

//initialize empty GameList
void game_list_init(GameList* gl);

//free given GameList if not empty
void game_list_free(GameList* gl);

//append GameRow to GameList
void game_list_add(GameList* gl, GameRow* gr);

//read GameRows from pgn file
void game_list_read_pgn(GameList* gl, FILE* f);

//fill GameList new_gl with GameRows containing case insensitive str
void game_list_search_str(GameList* gl, GameList* new_gl, const char* str);

//fill GameList new_gl with GameRows containing given Board
void game_list_search_board(GameList* gl, GameList* new_gl, FILE* f, Board* b);

//compare function for sorting GameList
int game_list_cmp(const void* a, const void* b);

#ifdef __cplusplus
}
#endif

#endif // _CHESS_UTILS_H_

#ifdef CHESS_UTILS_IMPLEMENTATION

const char* FEN_DEFAULT = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const char* PIECE_STR = ".pnbrqkPNBRQK";
const char* CASTLE_STR_SHORT = "O-O";
const char* CASTLE_STR_LONG = "O-O-O";
const char* piece_map[] = {"", "", "N", "B", "R", "Q", "K", "", "N", "B", "R", "Q", "K"};

const OffsetIndex offset_map[] = {OffsetNone,  OffsetBlackPawn, OffsetKnight,    OffsetBishop, OffsetRook,
                                  OffsetQueen, OffsetKing,      OffsetWhitePawn, OffsetKnight, OffsetBishop,
                                  OffsetRook,  OffsetQueen,     OffsetKing};

const int piece_offset[][9] = {{16, 32, 17, 15, 0, 0, 0, 0, 0},         {-16, -32, -17, -15, 0, 0, 0, 0, 0},
                               {-18, -33, -31, -14, 18, 33, 31, 14, 0}, {-17, -15, 17, 15, 0, 0, 0, 0, 0},
                               {-16, 1, 16, -1, 0, 0, 0, 0, 0},         {-17, -16, -15, 1, 17, 16, 15, -1, 0},
                               {-17, -16, -15, 1, 17, 16, 15, -1, 0}}; //offset

char*
strtok_r(char* str, const char* delim, char** nextp) {
    char* ret;
    if (str == NULL) {
        str = *nextp;
    }

    if (*str == '\0') {
        return NULL;
    }

    str += strspn(str, delim);
    if (*str == '\0') {
        return NULL;
    }

    ret = str;
    str += strcspn(str, delim);

    if (*str) {
        *str++ = '\0';
    }

    *nextp = str;
    return ret;
}

char*
strdup(const char* str) {
    int size = strlen(str) + 1;
    char* dup = (char*)malloc(sizeof(char) * size);
    if (dup) {
        memcpy(dup, str, size);
    }
    return dup;
}

int
isubstr(const char* haystack, const char* needle) {
    unsigned int i, j;
    j = 0;
    for (i = 0; i < strlen(haystack);) {
        if (tolower(haystack[i]) == tolower(needle[j])) {
            if (j == strlen(needle) - 1) {
                return 1;
            }
            i++;
            j++;
        } else {
            j = 0;
            //find next first character of needle string
            while (++i < strlen(haystack) && tolower(haystack[i]) != tolower(needle[j]))
                ;
        }
    }
    return 0;
}

void
concate(char* str, int len, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(str + strlen(str), len, fmt, args);
    va_end(args);
}

void
trimmove(char* str) {
    int i = 0;
    if (charcount(str, '.') == 1) {
        while (str[i] != '.') {
            charremove(str, i);
        }
        charremove(str, i);
        i = 0;
    }
    while (str[i] != '\0') {
        if (strchr("+x#=()", str[i]) != NULL) {
            charremove(str, i);
        } else {
            i++;
        }
    }
}

void
trimendl(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        if (strchr("\r\n", str[i]) != NULL) {
            charremove(str, i);
        } else {
            i++;
        }
    }
}

void
trimcomment(char* str) {
    int j, i;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == '{') {
            i++;
            for (j = 0; str[i + j] != '}'; j++) {
                str[j] = str[i + j];
            }
            str[j] = '\0';
            return;
        }
    }
}

void
charremove(char* str, int i) {
    int j;
    for (j = 1; str[i + j - 1] != '\0'; j++) {
        str[i + (j - 1)] = str[i + j];
    }
}

int
charcount(const char* str, char target) {
    unsigned int i;
    int count = 0;
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == target) {
            count++;
        }
    }
    return count;
}

int
charcount_before(const char* str, char target, char end) {
    int i;
    int count = 0;
    for (i = 0; str[i] != end; i++) {
        if (str[i] == target) {
            count++;
        }
    }
    return count;
}

int
charcount_after(const char* str, char target, char start) {
    unsigned int i;
    int count = 0;
    for (i = 0; str[i] != start; i++)
        ;

    for (; i < strlen(str); i++) {
        if (str[i] == target) {
            count++;
        }
    }
    return count;
}

int
str_is_move(const char* str) {
    unsigned int i;
    if (strlen(str) < 2) {
        return 0;
    }
    const char* allowed = "12345678abcdefghxBNKQRO-+#=";
    if (!strcmp(str, "--")) {
        return 0;
    }
    for (i = 0; i < strlen(str); i++) {
        if (strchr(allowed, str[i]) == NULL) {
            return 0;
        }
    }
    return 1;
}

int
str_is_square(const char* str) {
    if (strlen(str) < 2) {
        return 0;
    }
    if (strchr("abcdefgh", str[0]) == NULL) {
        return 0;
    }

    if (strchr("12345678", str[1]) == NULL) {
        return 0;
    }
    return 1;
}

int
str_is_fen(const char* str) {
    if (str == NULL) {
        return 0;
    }

    const char* valid_castling[] = {"KQkq", "KQk", "KQq", "KQ", "Kkq", "Kk", "Kq", "K", "Qkq",
                                    "Qk",   "Qq",  "Q",   "kq", "k",   "q",  "-",  NULL};
    unsigned int i;
    int pieces, rank;
    char *board, *turn, *castling, *en_passant, *saveptr, *row;
    char fen[FEN_LEN];
    snprintf(fen, FEN_LEN, "%s", str);

    board = strtok_r(fen, " ", &saveptr);
    if (board == NULL) {
        return 0;
    }
    turn = strtok_r(NULL, " ", &saveptr);
    if (turn == NULL || (strcmp(turn, "w") && strcmp(turn, "b"))) {
        return 0;
    }
    castling = strtok_r(NULL, " ", &saveptr);
    if (castling == NULL) {
        castling = (char*)"-";
        en_passant = (char*)"-";
    } else {
        for (i = 0; valid_castling[i] != NULL; i++) {
            if (!strcmp(castling, valid_castling[i])) {
                break;
            }
        }
        if (valid_castling[i] == NULL) {
            return 0;
        }

        en_passant = strtok_r(NULL, " ", &saveptr);
        if (en_passant == NULL) {
            en_passant = (char*)"-";
        }
    }

    if (charcount(board, 'K') != 1 || charcount(board, 'k') != 1) {
        return 0;
    }

    if (charcount(board, '/') != 7) {
        return 0;
    }

    rank = 1;
    row = strtok_r(board, "/", &saveptr);
    while (row != NULL) {
        pieces = 0;
        for (i = 0; i < strlen(row); i++) {
            //pawns should not be on first or last rank
            if ((rank == 1 || rank == 8) && (row[i] == 'p' || row[i] == 'P')) {
                return 0;
            }

            if (strchr("12345678bnprkqBNPRKQ", row[i]) == NULL) {
                return 0;
            }

            //sum of empty spaces and pieces should be 8
            if (row[i] >= '1' && row[i] <= '8') {
                pieces += (int)row[i] - 48;
            } else {
                pieces++;
            }
        }
        rank++;
        if (pieces != 8) {
            return 0;
        }
        row = strtok_r(NULL, "/", &saveptr);
    }

    return 1;
}

int
tag_extract(const char* str, Tag* tag) {
    int j;
    unsigned int i;
    tag->key[0] = '\0';
    tag->value[0] = '\0';
    //ignore invisible ascii characters before [
    for (i = 0; str[i] != '['; i++) {
        if ((str[i] > ' ' && str[i] <= '~') || str[i] == '\0') {
            return 0;
        }
    }
    i++; //skip [

    for (j = 0; str[i] != ' '; j++) {
        tag->key[j] = str[i++];
    }
    tag->key[j] = '\0';

    if (str[++i] != '"') {
        return 0;
    }

    i++;
    for (j = 0; str[i] != '"'; j++) {
        //handle escaped double quote
        if (str[i + 1] != '\0' && str[i] == '\\' && str[i + 1] == '"') {
            i++;
        }
        tag->value[j] = str[i++];
    }
    tag->value[j] = '\0';
    return !(strlen(str) < i + 1 || str[i] != '"' || str[i + 1] != ']');
}

void
tag_escape_value(const char* str, char* escaped, int len) {
    int j;
    int i = 0;
    if ((charcount(str, '"') + (int)strlen(str)) > len) {
        escaped[0] = '\0';
        return;
    }
    for (j = 0; str[j] != '\0'; j++) {
        if (str[j] == '"') {
            escaped[i++] = '\\';
        }
        escaped[i++] = str[j];
    }
    escaped[i] = '\0';
}

Square
filerank2square(File file, Rank rank) {
    return (Square)(16 * (rank) + (file));
}

File
square2file(Square sq) {
    return (File)((sq) & 7);
}

Rank
square2rank(Square sq) {
    return (Rank)((sq) >> 4);
}

File
char2file(char ch) {
    return (File)((ch)-97);
}

Rank
char2rank(char ch) {
    return (Rank)abs((ch - 49) - 7);
}

Piece
char2piece(char ch) {
    unsigned int i;
    for (i = 0; i < strlen(PIECE_STR); i++) {
        if (PIECE_STR[i] == ch) {
            return (Piece)i;
        }
    }
    return Empty;
}

char
file2char(File file) {
    return (char)(file + 97);
}

char
rank2char(Rank rank) {
    return (char)(abs((int)rank - 7) + 49);
}

char
piece2char(Piece piece) {
    return PIECE_STR[piece];
}

int
move_offset_find(Square src, Square dst, OffsetIndex offset_index) {
    int i, offset;
    Square sq;
    for (i = 0; piece_offset[offset_index][i] != 0; i++) {
        offset = piece_offset[offset_index][i];
        sq = (Square)(src + offset);
        while (sq != dst) {
            sq = (Square)(sq + offset);
            if (sq & 0x88) {
                break;
            }
        }
        if (sq == dst) {
            return offset;
        }
    }
    return 0;
}

void
move_uci_export(Square src, Square dst, Piece prom_piece, char* uci, int len) {
    File src_file = square2file(src);
    Rank src_rank = square2rank(src);
    File dst_file = square2file(dst);
    Rank dst_rank = square2rank(dst);
    snprintf(uci, len, "%c%c%c%c%s", file2char(src_file), rank2char(src_rank), file2char(dst_file), rank2char(dst_rank),
             piece_map[prom_piece]);
}

void
board_clear(Board* b) {
    memset(b->position, Empty, 128 * sizeof(Piece));
}

void
board_print(Board* b) {
    int i;
    printf("turn:%d KQkq:%d%d%d%d en_passant:%d half_move:%d\
            move_number:%d\n\n",
           b->turn, b->castling[White][KingSide], b->castling[White][QueenSide], b->castling[Black][KingSide],
           b->castling[Black][QueenSide], b->en_passant, b->half_move, b->move_number);
    for (i = 0; i < 128; i++) {
        if (!(i & 0x88)) {
            printf("%c ", piece2char(b->position[i]));
        }
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

Color
board_square_piece_color(Board* b, Square sq) {
    if (b->position[sq] == Empty) {
        return NoColor;
    }
    return (Color)(b->position[sq] >= WhitePawn);
}

void
board_square_set(Board* b, Square sq, Piece piece) {
    b->position[sq] = piece;
}

int
board_square_is_attacked(Board* b, Square sq, Color color) {
    int i;
    Square src;
    Piece pawn = (color == White) ? WhitePawn : BlackPawn;
    // While trying to find square where pawn came from the offset is opposite
    OffsetIndex pawn_offset = (color == White) ? OffsetBlackPawn : OffsetWhitePawn;
    Piece bishop = (color == White) ? WhiteBishop : BlackBishop;
    Piece rook = (color == White) ? WhiteRook : BlackRook;
    Piece queen = (color == White) ? WhiteQueen : BlackQueen;
    Piece king = (color == White) ? WhiteKing : BlackKing;

    for (i = 0; piece_offset[OffsetKnight][i] != 0; i++) {
        src = (Square)(sq + piece_offset[OffsetKnight][i]);
        if (src & 0x88) {
            continue;
        }

        if (board_move_knight_is_valid(b, src, sq, color)) {
            return 1;
        }
    }

    for (i = 0; piece_offset[OffsetBishop][i] != 0; i++) {
        src = (Square)(sq + piece_offset[OffsetBishop][i]);
        if (!(src & 0x88) && b->position[src] == king) {
            return 1;
        }

        while (!(src & 0x88)) {
            if (!board_move_pattern_bishop_is_valid(b, src, sq, color)) {
                break;
            }
            if (b->position[src] == bishop || b->position[src] == queen) {
                return 1;
            }
            src = (Square)(src + piece_offset[OffsetBishop][i]);
        }
    }

    for (i = 0; piece_offset[OffsetRook][i] != 0; i++) {
        src = (Square)(sq + piece_offset[OffsetRook][i]);
        if (!(src & 0x88) && b->position[src] == king) {
            return 1;
        }

        while (!(src & 0x88)) {
            if (!board_move_pattern_rook_is_valid(b, src, sq, color)) {
                break;
            }
            if (b->position[src] == rook || b->position[src] == queen) {
                return 1;
            }
            src = (Square)(src + piece_offset[OffsetRook][i]);
        }
    }
    for (i = 2; piece_offset[pawn_offset][i] != 0; i++) {
        src = (Square)(sq + piece_offset[pawn_offset][i]);
        if (src & 0x88) {
            continue;
        }

        if (b->position[src] == pawn) {
            return 1;
        }
    }

    return 0;
}

int
board_is_check(Board* b) {
    return board_square_is_attacked(b, b->kings[b->turn], (b->turn == White ? Black : White));
}

int
board_is_checkmate(Board* b) {
    return (board_is_out_of_moves(b) && board_is_check(b));
}

int
board_is_stalemate(Board* b) {
    return (board_is_out_of_moves(b) && !board_is_check(b));
}

int
board_is_out_of_moves(Board* b) {
    int i, offset, rank, file;
    Piece piece;
    Square src, dst;
    Color color = b->turn;
    Piece prom_piece = (color == White) ? WhiteQueen : BlackQueen;

    for (rank = 0; rank < 8; rank++) {
        for (file = 0; file < 8; file++) {
            src = filerank2square((File)file, (Rank)rank);
            if (board_square_piece_color(b, src) == color) {
                piece = b->position[src];
                for (i = 0; piece_offset[offset_map[piece]][i] != 0; i++) {
                    dst = src;
                    offset = piece_offset[offset_map[piece]][i];
                    dst = (Square)(dst + offset);
                    while (!(dst & 0x88)) {
                        if (board_move_status(b, src, dst, prom_piece)) {
                            return 0;
                        }
                        dst = (Square)(dst + offset);
                    }
                }
            }
        }
    }
    return 1;
}

int
board_is_equal(Board* b1, Board* b2, int strict) {
    if (b1->turn != b2->turn) {
        return 0;
    }
    if (b1->move_number != b2->move_number && strict) {
        return 0;
    }
    if (b1->half_move != b2->half_move && strict) {
        return 0;
    }
    if (b1->kings[0] != b2->kings[0] || b1->kings[1] != b2->kings[1]) {
        return 0;
    }
    if (b1->en_passant != b2->en_passant) {
        return 0;
    }
    if (b1->castling[0][0] != b2->castling[0][0] || b1->castling[0][1] != b2->castling[0][1]
        || b1->castling[1][0] != b2->castling[1][0] || b1->castling[1][1] != b2->castling[1][1]) {
        return 0;
    }
    int i, j;
    for (j = 0; j <= a1; j += 16) {
        for (i = 0; i <= 7; i++) {
            if (b1->position[i + j] != b2->position[i + j]) {
                return 0;
            }
        }
    }
    return 1;
}

int
board_move_pattern_knight_is_valid(Board* b, Square src, Square dst, Color color) {
    int i;
    for (i = 0; piece_offset[OffsetKnight][i] != 0; i++) {
        if (src + piece_offset[OffsetKnight][i] == dst) {
            return (board_square_piece_color(b, dst) != color);
        }
    }
    return 0;
}

int
board_move_pattern_bishop_is_valid(Board* b, Square src, Square dst, Color color) {
    Color src_color;
    int offset = move_offset_find(src, dst, OffsetBishop);
    if (!offset) {
        return 0;
    }

    while (src != dst) {
        src = (Square)(src + offset);
        src_color = board_square_piece_color(b, src);
        if (src_color == color) {
            src = (Square)(src - offset);
            break;
        }
        if (src_color == !color) {
            break;
        }
    }
    return (src == dst);
}

int
board_move_pattern_rook_is_valid(Board* b, Square src, Square dst, Color color) {
    Color src_color;
    int offset = move_offset_find(src, dst, OffsetRook);
    if (!offset) {
        return 0;
    }

    while (src != dst) {
        src = (Square)(src + offset);
        src_color = board_square_piece_color(b, src);
        if (src_color == color) {
            src = (Square)(src - offset);
            break;
        }
        if (src_color == !color) {
            break;
        }
    }
    return (src == dst);
}

int
board_move_knight_is_valid(Board* b, Square src, Square dst, Color color) {
    Piece knight = (color == White) ? WhiteKnight : BlackKnight;
    int valid = board_move_pattern_knight_is_valid(b, src, dst, color);
    return (valid && b->position[src] == knight);
}

int
board_move_bishop_is_valid(Board* b, Square src, Square dst, Color color) {
    Piece bishop = (color == White) ? WhiteBishop : BlackBishop;
    int valid = board_move_pattern_bishop_is_valid(b, src, dst, color);
    return (valid && b->position[src] == bishop);
}

int
board_move_rook_is_valid(Board* b, Square src, Square dst, Color color) {
    Piece rook = (color == White) ? WhiteRook : BlackRook;
    int valid = board_move_pattern_rook_is_valid(b, src, dst, color);
    return (valid && b->position[src] == rook);
}

int
board_move_queen_is_valid(Board* b, Square src, Square dst, Color color) {
    Piece queen = (color == White) ? WhiteQueen : BlackQueen;
    int valid_bishop = board_move_pattern_bishop_is_valid(b, src, dst, color);
    int valid_rook = board_move_pattern_rook_is_valid(b, src, dst, color);
    return (valid_bishop || valid_rook) && b->position[src] == queen;
}

Status
board_move_pawn_status(Board* b, Square src, Square dst, Color color) {
    int i, dst_color;
    Rank start_rank = ((color == White) ? Rank_2 : Rank_7);
    int offset = 0;
    OffsetIndex offset_index = ((color == White) ? OffsetWhitePawn : OffsetBlackPawn);
    for (i = 0; piece_offset[offset_index][i] != 0; i++) {
        if ((src + piece_offset[offset_index][i]) == dst) {
            offset = piece_offset[offset_index][i];
            break;
        }
    }
    if (!offset) {
        return Invalid;
    }

    switch (i) {
        case 0:
            if (b->position[src + offset] != Empty) {
                return Invalid;
            }
            break;

        case 1:
            if (square2rank(src) != start_rank || b->position[src + piece_offset[offset_index][0]] != Empty
                || b->position[src + offset] != Empty) {
                return Invalid;
            }
            break;

        case 2:
        case 3:
            dst_color = board_square_piece_color(b, dst);
            if (dst == b->en_passant) {
                return EnPassant;
            }
            if (dst_color != !color) {
                return Invalid;
            }
            break;
    }
    if (board_move_is_promotion(b, src, dst)) {
        return Promotion;
    }
    return Valid;
}

Status
board_move_king_status(Board* b, Square src, Square dst, Color color) {
    int i;
    Square sq;
    Color dst_color;
    int offset = 0;
    Side side = NoSide;
    Piece rook = (color == White) ? WhiteRook : BlackRook;
    Square castle_squares[][2][2] = {
        {
            {d8, c8},
            {f8, g8},
        },
        {
            {d1, c1},
            {f1, g1},
        },
    };

    Square rook_squares[][2][1] = {
        {
            {a8},
            {h8},
        },
        {
            {a1},
            {h1},
        },
    };

    if ((src == e8 && dst == g8) || (src == e1 && dst == g1)) {
        side = KingSide;
    }
    if ((src == e8 && dst == c8) || (src == e1 && dst == c1)) {
        side = QueenSide;
    }

    if (side != NoSide) {
        if (board_square_is_attacked(b, src, (color == White ? Black : White))) {
            return Invalid;
        }

        for (i = 0; i < 2; i++) {
            sq = castle_squares[color][side][i];
            if (b->position[sq] != Empty || board_square_is_attacked(b, sq, (color == White ? Black : White))) {
                return Invalid;
            }
        }

        if (b->position[rook_squares[color][side][0]] != rook) {
            return Invalid;
        }

        if (!b->castling[color][side]) {
            return Invalid;
        }
        return Castling;
    }

    for (i = 0; piece_offset[OffsetKing][i] != 0; i++) {
        if (src + piece_offset[OffsetKing][i] == dst) {
            offset = piece_offset[OffsetKing][i];
            break;
        }
    }
    if (!offset) {
        return Invalid;
    }

    dst_color = board_square_piece_color(b, dst);
    if (dst_color == color) {
        return Invalid;
    }

    return Valid;
}

int
board_move_is_promotion(Board* b, Square src, Square dst) {
    Piece piece = b->position[src];
    Rank rank = square2rank(dst);
    return ((piece == WhitePawn && rank == Rank_8) || (piece == BlackPawn && rank == Rank_1));
}

int
board_move_is_capture(Board* b, Square src, Square dst) {
    return ((b->position[dst] != Empty || dst == b->en_passant) && b->position[src] != Empty);
}

int
board_move_is_check(Board* b, Square src, Square dst, Piece prom_piece, Status status, Color color) {
    Board tmp = *b;
    board_move_do(&tmp, src, dst, prom_piece, status);
    return board_square_is_attacked(&tmp, tmp.kings[color], (color == White ? Black : White));
}

int
board_move_is_checkmate(Board* b, Square src, Square dst, Piece prom_piece, Status status) {
    Board tmp = *b;
    board_move_do(&tmp, src, dst, prom_piece, status);
    return board_is_checkmate(&tmp);
}

void
board_move_which(Board* b, Square src, Square dst, char* which) {
    int rank, file;
    Square sq;
    Square possible[10];
    int possible_count = 0;
    Piece piece = b->position[src];
    which[0] = '\0';

    for (rank = 0; rank < 8; rank++) {
        for (file = 0; file < 8; file++) {
            sq = filerank2square((File)file, (Rank)rank);
            if (b->position[sq] == piece && board_move_status(b, sq, dst, Empty) && sq != src) {
                possible[possible_count++] = sq;
            }
        }
    }

    if (possible_count >= 2) {
        snprintf(which, SQUARE_LEN, "%c%c", file2char(square2file(src)), rank2char(square2rank(src)));
    } else if (possible_count == 1) {
        snprintf(which, SQUARE_LEN, "%c", file2char(square2file(src)));
        if (square2file(possible[0]) == square2file(src)) {
            snprintf(which, SQUARE_LEN, "%c", rank2char(square2rank(src)));
        }
    }
}

Status
board_move_status(Board* b, Square src, Square dst, Piece prom_piece) {
    Status tmp_valid;
    Color color = board_square_piece_color(b, src);
    Piece piece = b->position[src];
    Status status = Valid;

    if (src == dst) {
        return Invalid;
    }
    if (color != b->turn) {
        return Invalid;
    }

    switch (piece) {
        case WhitePawn:
        case BlackPawn:
            tmp_valid = board_move_pawn_status(b, src, dst, color);
            if (tmp_valid == Invalid) {
                return Invalid;
            } else {
                status = tmp_valid;
            }
            break;

        case WhiteKnight:
        case BlackKnight:
            if (!board_move_knight_is_valid(b, src, dst, color)) {
                return Invalid;
            }
            break;

        case WhiteBishop:
        case BlackBishop:
            if (!board_move_bishop_is_valid(b, src, dst, color)) {
                return Invalid;
            }
            break;

        case WhiteRook:
        case BlackRook:
            if (!board_move_rook_is_valid(b, src, dst, color)) {
                return Invalid;
            }
            break;

        case WhiteQueen:
        case BlackQueen:
            if (!board_move_queen_is_valid(b, src, dst, color)) {
                return Invalid;
            }
            break;

        case WhiteKing:
        case BlackKing:
            tmp_valid = board_move_king_status(b, src, dst, color);
            if (tmp_valid == Invalid) {
                return Invalid;
            } else {
                status = tmp_valid;
            }
            break;

        case Empty: return Invalid;
    }

    if (board_move_is_check(b, src, dst, prom_piece, status, b->turn)) {
        return Invalid;
    }
    return status;
}

Status
board_move_san_status(Board* b, const char* san, Square* src, Square* dst, Piece* prom_piece) {
    char san_str[SAN_LEN];
    snprintf(san_str, SAN_LEN, "%s", san);
    trimmove(san_str);
    Piece piece = Empty;
    Status status = Invalid;
    *src = none;
    *prom_piece = Empty;
    Color color = b->turn;
    File file_hint = File_none;
    Rank rank_hint = Rank_none;
    int file_index = 1;
    int rank_index = 2;
    int last = strlen(san) - 1;
    int rank, file;

    switch (san[0]) {
        case 'B':
        case 'N':
        case 'R':
        case 'Q':
        case 'K':
            //to lower if black
            piece = char2piece(color == Black ? san_str[0] + 32 : san_str[0]);
            switch (last - rank_index) {
                case 1:
                    file_index = 2;
                    rank_index = 3;
                    if (san_str[1] >= 'a' && san_str[1] <= 'h') {
                        file_hint = char2file(san_str[1]);
                    } else if (san_str[1] >= '1' && san_str[1] <= '8') {
                        rank_hint = char2rank(san_str[1]);
                    } else {
                        return Invalid;
                    }
                    break;

                case 2:
                    file_index = 3;
                    rank_index = 4;
                    *src = filerank2square(char2file(san_str[1]), char2rank(san_str[2]));
                    break;

                default: break;
            }
            *dst = filerank2square(char2file(san_str[file_index]), char2rank(san_str[rank_index]));
            break;

        case 'O':
            if (!strcmp(san_str, CASTLE_STR_SHORT)) {
                *src = (color == White) ? e1 : e8;
                *dst = (color == White) ? g1 : g8;
            } else if (!strcmp(san_str, CASTLE_STR_LONG)) {
                *src = (color == White) ? e1 : e8;
                *dst = (color == White) ? c1 : c8;
            }
            break;

        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
            piece = (color == White) ? WhitePawn : BlackPawn;
            if (san_str[1] >= 'a' && san_str[1] <= 'h') { //capture
                file_hint = char2file(san_str[0]);
            } else {
                file_index = 0;
                rank_index = 1;
            }
            *dst = filerank2square(char2file(san_str[file_index]), char2rank(san_str[rank_index]));
            if (strchr("BNRQ", san_str[last]) != NULL) {
                *prom_piece = char2piece(color == Black ? san_str[last] + 32 : san_str[last]);
            }
            break;

        default: return Invalid; break;
    }

    if (*src == none) {
        for (rank = (rank_hint == Rank_none ? 0 : rank_hint); rank <= (rank_hint == Rank_none ? 7 : rank_hint);
             rank++) {
            for (file = (file_hint == File_none ? 0 : file_hint); file <= (file_hint == File_none ? 7 : file_hint);
                 file++) {
                *src = filerank2square((File)file, (Rank)rank);
                if (b->position[*src] == piece) {
                    status = board_move_status(b, *src, *dst, *prom_piece);
                    if (status != Invalid) {
                        rank = 8;
                        file = 8;
                    }
                }
            }
        }
    } else {
        status = board_move_status(b, *src, *dst, *prom_piece);
    }
    return status;
}

Status
board_move_uci_status(Board* b, const char* uci, Square* src, Square* dst, Piece* prom_piece) {
    *src = none;
    *prom_piece = Empty;
    char src_str[3];
    char dst_str[3];

    if (strlen(uci) != 4 && strlen(uci) != 5) {
        return Invalid;
    }

    snprintf(src_str, 3, "%c%c", uci[0], uci[1]);
    snprintf(dst_str, 3, "%c%c", uci[2], uci[3]);

    if (!str_is_square(src_str) || !str_is_square(dst_str)) {
        return Invalid;
    }

    if (strlen(uci) == 5) {
        *prom_piece = char2piece(uci[4]);
    }

    *src = filerank2square(char2file(uci[0]), char2rank(uci[1]));
    *dst = filerank2square(char2file(uci[2]), char2rank(uci[3]));

    return board_move_status(b, *src, *dst, *prom_piece);
}

void
board_move_do(Board* b, Square src, Square dst, Piece prom_piece, Status status) {
    b->half_move++;
    if (b->position[dst] != Empty) {
        b->half_move = 0;
    }
    b->position[dst] = b->position[src];
    b->position[src] = Empty;

    switch (dst) {
        case a1: b->castling[White][QueenSide] = 0; break;

        case a8: b->castling[Black][QueenSide] = 0; break;

        case h1: b->castling[White][KingSide] = 0; break;

        case h8: b->castling[Black][KingSide] = 0; break;

        default: break;
    }

    switch (status) {
        case Castling:
            switch (dst) {
                case g1:
                    b->position[h1] = Empty;
                    b->position[f1] = WhiteRook;
                    break;

                case g8:
                    b->position[h8] = Empty;
                    b->position[f8] = BlackRook;
                    break;

                case c1:
                    b->position[a1] = Empty;
                    b->position[d1] = WhiteRook;
                    break;

                case c8:
                    b->position[a8] = Empty;
                    b->position[d8] = BlackRook;
                    break;

                default: break;
            }
            break;

        case EnPassant:
            if (b->turn == White) {
                b->position[dst - piece_offset[OffsetWhitePawn][0]] = Empty;
            } else {
                b->position[dst - piece_offset[OffsetBlackPawn][0]] = Empty;
            }
            break;

        case Promotion: b->position[dst] = prom_piece; break;

        default: break;
    }

    b->en_passant = none;
    switch (b->position[dst]) {
        case BlackKing:
            b->kings[Black] = dst;
            b->castling[Black][KingSide] = 0;
            b->castling[Black][QueenSide] = 0;
            break;

        case WhiteKing:
            b->kings[White] = dst;
            b->castling[White][KingSide] = 0;
            b->castling[White][QueenSide] = 0;
            break;

        case BlackPawn:
            if (dst - src == piece_offset[OffsetBlackPawn][1]) {
                b->en_passant = (Square)(dst - piece_offset[OffsetBlackPawn][0]);
            }
            b->half_move = 0;
            break;

        case WhitePawn:
            if (dst - src == piece_offset[OffsetWhitePawn][1]) {
                b->en_passant = (Square)(dst - piece_offset[OffsetWhitePawn][0]);
            }
            b->half_move = 0;
            break;

        case BlackRook:
            if (src == h8) {
                b->castling[Black][KingSide] = 0;
            }
            if (src == a8) {
                b->castling[Black][QueenSide] = 0;
            }
            break;

        case WhiteRook:
            if (src == h1) {
                b->castling[White][KingSide] = 0;
            }
            if (src == a1) {
                b->castling[White][QueenSide] = 0;
            }
            break;

        default: break;
    }

    if (b->turn == Black) {
        b->move_number++;
    }
    b->turn = b->turn == White ? Black : White;
}

void
board_move_san_export(Board* b, Square src, Square dst, Piece prom_piece, char* san, int len, Status status) {
    OffsetIndex offset_index;
    char which[SQUARE_LEN];
    char promotion[SQUARE_LEN];
    Piece piece = b->position[src];
    const char* piece_char = piece_map[piece];
    const char* prom_char = piece_map[prom_piece];
    char file = file2char(square2file(dst));
    char rank = rank2char(square2rank(dst));
    const char* capture = board_move_is_capture(b, src, dst) ? "x" : "";
    const char* check = board_move_is_check(b, src, dst, prom_piece, status, (b->turn == White ? Black : White)) ? "+"
                                                                                                                 : "";

    if (board_move_is_checkmate(b, src, dst, prom_piece, status)) {
        check = "#";
    }

    board_move_which(b, src, dst, which);

    snprintf(san, len, "%s%s%s%c%c%s", piece_char, which, capture, file, rank, check);
    switch (piece) {
        case BlackKing:
        case WhiteKing:
            if (src == e1 || src == e8) {
                if (dst == g1 || dst == g8) {
                    snprintf(san, len, "%s%s", CASTLE_STR_SHORT, check);
                }
                if (dst == c1 || dst == c8) {
                    snprintf(san, len, "%s%s", CASTLE_STR_LONG, check);
                }
            }
            break;

        case BlackPawn:
        case WhitePawn:
            offset_index = offset_map[piece];
            promotion[0] = '\0';
            if (board_move_is_promotion(b, src, dst)) {
                snprintf(promotion, SQUARE_LEN, "=%s", prom_char);
            }
            snprintf(san, len, "%s%s%s%c%c%s%s", piece_char, which, capture, file, rank, promotion, check);

            if (dst - src == piece_offset[offset_index][2] || dst - src == piece_offset[offset_index][3]) {
                snprintf(san, len, "%s%c%s%c%c%s%s", piece_char, file2char(square2file(src)), capture, file, rank,
                         promotion, check);
            }
            break;

        default: break;
    }
}

void
board_fen_import(Board* b, const char* FEN_str) {
    memset(b, 0, sizeof(Board));
    char fen[FEN_LEN];
    char* saveptr;
    unsigned int i;
    Rank rank = (Rank)0;
    int file = 0;
    Square en_passant;
    snprintf(fen, FEN_LEN, "%s", FEN_str);
    char* p = strtok_r(fen, " ", &saveptr);
    for (i = 0; i < strlen(p); i++) {
        if (p[i] >= '1' && p[i] <= '8') {
            file += (p[i] - 48);
        } else if (p[i] == '/') {
            rank = (Rank)(rank + 1);
            file = 0;
        } else {
            b->position[filerank2square((File)file, rank)] = char2piece(p[i]);
            if (p[i] == 'K' || p[i] == 'k') {
                b->kings[(p[i] == 'K' ? White : Black)] = filerank2square((File)file, rank);
            }
            file++;
        }
    }
    p = strtok_r(NULL, " ", &saveptr);
    b->turn = (p[0] == 'w' ? White : Black);

    p = strtok_r(NULL, " ", &saveptr);
    for (i = 0; i < strlen(p); i++) {
        switch (p[i]) {
            case 'K': b->castling[White][KingSide] = 1; break;
            case 'Q': b->castling[White][QueenSide] = 1; break;
            case 'k': b->castling[Black][KingSide] = 1; break;
            case 'q': b->castling[Black][QueenSide] = 1; break;
        }
    }

    b->half_move = 0;
    b->move_number = 1;
    p = strtok_r(NULL, " ", &saveptr);

    en_passant = filerank2square(char2file(p[0]), char2rank(p[1]));
    b->en_passant = !strcmp(p, "-") ? none : en_passant;

    p = strtok_r(NULL, " ", &saveptr);
    if (p == NULL) {
        return;
    }
    b->half_move = strtol(p, NULL, 10);

    p = strtok_r(NULL, " ", &saveptr);
    if (p == NULL) {
        return;
    }
    b->move_number = strtol(p, NULL, 10);
}

void
board_fen_export(Board* b, char* FEN_str) {
    memset(FEN_str, 0, FEN_LEN);
    int rank, file;
    Piece piece;
    int empty;
    for (rank = 0; rank < 8; rank++) {
        empty = 0;
        for (file = 0; file < 8; file++) {
            piece = b->position[filerank2square((File)file, (Rank)rank)];
            if (piece > 0) {
                if (empty) {
                    concate(FEN_str, FEN_LEN, "%c", (48 + empty));
                    empty = 0;
                }
                concate(FEN_str, FEN_LEN, "%c", piece2char(piece));
            } else {
                empty++;
            }
        }
        if (empty) {
            concate(FEN_str, FEN_LEN, "%c", (48 + empty));
        }
        concate(FEN_str, FEN_LEN, "%c", (rank == 7 ? ' ' : '/'));
    }
    concate(FEN_str, FEN_LEN, "%c ", (b->turn ? 'w' : 'b'));

    if (b->castling[White][KingSide]) {
        concate(FEN_str, FEN_LEN, "%c", 'K');
    }
    if (b->castling[White][QueenSide]) {
        concate(FEN_str, FEN_LEN, "%c", 'Q');
    }
    if (b->castling[Black][KingSide]) {
        concate(FEN_str, FEN_LEN, "%c", 'k');
    }
    if (b->castling[Black][QueenSide]) {
        concate(FEN_str, FEN_LEN, "%c", 'q');
    }
    if (FEN_str[strlen(FEN_str) - 1] == ' ') {
        concate(FEN_str, FEN_LEN, "%c", '-');
    }
    concate(FEN_str, FEN_LEN, "%c", ' ');

    if (b->en_passant == none) {
        concate(FEN_str, FEN_LEN, "%c ", '-');
    } else {
        concate(FEN_str, FEN_LEN, "%c%c ", file2char(square2file(b->en_passant)),
                rank2char(square2rank(b->en_passant)));
    }
    concate(FEN_str, FEN_LEN, "%d %d", (b->half_move), b->move_number);
}

void
move_init(Move* m) {
    m->variation_list = NULL;
    m->variation_count = 0;
    m->comment = NULL;
    m->nag_move = 0;
    m->nag_position = 0;
}

void
move_free(Move* m) {
    int i;
    if (m->comment != NULL) {
        free(m->comment);
    }
    for (i = 0; i < m->variation_count; i++) {
        variation_free(m->variation_list[i]);
        free(m->variation_list[i]);
    }
    if (m->variation_list != NULL) {
        free(m->variation_list);
    }
}

void
move_copy(Move* src, Move* dst, Variation* prev) {
    int i;
    dst->src = src->src;
    dst->dst = src->dst;
    dst->prom_piece = src->prom_piece;
    dst->board = src->board;
    snprintf(dst->san, SAN_LEN, "%s", src->san);
    dst->comment = NULL;
    if (src->comment != NULL) {
        dst->comment = strdup(src->comment);
    }
    dst->nag_move = src->nag_move;
    dst->nag_position = src->nag_position;
    dst->variation_count = src->variation_count;
    dst->variation_list = NULL;
    if (src->variation_count) {
        dst->variation_list = (Variation**)malloc(sizeof(Variation*) * src->variation_count);
    }
    for (i = 0; i < src->variation_count; i++) {
        dst->variation_list[i] = variation_clone(src->variation_list[i], prev);
    }
}

int
move_variation_find(Move* m, Variation* v) {
    int i;
    for (i = 0; i < m->variation_count; i++) {
        if (m->variation_list[i] == v) {
            return i;
        }
    }
    return -1;
}

int
move_variation_delete(Move* m, Variation* v) {
    int i, j;
    i = move_variation_find(m, v);
    if (i != -1) {
        m->variation_list[i] = NULL;
        for (j = i + 1; j < m->variation_count; j++) {
            m->variation_list[j - 1] = m->variation_list[j];
        }
        m->variation_count--;
        variation_free(v);
        free(v);
        return 1;
    }
    return 0;
}

void
variation_init(Variation* v, Board* b) {
    v->move_list = (Move*)malloc(sizeof(Move));
    v->move_current = 0;
    move_init(&v->move_list[0]);
    v->move_list[0].src = none;
    v->move_list[0].dst = none;
    v->move_list[0].prom_piece = Empty;
    if (b == NULL) {
        b = (Board*)malloc(sizeof(Board));
        board_fen_import(b, FEN_DEFAULT);
        v->move_list[0].board = *b;
        free(b);
    } else {
        v->move_list[0].board = *b;
    }
    v->move_list[0].san[0] = '\0';
    v->move_count = 1;
    v->prev = NULL;
}

void
variation_free(Variation* v) {
    int i;
    for (i = 0; i < v->move_count; i++) {
        move_free(&v->move_list[i]);
    }
    free(v->move_list);
}

void
variation_delete_next_moves(Variation* v) {
    int i;
    for (i = v->move_current + 1; i < v->move_count; i++) {
        move_free(&v->move_list[i]);
    }
    v->move_count = v->move_current + 1;
}

void
variation_move_add(Variation* v, Square src, Square dst, Piece prom_piece, Board* b, const char* san) {
    v->move_current++;
    v->move_count++;
    v->move_list = (Move*)realloc(v->move_list, sizeof(Move) * v->move_count);
    move_init(&v->move_list[v->move_current]);
    v->move_list[v->move_current].src = src;
    v->move_list[v->move_current].dst = dst;
    v->move_list[v->move_current].prom_piece = prom_piece;
    v->move_list[v->move_current].board = *b;
    snprintf(v->move_list[v->move_current].san, SAN_LEN, "%s", san);
}

int
variation_move_next(Variation* v) {
    if (v->move_current + 1 >= v->move_count) {
        return 0;
    }
    v->move_current++;
    return 1;
}

int
variation_move_prev(Variation* v) {
    if (v->move_current <= 0) {
        return 0;
    }
    v->move_current--;
    return 1;
}

int
variation_index_find(Variation* v, Variation* prev) {
    int i;
    for (i = 0; i < prev->move_count; i++) {
        if (move_variation_find(&prev->move_list[i], v) != -1) {
            return i;
        }
    }
    return -1;
}

Move*
variation_move_get(Variation* v) {
    return &v->move_list[v->move_current];
}

void
variation_movenumber_export(Variation* v, int index, int prev_index, char* num, int len) {
    num[0] = '\0';
    int movenumber = (index / 2) + (prev_index / 2) + 1;

    //after white's move it is black's turn
    if (v->move_list[index].board.turn == Black) {
        snprintf(num, len, "%d.", movenumber);
    }
    //add 3 dots when first move is from black
    else if (index == 1) {
        snprintf(num, len, "%d...", movenumber);
    }
}

Variation*
variation_equivalent_find(Variation* orig, Variation* clone, Variation* v) {
    int i, j;
    Variation* tmp;
    if (orig == v) {
        return clone;
    }
    for (i = 0; i < orig->move_count; i++) {
        for (j = 0; j < orig->move_list[i].variation_count; j++) {
            if (orig->move_list[i].variation_list[j] == v) {
                return clone->move_list[i].variation_list[j];
            }
            tmp = variation_equivalent_find(orig->move_list[i].variation_list[j], clone->move_list[i].variation_list[j],
                                            v);
            if (tmp != NULL) {
                return tmp;
            }
        }
    }
    return NULL;
}

Variation*
variation_clone(Variation* v, Variation* prev) {
    int i;
    Variation* clone = (Variation*)malloc(sizeof(Variation));
    clone->move_list = (Move*)malloc(sizeof(Move) * v->move_count);
    clone->move_current = v->move_current;
    clone->move_count = v->move_count;
    for (i = 0; i < v->move_count; i++) {
        move_copy(&v->move_list[i], &clone->move_list[i], clone);
    }
    clone->prev = prev;
    return clone;
}

int
variation_board_find(Variation* v, Board* b, Variation** found) {
    *found = *found ? *found : NULL;
    int i, j, index;
    index = -1;
    for (i = 0; i < v->move_count; i++) {
        if (board_is_equal(&v->move_list[i].board, b, 0)) {
            *found = v;
            return i;
        }
        for (j = 0; j < v->move_list[i].variation_count; j++) {
            index = variation_board_find(v->move_list[i].variation_list[j], b, found);
            if (index > -1 && *found != NULL) {
                return index;
            }
        }
    }
    return index;
}

void
vs_init(VariationSequence* vs) {
    vs->list = NULL;
    vs->count = 0;
}

void
vs_free(VariationSequence* vs) {
    if (vs->list != NULL) {
        free(vs->list);
    }
}

void
vs_add(VariationSequence* vs, int index, int count, int searched) {
    vs->count++;
    vs->list = (VariationBranch*)realloc(vs->list, sizeof(VariationBranch) * vs->count);
    vs->list[vs->count - 1].index = index;
    vs->list[vs->count - 1].count = count;
    vs->list[vs->count - 1].searched = searched;
}

void
vs_remove_last(VariationSequence* vs) {
    if (vs->count == 0) {
        return;
    }
    vs->count--;
    vs->list = (VariationBranch*)realloc(vs->list, sizeof(VariationBranch) * vs->count);
}

int
vs_is_equal(VariationSequence* vs1, VariationSequence* vs2) {
    if (vs1->count != vs2->count) {
        return 0;
    }

    int i;
    for (i = 0; i < vs1->count; i++) {
        if (vs1->list[i].index != vs2->list[i].index || vs1->list[i].count != vs2->list[i].count) {
            return 0;
        }
    }
    return 1;
}

void
vs_generate_first(VariationSequence* vs, Variation* v, Color color) {
    int i, num;
    Move* m;
    for (i = v->move_current; i < v->move_count; i++) {
        m = &v->move_list[i];
        if (m->variation_count > 0) {
            if (color == m->board.turn) {
                continue;
            }
            num = (v->move_current + 1 == v->move_count) + m->variation_count;
            vs_add(vs, 0, num, 0);
        }
    }
}

void
vs_generate_next(VariationSequence* vs, Variation* v, VariationSequence* prev, Color color) {
    int i, j, l;
    Move* m;
    Variation* tmp = v;

    //copy prev
    for (i = 0; i < prev->count; i++) {
        vs_add(vs, prev->list[i].index, prev->list[i].count, prev->list[i].searched);
    }
    for (j = prev->count - 1; j > 0 && prev->list[j].index == prev->list[j].count && prev->list[j].searched; j--) {
        vs_remove_last(vs);
    }
    vs->list[j].index++;
    vs->list[j].searched = 0;

    l = 0;
    for (i = tmp->move_current; i < tmp->move_count; i++) {
        m = &tmp->move_list[i];
        if (m->variation_count > 0) {
            if (color == m->board.turn) {
                continue;
            }
            if (l < vs->count) {
                if (vs->list[l].index > 0) { //if subvariation
                    tmp = m->variation_list[vs->list[l].index - 1];
                    i = 0;

                    if (l == j) {
                        vs->list[j].searched = 1;
                        vs_generate_first(vs, tmp, color);
                    }
                }
            }
            l++;
        }
    }
    vs->list[vs->count - 1].searched = 1;
}

int
vs_can_generate_next(VariationSequence* vs) {
    int i;
    for (i = 0; i < vs->count; i++) {
        if (vs->list[i].index != vs->list[i].count || !vs->list[i].searched) {
            return 1;
        }
    }
    return 0;
}

void
vs_print(VariationSequence* vs) {
    int i;
    for (i = 0; i < vs->count; i++) {
        printf("(%d/%d %d) ", vs->list[i].index, vs->list[i].count, vs->list[i].searched);
    }
    printf("\n");
}

void
game_tag_init(Game* g) {
    g->tag_list = (Tag*)malloc(sizeof(Tag));
    g->tag_count = 0;
    game_tag_set(g, "Event", "");
    game_tag_set(g, "Site", "");
    game_tag_set(g, "Date", "");
    game_tag_set(g, "Round", "");
    game_tag_set(g, "White", "");
    game_tag_set(g, "Black", "");
    game_tag_set(g, "Result", "*");
}

void
game_tag_free(Game* g) {
    if (g->tag_count > 0) {
        free(g->tag_list);
    }
}

int
game_tag_index(Game* g, const char* key) {
    int i;
    for (i = 0; i < g->tag_count; i++) {
        if (!strcmp(g->tag_list[i].key, key)) {
            return i;
        }
    }
    return -1;
}

void
game_tag_add(Game* g, Tag* tag) {
    int last = g->tag_count;
    g->tag_count++;
    g->tag_list = (Tag*)realloc(g->tag_list, sizeof(Tag) * g->tag_count);
    g->tag_list[last] = *tag;
}

void
game_tag_remove(Game* g, const char* key) {
    int i = game_tag_index(g, key);
    if (i == -1) {
        return;
    }

    int j;
    for (j = 1; j + i < g->tag_count; j++) {
        g->tag_list[i + (j - 1)] = g->tag_list[i + j];
    }
    g->tag_count--;
    g->tag_list = (Tag*)realloc(g->tag_list, sizeof(Tag) * g->tag_count);
}

Tag*
game_tag_get(Game* g, const char* key) {
    int index = game_tag_index(g, key);
    if (index == -1) {
        return NULL;
    }
    return &g->tag_list[index];
}

void
game_tag_set(Game* g, const char* key, const char* value) {
    Tag tag;
    int index = game_tag_index(g, key);
    if (index == -1) {
        snprintf(tag.key, TAG_LEN, "%s", key);
        snprintf(tag.value, TAG_LEN, "%s", value);
        game_tag_add(g, &tag);
    } else {
        snprintf(g->tag_list[index].value, TAG_LEN, "%s", value);
    }
}

void
game_init(Game* g, Board* b) {
    g->line_main = (Variation*)malloc(sizeof(Variation));
    g->line_current = g->line_main;
    variation_init(g->line_main, b);
    game_tag_init(g);
}

void
game_free(Game* g) {
    variation_free(g->line_main);
    game_tag_free(g);
    free(g->line_main);
}

Game*
game_clone(Game* g) {
    Game* clone = (Game*)malloc(sizeof(Game));
    clone->line_main = variation_clone(g->line_main, NULL);
    clone->line_current = variation_equivalent_find(g->line_main, clone->line_main, g->line_current);
    clone->tag_count = g->tag_count;
    clone->tag_list = (Tag*)malloc(clone->tag_count * sizeof(Tag));
    memcpy(clone->tag_list, g->tag_list, g->tag_count * sizeof(Tag));
    return clone;
}

int
game_move_is_last(Game* g) {
    return g->line_current->move_current + 1 == g->line_current->move_count;
}

int
game_move_is_present(Game* g, Square src, Square dst, Piece prom_piece) {
    int i;
    Move* m;

    if (!game_move_is_last(g)) {
        m = &g->line_current->move_list[g->line_current->move_current + 1];
        if (m->src == src && m->dst == dst && m->prom_piece == prom_piece) {
            return 1;
        }
    }
    for (i = 0; i < game_move_get(g)->variation_count; i++) {
        m = variation_move_get(game_move_get(g)->variation_list[i]);
        if (m->src == src && m->dst == dst && m->prom_piece == prom_piece) {
            return 1;
        }
    }
    return 0;
}

int
game_line_is_main(Game* g) {
    return g->line_current == g->line_main;
}

Move*
game_move_get(Game* g) {
    return variation_move_get(g->line_current);
}

int
game_move_index_get(Game* g) {
    return g->line_current->move_current;
}

int
game_move_index_set(Game* g, int index) {
    int old = g->line_current->move_current;
    g->line_current->move_current = index;
    return old != index;
}

Status
game_move_status(Game* g, Square src, Square dst, Piece prom_piece) {
    return board_move_status(&game_move_get(g)->board, src, dst, prom_piece);
}

Status
game_move_san_status(Game* g, const char* san, Square* src, Square* dst, Piece* prom_piece) {
    return board_move_san_status(&game_move_get(g)->board, san, src, dst, prom_piece);
}

void
game_move_add(Game* g, Square src, Square dst, Piece prom_piece, Status status) {
    char san[SAN_LEN];
    Board b = game_move_get(g)->board;
    board_move_san_export(&b, src, dst, prom_piece, san, SAN_LEN, status);
    board_move_do(&b, src, dst, prom_piece, status);
    variation_move_add(g->line_current, src, dst, prom_piece, &b, san);
}

void
game_variation_add(Game* g, Square src, Square dst, Piece prom_piece, Status status) {
    char san[SAN_LEN];
    Board b = game_move_get(g)->board;
    board_move_san_export(&b, src, dst, prom_piece, san, SAN_LEN, status);
    board_move_do(&b, src, dst, prom_piece, status);
    Variation* v = g->line_current;
    Move* m = &v->move_list[v->move_current];
    m->variation_count++;
    m->variation_list = (Variation**)realloc(m->variation_list, sizeof(Variation*) * m->variation_count);
    m->variation_list[m->variation_count - 1] = (Variation*)malloc(sizeof(Variation));
    Variation* new_v = m->variation_list[m->variation_count - 1];
    variation_init(new_v, &b);
    new_v->move_list[0].src = src;
    new_v->move_list[0].dst = dst;
    new_v->move_list[0].prom_piece = prom_piece;
    new_v->prev = v;
    snprintf(new_v->move_list[0].san, SAN_LEN, "%s", san);
    variation_move_add(new_v, src, dst, prom_piece, &b, san);
    g->line_current = new_v;
}

void
game_variation_delete(Game* g) {
    if (g->line_current == g->line_main) {
        return;
    }
    Variation* deleted = g->line_current;
    g->line_current = g->line_current->prev;

    int i;
    for (i = 0; i < g->line_current->move_count; i++) {
        if (move_variation_delete(&g->line_current->move_list[i], deleted)) {
            break;
        }
    }
}

void
game_variation_promote(Game* g) {
    if (g->line_current == g->line_main) {
        return;
    }

    Variation* v = g->line_current;
    Variation* parent = v->prev;
    Variation* tmp_v;
    Variation** tmp_list;
    int tmp_count;
    int i, j;
    int l = -1;

    //find move and variation index of promoted variation
    for (i = 0; i < parent->move_count; i++) {
        l = move_variation_find(&parent->move_list[i], v);
        if (l != -1) {
            break;
        }
    }

    if (l == -1) {
        return;
    }

    if (parent->move_list[i].variation_count < 1) {
        return;
    }

    if (parent->move_list[i].variation_list[l] != v) {
        return;
    }

    //tmp_v is ex parent variation
    tmp_v = (Variation*)malloc(sizeof(Variation));
    variation_init(tmp_v, &parent->move_list[i].board);
    tmp_v->move_count = parent->move_count - i;
    tmp_v->move_list = (Move*)realloc(tmp_v->move_list, sizeof(Move) * tmp_v->move_count);
    tmp_v->prev = parent;
    tmp_v->move_current = 1;

    //remove variations from ex parent variation
    parent->move_list[i].variation_list[l] = tmp_v;
    tmp_list = parent->move_list[i].variation_list;
    tmp_count = parent->move_list[i].variation_count;
    parent->move_list[i].variation_list = NULL;
    parent->move_list[i].variation_count = 0;

    //moves after i are copied to tmp_v
    for (j = 0; j + i < parent->move_count; j++) {
        tmp_v->move_list[j] = parent->move_list[j + i];
    }

    //update prev on sub variations of ex parent variation
    for (j = 0; j < tmp_v->move_count; j++) {
        for (l = 0; l < tmp_v->move_list[j].variation_count; l++) {
            tmp_v->move_list[j].variation_list[l]->prev = tmp_v;
        }
    }

    //prevent commment duplication on 0 move
    parent->move_list[i].comment = NULL;

    //resize parent variation and add moves from ex sub variation
    parent->move_list = (Move*)realloc(parent->move_list, sizeof(Move) * (i + v->move_count));
    parent->prev = parent->prev; //this is fine
    parent->move_count = i + v->move_count;
    for (j = 1; j < v->move_count; j++) {
        parent->move_list[i + j] = v->move_list[j];
    }

    //update prev on sub variations of new parent variation
    for (j = i + 1; j < parent->move_count; j++) {
        for (l = 0; l < parent->move_list[j].variation_count; l++) {
            parent->move_list[j].variation_list[l]->prev = parent;
        }
    }

    //put variations back
    parent->move_list[i].variation_list = tmp_list;
    parent->move_list[i].variation_count = tmp_count;

    //promoted variation becomes current
    parent->move_current = i + v->move_current;
    g->line_current = parent;

    //free ex sub variation
    free(v->move_list);
    free(v);
}

void
game_variation_insert(Game* g) {
    int i, j;
    Board b;
    Move m;
    Status status;

    if (g->line_current == g->line_main) {
        return;
    }

    Variation* v = g->line_current;
    Variation* parent = v->prev;

    j = variation_index_find(v, parent);
    if (j == -1) {
        return;
    }

    b = variation_move_get(v)->board;
    for (i = j + 2; i < parent->move_count; i++) {
        m = parent->move_list[i];
        status = board_move_status(&b, m.src, m.dst, m.prom_piece);
        if (status != Invalid) {
            b = variation_move_get(v)->board;
            board_move_do(&b, m.src, m.dst, m.prom_piece, status);
            variation_move_add(v, m.src, m.dst, m.prom_piece, &b, m.san);
        } else {
            break;
        }
    }
}

void
game_board_find(Game* g, Board* b) {
    Variation* found = NULL;
    int index = variation_board_find(g->line_main, b, &found);
    if (index != -1 && found != NULL) {
        g->line_current = found;
        g->line_current->move_current = index;
    }
}

void
pgn_read_next(FILE* f, int tags) {
    char *tmp, *saveptr;
    char buffer[BUFFER_LEN];
    char result[10] = "*";
    Tag tag;
    while (fgets(buffer, BUFFER_LEN, f)) {
        trimendl(buffer);
        if (tags) {
            if (tag_extract(buffer, &tag)) {
                if (!strcmp(tag.key, "Result")) {
                    snprintf(result, 10, "%s", tag.value);
                }
            }
        } else {
            tmp = strtok_r(buffer, " ", &saveptr);
            while (tmp != NULL && strcmp(tmp, result)) {
                tmp = strtok_r(NULL, " ", &saveptr);
            }
        }
        if (strlen(buffer) == 0) {
            if (tags) {
                tags = 0;
            } else {
                break;
            }
        }
    }
}

int
pgn_read_file(FILE* f, Game* g, int index) {
    char buffer[BUFFER_LEN];
    char word[WORD_LEN];
    char comment[COMMENT_LEN];
    char san[SAN_LEN];
    char result[10];
    snprintf(result, 10, "*");
    char* tmp;
    char* saveptr;
    int i, comment_start, comment_end, variation_start, variation_end;
    int comments = 0;
    int anglebrackets = 0; //pgn standard
    int nags = 0;
    int nag = 0;
    Tag tag;
    Status status;
    Square src, dst;
    Piece prom_piece;
    Board b;
    Variation *v, *new_v;
    Move* m;

    comment[0] = '\0';
    board_fen_import(&b, FEN_DEFAULT);
    v = g->line_main;

    for (i = 0; i < index; i++) {
        pgn_read_next(f, 1);
    }

    while (fgets(buffer, BUFFER_LEN, f)) {

        trimendl(buffer);
        if (strlen(buffer) == 0) { //skip empty lines
            continue;
        }

        if (tag_extract(buffer, &tag)) {
            game_tag_set(g, tag.key, tag.value);
            if (!strcmp(tag.key, "Result")) {
                snprintf(result, 10, "%s", tag.value);
            }
            if (!strcmp(tag.key, "FEN")) {
                board_fen_import(&b, tag.value);
                g->line_main->move_list[0].board = b;
            }
        } else {
            //skip lines starting with %
            tmp = buffer[0] != '%' ? strtok_r(buffer, " ", &saveptr) : NULL;
            while (tmp != NULL && !(comments == 0 && !strcmp(tmp, result))) {
                variation_start = 0;
                variation_end = 0;
                comment_start = charcount(tmp, '{');
                comment_end = charcount(tmp, '}');

                comments += comment_start;

                if (comments == 0) {
                    variation_start = charcount(tmp, '(');
                    variation_end = charcount(tmp, ')');
                    anglebrackets += charcount(tmp, '<');
                    nags = charcount(tmp, '$');
                } else {
                    concate(comment, COMMENT_LEN, "%s ", tmp);
                }

                if (comment_start) {
                    variation_start = charcount_before(tmp, '(', '{');
                }

                if (comment_end) {
                    variation_end = charcount_after(tmp, ')', '}');
                }

                if (variation_start) {
                    m = &v->move_list[v->move_current - 1];
                    b = m->board;
                    m->variation_count++;
                    m->variation_list = (Variation**)realloc(m->variation_list,
                                                             sizeof(Variation*) * m->variation_count);
                    new_v = (Variation*)malloc(sizeof(Variation));
                    variation_init(new_v, &b);
                    m->variation_list[m->variation_count - 1] = new_v;
                    new_v->prev = v;
                    v = new_v;
                }

                //parse NAG numbers
                if (nags == 1) {
                    snprintf(word, WORD_LEN, "%s", tmp);
                    charremove(word, 0);
                    nag = strtol(word, NULL, 10);
                    if (nag < 10) {
                        v->move_list[v->move_current].nag_move = nag;
                    } else {
                        v->move_list[v->move_current].nag_position = nag;
                    }
                    nags = 0;
                }

                //parse SAN moves
                if (comments == 0 && anglebrackets == 0 && charcount(tmp, '.') < 2 && nags == 0) {
                    snprintf(word, WORD_LEN, "%s", tmp);
                    trimmove(word);
                    if (str_is_move(word)) {

                        status = board_move_san_status(&b, word, &src, &dst, &prom_piece);
                        if (status == Invalid) {
                            return 0;
                        }
                        board_move_san_export(&b, src, dst, prom_piece, san, SAN_LEN, status);
                        board_move_do(&b, src, dst, prom_piece, status);
                        variation_move_add(v, src, dst, prom_piece, &b, san);
                    }
                }

                //if comment is finished add it to current move
                if (comments > 0 && comment_end) {
                    trimcomment(comment);
                    if (v->move_list[v->move_current].comment == NULL) {
                        v->move_list[v->move_current].comment = (char*)malloc(sizeof(char) * COMMENT_LEN);
                        snprintf(v->move_list[v->move_current].comment, strlen(comment) + 1, "%s", comment);
                    }
                }

                while (variation_end--) {
                    v->move_current = 1;
                    v = v->prev;
                    b = v->move_list[v->move_current].board;
                }

                comments -= comment_end;
                if (comments == 0) {
                    if (comment_end > 0) {
                        anglebrackets -= charcount_after(tmp, '>', '}');
                    } else {
                        anglebrackets -= charcount(tmp, '>');
                    }
                }
                tmp = strtok_r(NULL, " ", &saveptr);
            }
            if (tmp != NULL && (comments == 0 && !strcmp(tmp, result))) {
                break;
            }
        }
    }
    v->move_current = 0;
    return 1;
}

void
pgn_write_concate(FILE* f, char* line, int len, const char* fmt, ...) {
    if (strlen(line) + len >= PGN_LINE_LEN) {
        fprintf(f, "%s\n", line);
        line[0] = '\0';
    }
    if (strlen(line)) {
        if (line[strlen(line) - 1] != ' ' && line[strlen(line) - 1] != '(' && fmt[0] != ')' && fmt[0] != '}') {
            concate(line, PGN_LINE_LEN, " ");
        }
    }

    va_list args;
    va_start(args, fmt);
    vsnprintf(line + strlen(line), PGN_LINE_LEN, fmt, args);
    va_end(args);
}

void
pgn_write_comment(FILE* f, char* line, const char* str) {
    char* word = NULL;
    char* saveptr;
    char comment[COMMENT_LEN];

    snprintf(comment, COMMENT_LEN, "%s", str);
    word = strtok_r(comment, " ", &saveptr);
    pgn_write_concate(f, line, strlen(word) + 1, "{%s", word);
    while ((word = strtok_r(NULL, " ", &saveptr)) != NULL) {
        pgn_write_concate(f, line, strlen(word) + 1, "%s", word);
    }
    pgn_write_concate(f, line, 2, "}");
}

void
pgn_write_variation(FILE* f, Variation* v, char* line, int i) {
    Move* m;
    int j, l;
    char num[MOVENUM_LEN];
    int is_main = (i == -1) ? 1 : 0;
    i = (i == -1) ? 0 : i;
    for (j = 1; j < v->move_count; j++) {
        m = &v->move_list[j];
        if (j == 1 && !is_main) {
            pgn_write_concate(f, line, 1, "(");
        }

        if (j == 1 && v->move_list[0].comment != NULL) {
            pgn_write_comment(f, line, v->move_list[0].comment);
        }

        variation_movenumber_export(v, j, i, num, MOVENUM_LEN);
        if (strlen(num)) {
            pgn_write_concate(f, line, strlen(num), num);
        }

        if (j > 1 && v->move_list[j - 2].variation_count > 0 && m->board.turn == White) {
            pgn_write_concate(f, line, 9, "%d...", (j / 2) + (i / 2));
        }

        pgn_write_concate(f, line, strlen(m->san) + 1, "%s", m->san);
        if (m->nag_move) {
            pgn_write_concate(f, line, 5, "$%d", m->nag_move);
        }
        if (m->nag_position) {
            pgn_write_concate(f, line, 8, "$%d", m->nag_position);
        }

        if (m->comment != NULL) {
            pgn_write_comment(f, line, m->comment);
        }

        for (l = 0; l < v->move_list[j - 1].variation_count; l++) {
            pgn_write_variation(f, v->move_list[j - 1].variation_list[l], line, i + j - 1);
        }

        if (j + 1 == v->move_count && !is_main) {
            pgn_write_concate(f, line, 2, ")");
        }
    }
}

void
pgn_write_file(FILE* f, Game* g) {
    int i;
    char line[PGN_LINE_LEN];
    char result[10];
    char escaped[TAG_LEN];
    line[0] = '\0';

    for (i = 0; i < g->tag_count; i++) {
        tag_escape_value(g->tag_list[i].value, escaped, TAG_LEN);
        fprintf(f, "[%s \"%s\"]\n", g->tag_list[i].key, escaped);
        if (!strcmp(g->tag_list[i].key, "Result")) {
            snprintf(result, 10, "%s", g->tag_list[i].value);
        }
    }

    fprintf(f, "\n");
    pgn_write_variation(f, g->line_main, line, -1);
    fprintf(f, "%s %s\n\n", line, result);
}

int
pgn_replace_game(const char* filename, Game* g, int index) {
    int i;
    long before_len, after_len, filesize;
    char *before_str, *after_str;

    //append mode does not fail if file does not exist
    FILE* f = fopen(filename, "a+");
    if (f == NULL) {
        return 0;
    }

    //get file size
    fseek(f, 0L, SEEK_END);
    filesize = ftell(f);
    rewind(f);

    //read games till given index
    for (i = 0; i < index; i++) {
        pgn_read_next(f, 1);
    }

    //get string before given index
    before_len = ftell(f);
    rewind(f);
    before_str = (char*)malloc(sizeof(char) * before_len);
    fread(before_str, sizeof(char), before_len, f);

    //skip game with given index
    pgn_read_next(f, 1);

    //get string after given index
    after_len = filesize - ftell(f);
    after_str = (char*)malloc(sizeof(char) * after_len);
    fread(after_str, sizeof(char), after_len, f);

    fclose(f);
    f = fopen(filename, "w");
    if (f == NULL) {
        return 0;
    }

    //write replaced file
    fwrite(before_str, sizeof(char), before_len, f);
    pgn_write_file(f, g);
    fwrite(after_str, sizeof(char), after_len, f);

    fclose(f);
    free(before_str);
    free(after_str);
    return 1;
}

int
pgn_count_games(FILE* f) {
    char buffer[BUFFER_LEN];
    int count = 0;
    int tags = 0;
    Tag tag;

    while (fgets(buffer, BUFFER_LEN, f)) {
        trimendl(buffer);
        if (tag_extract(buffer, &tag)) {
            tags++;
        } else {
            if (tags < 7) {
                return -1;
            }
            tags = 0;
            count++;
            pgn_read_next(f, 0);
        }
    }
    return count;
}

void
uci_line_parse(const char* str, int len, Board* b, int* depth, int* multipv, UciScoreType* type, int* score,
               Variation* v) {
    *type = NoType;
    char buffer[len];
    char san[SAN_LEN];
    char *tmp, *saveptr, *last;
    int moves = 0;
    int i = 1;
    Square src, dst;
    Piece prom_piece;
    Status status;
    Board tmp_b;

    if (v != NULL && b != NULL) {
        tmp_b = *b;
        move_init(&v->move_list[0]);
        v->move_list[0].src = none;
        v->move_list[0].dst = none;
        v->move_list[0].prom_piece = Empty;
        v->move_list[0].board = tmp_b;
        v->move_list[0].san[0] = '\0';
    }

    snprintf(buffer, len, "%s", str);
    last = strtok_r(buffer, " ", &saveptr);
    if (last == NULL) {
        return;
    }

    tmp = strtok_r(NULL, " ", &saveptr);
    while (tmp != NULL) {
        if (!strcmp(last, "depth")) {
            *depth = strtol(tmp, NULL, 10);
        }
        if (!strcmp(last, "multipv")) {
            *multipv = strtol(tmp, NULL, 10);
        }
        if (!strcmp(last, "cp")) {
            *type = Centipawn;
            *score = strtol(tmp, NULL, 10);
        }
        if (!strcmp(last, "mate")) {
            *type = Mate;
            *score = strtol(tmp, NULL, 10);
        }
        if (!strcmp(last, "pv") && *type != NoType) {
            moves = 1;
        }
        if (moves && v != NULL && b != NULL) {
            status = board_move_uci_status(&tmp_b, tmp, &src, &dst, &prom_piece);
            if (status != Invalid) {
                board_move_san_export(&tmp_b, src, dst, prom_piece, san, SAN_LEN, status);
                board_move_do(&tmp_b, src, dst, prom_piece, status);
                if (i >= v->move_count) {
                    variation_move_add(v, src, dst, prom_piece, &tmp_b, san);
                } else {
                    move_init(&v->move_list[i]);
                    v->move_list[i].src = src;
                    v->move_list[i].dst = dst;
                    v->move_list[i].prom_piece = prom_piece;
                    v->move_list[i].board = tmp_b;
                    snprintf(v->move_list[i].san, SAN_LEN, "%s", san);
                }
            }
            i++;
        }

        last = tmp;
        tmp = strtok_r(NULL, " ", &saveptr);
    }
    while (v != NULL && i < v->move_count) {
        v->move_list[i++].san[0] = '\0';
    }
}

void
game_list_init(GameList* gl) {
    gl->list = NULL;
    gl->count = 0;
}

void
game_list_free(GameList* gl) {
    if (gl->list == NULL || gl->count == 0) {
        return;
    }
    free(gl->list);
}

void
game_list_add(GameList* gl, GameRow* gr) {
    gl->count++;
    gl->list = (GameRow*)realloc(gl->list, sizeof(GameRow) * gl->count);
    gl->list[gl->count - 1] = *gr;
}

void
game_list_read_pgn(GameList* gl, FILE* f) {
    char buffer[BUFFER_LEN];
    int index = 0;
    Tag tag;
    GameRow gr;
    Game g;
    game_tag_init(&g);
    game_list_init(gl);

    while (fgets(buffer, BUFFER_LEN, f)) {
        trimendl(buffer);
        if (tag_extract(buffer, &tag)) {
            game_tag_set(&g, tag.key, tag.value);
        } else {
            snprintf(gr.title, GAMETITLE_LEN, "%s-%s/%s[%s]/%s (%s)", game_tag_get(&g, "White")->value,
                     game_tag_get(&g, "Black")->value, game_tag_get(&g, "Event")->value,
                     game_tag_get(&g, "Round")->value, game_tag_get(&g, "Date")->value,
                     game_tag_get(&g, "Result")->value);
            gr.index = index++;
#ifdef ADDITIONAL_TAG
            Tag* tmp_tag = game_tag_get(&g, ADDITIONAL_TAG);
            gr.tag_value[0] = '\0';
            if (tmp_tag != NULL) {
                snprintf(gr.tag_value, TAG_LEN, "%s", tmp_tag->value);
            }
#endif
            game_list_add(gl, &gr);
            pgn_read_next(f, 0);

            //reset tags
            game_tag_set(&g, "White", "");
            game_tag_set(&g, "Black", "");
            game_tag_set(&g, "Event", "");
            game_tag_set(&g, "Round", "");
            game_tag_set(&g, "Date", "");
            game_tag_set(&g, "Result", "*");
#ifdef ADDITIONAL_TAG
            game_tag_remove(&g, ADDITIONAL_TAG);
#endif
        }
    }
    game_tag_free(&g);
}

void
game_list_search_str(GameList* gl, GameList* new_gl, const char* str) {
    game_list_init(new_gl);
    int i;
    for (i = 0; i < gl->count; i++) {
        if (isubstr(gl->list[i].title, str)) {
            game_list_add(new_gl, &gl->list[i]);
        }
    }
}

int
game_list_cmp(const void* a, const void* b) {
    return (((GameRow*)a)->index > ((GameRow*)b)->index);
}

void
game_list_search_board(GameList* gl, GameList* new_gl, FILE* f, Board* b) {
    char buffer[BUFFER_LEN];
    char fen[FEN_LEN];
    char word[WORD_LEN];
    char san[SAN_LEN];
    char result[10];
    char* tmp;
    char* saveptr;
    int i, j, comment_start, comment_end, variation_start, variation_end, skip, skip_var;
    int tags;
    int comments;
    int anglebrackets; //pgn standard
    int nags;
    Tag tag;
    Status status;
    Square src, dst;
    Piece prom_piece;
    Board b_tmp, b_start;
    Variation *v, *new_v;
    Move* m;
    Game g;
    Square wp_start[] = {a2, b2, c2, d2, e2, f2, g2, h2};
    Square bp_start[] = {a7, b7, c7, d7, e7, f7, g7, h7};

    board_fen_import(&b_start, FEN_DEFAULT);
    game_list_init(new_gl);

    qsort(gl->list, gl->count, sizeof(GameRow), game_list_cmp);
    for (i = 0; i < gl->count; i++) {
        while (i < gl->list[i].index) {
            pgn_read_next(f, 1);
            i++;
        }
        snprintf(fen, FEN_LEN, "%s", FEN_DEFAULT);
        b_tmp = b_start;
        game_init(&g, &b_tmp);
        v = g.line_main;
        snprintf(result, 10, "*");
        skip = 0;
        skip_var = 0;
        tags = 1;
        comments = 0;
        anglebrackets = 0;
        nags = 0;
        while (fgets(buffer, BUFFER_LEN, f)) {
            trimendl(buffer);
            if (tags) { //parse tags
                if (tag_extract(buffer, &tag)) {
                    if (!strcmp(tag.key, "Result")) {
                        snprintf(result, 10, "%s", tag.value);
                    }
                    if (!strcmp(tag.key, "FEN")) {
                        snprintf(fen, FEN_LEN, "%s", tag.value);
                    }
                }
            } else { //parse moves
                tmp = strtok_r(buffer, " ", &saveptr);
                while (tmp != NULL && !(comments == 0 && !strcmp(tmp, result))) {

                    variation_start = 0;
                    variation_end = 0;
                    comment_start = charcount(tmp, '{');
                    comment_end = charcount(tmp, '}');

                    comments += comment_start;

                    if (comments == 0) {
                        variation_start = charcount(tmp, '(');
                        variation_end = charcount(tmp, ')');
                        anglebrackets += charcount(tmp, '<');
                        nags = charcount(tmp, '$');
                    }

                    if (comment_start) {
                        variation_start = charcount_before(tmp, '(', '{');
                    }

                    if (comment_end) {
                        variation_end = charcount_after(tmp, ')', '}');
                    }

                    if (variation_start && !skip) {
                        if (!skip_var) {
                            m = &v->move_list[v->move_current - 1];
                            b_tmp = m->board;
                            m->variation_count++;
                            m->variation_list = (Variation**)realloc(m->variation_list,
                                                                     sizeof(Variation*) * m->variation_count);
                            new_v = (Variation*)malloc(sizeof(Variation));
                            variation_init(new_v, &b_tmp);
                            m->variation_list[m->variation_count - 1] = new_v;
                            new_v->prev = v;
                            v = new_v;
                        } else {
                            skip_var++;
                        }
                    }

                    //parse SAN moves
                    if (comments == 0 && anglebrackets == 0 && charcount(tmp, '.') < 2 && nags == 0 && !skip
                        && !skip_var) {
                        snprintf(word, WORD_LEN, "%s", tmp);
                        trimmove(word);
                        if (str_is_move(word)) {
                            if (b_tmp.move_number > b->move_number + MOVENUM_OVERCHECK) {
                                if (v == g.line_main) {
                                    skip = 1;
                                } else {
                                    skip_var++;
                                }
                            }
                            status = board_move_san_status(&b_tmp, word, &src, &dst, &prom_piece);
                            if (status == Invalid) {
                                skip = 1;
                            }
                            board_move_san_export(&b_tmp, src, dst, prom_piece, san, SAN_LEN, status);
                            board_move_do(&b_tmp, src, dst, prom_piece, status);
                            if (board_is_equal(b, &b_tmp, 0)) {
                                game_list_add(new_gl, &gl->list[i]);
                                skip = 1;
                            }
                            for (j = 0; j < 8 && !skip; j++) {
                                if ((b->position[bp_start[j]] == BlackPawn
                                     && b->position[bp_start[j]] != b_tmp.position[bp_start[j]])
                                    || (b->position[wp_start[j]] == WhitePawn
                                        && b->position[wp_start[j]] != b_tmp.position[wp_start[j]])) {
                                    if (v == g.line_main) {
                                        skip = 1;
                                    } else {
                                        skip_var = 1;
                                    }
                                    break;
                                }
                            }
                            variation_move_add(v, src, dst, prom_piece, &b_tmp, san);
                        }
                    }

                    while (variation_end-- && !skip) {
                        if (--skip_var <= 0) {
                            skip_var = 0;
                            v->move_current = 1;
                            v = v->prev;
                            b_tmp = v->move_list[v->move_current].board;
                        }
                    }

                    comments -= comment_end;
                    if (comments == 0) {
                        if (comment_end > 0) {
                            anglebrackets -= charcount_after(tmp, '>', '}');
                        } else {
                            anglebrackets -= charcount(tmp, '>');
                        }
                    }
                    tmp = strtok_r(NULL, " ", &saveptr);
                }
            }
            //if empty line
            if (strlen(buffer) == 0) {
                if (tags) {
                    board_fen_import(&b_tmp, fen);
                    if (board_is_equal(b, &b_tmp, 0)) {
                        game_list_add(new_gl, &gl->list[i]);
                        skip = 1;
                    }
                    tags = 0;
                    word[0] = '\0';
                } else {
                    break;
                }
            }
        }
        game_free(&g);
    }
}

#endif // CHESS_UTILS_IMPLEMENTATION
