#include "chessboard.h"

int cb_rotation = RotationWhite;
Square cb_hidden = none;
int cb_drag = 0;
SDL_Texture *cb_piece_texture[12];

void
piece_load(WindowData *data)
{
    SDL_Surface *tmp;
    int i;
    char path[data->conf.path_max_len];
    char *piece_str = "pnbrqk";
    char *path_format = "%s/%c%c.png";
    for (i=0; i<12; i++) {
        snprintf(path, data->conf.path_max_len, path_format,
                data->conf.piece_path, i/6 ? 'w' : 'b', piece_str[i%6]);
        if(!file_exists(path)){
            data->conf.piece_path = "./resources/alpha/";
            snprintf(path, data->conf.path_max_len, path_format,
                data->conf.piece_path, i/6 ? 'w' : 'b', piece_str[i%6]);
        }
        tmp = IMG_Load(path);
        cb_piece_texture[i] = SDL_CreateTextureFromSurface(data->renderer,
                tmp);
        SDL_FreeSurface(tmp);
        if (cb_piece_texture[i] == NULL) {
            fprintf(stderr, "Loading piece image %s failed: %s\n", path,
                    SDL_GetError());
            exit(1);
        }
    }
}

void
piece_unload()
{
    int i;
    for (i=0; i<12; i++) {
        SDL_DestroyTexture(cb_piece_texture[i]);
    }
}

void
piece_draw(WindowData *data, int file, int rank, SDL_Texture *texture)
{
    data->layout.square.x = file;
    data->layout.square.y = rank;
    SDL_RenderCopy(data->renderer, texture, NULL, &data->layout.square);
}

void
rotation_toggle()
{
    cb_rotation = cb_rotation == RotationWhite
        ? RotationBlack : RotationWhite;
}

int
rotation_convert(int n) //rank or file
{
    return abs(n - cb_rotation);
}

void
position_draw(WindowData *data)
{
    int i;
    int rank = 0;
    int file = 0;
    for (i=0; i<128; i++) {
        if (data->board.position[i] > 0 && i != cb_hidden) {
            file = rotation_convert(square2file(i)) * data->layout.square.w;
            rank = rotation_convert(square2rank(i)) * data->layout.square.w;
            piece_draw(data, file, rank,
                    cb_piece_texture[data->board.position[i]-1]);
        }
    }
}

void
background_draw(WindowData *data)
{
    int col,row;
    data->layout.square.x = 0;
    data->layout.square.y = 0;

    SDL_SetRenderDrawColor(data->renderer, STATUS_BACKGROUND);
    SDL_RenderFillRect(data->renderer, &data->layout.board);
    for (col = 0; col < 8; col++) {
        data->layout.square.y = col * data->layout.square.w;
        for (row = 0; row < 8; row++) {
            if ((col + row) % 2) {
                SDL_SetRenderDrawColor(data->renderer, SQUARE_BLACK);
            }else {
                SDL_SetRenderDrawColor(data->renderer, SQUARE_WHITE);
            }
            data->layout.square.x = row * data->layout.square.w;
            SDL_RenderFillRect(data->renderer, &data->layout.square);
        }
    }
}

void
foreground_draw(WindowData *data)
{
    position_draw(data);
}

void
piece_mouse_position(WindowData *data, Piece piece)
{
    piece_draw(data, data->mouse.x - (data->layout.square.w / 2),
            data->mouse.y - (data->layout.square.w / 2),
            cb_piece_texture[piece-1]);
}

int
promotion_hover_position(WindowData *data, Square sq)
{
    int i, m_x, m_y;
    int x = rotation_convert(square2file(sq)) * data->layout.square.w;
    int y = rotation_convert(square2rank(sq)) * data->layout.square.w;
    int diff = (y == 0) ? data->layout.square.w : -data->layout.square.w;

    for (i=0; i<4; i++) {
        m_x = data->mouse.x / data->layout.square.w * data->layout.square.w;
        m_y = data->mouse.y / data->layout.square.w * data->layout.square.w;
        if (x == m_x && (y + i*diff) == m_y)
            return i;
    }
    return -1;
}

void
promotion_selection_draw(WindowData *data, Square sq, Color color)
{
    int i;
    int texture[] = {
        ((color == White) ? TextureWhiteQueen : TextureBlackQueen),
        ((color == White) ? TextureWhiteKnight : TextureBlackKnight),
        ((color == White) ? TextureWhiteRook : TextureBlackRook),
        (color == White) ? TextureWhiteBishop : TextureBlackBishop,
    };
    int x = rotation_convert(square2file(sq)) * data->layout.square.w;
    int y = rotation_convert(square2rank(sq)) * data->layout.square.w;
    int diff = (y == 0) ? data->layout.square.w : -data->layout.square.w;
    int hover = promotion_hover_position(data, sq);
    data->layout.square.x = x;

    for (i=0; i<4; i++) {
        data->layout.square.y = y + i*diff;
        if (hover == i)
            SDL_SetRenderDrawColor(data->renderer, SQUARE_ORANGE);
        else
            SDL_SetRenderDrawColor(data->renderer, SQUARE_GREY);
        SDL_RenderFillRect(data->renderer, &data->layout.square);
        piece_draw(data, data->layout.square.x, data->layout.square.y,
                cb_piece_texture[texture[i]]);
    }

    SDL_RenderPresent(data->renderer);
}

int
promotion_selected_piece(WindowData *data, Square sq, Color color)
{
    int piece[] = {
        ((color == White) ? WhiteQueen : BlackQueen),
        ((color == White) ? WhiteKnight: BlackKnight),
        ((color == White) ? WhiteRook : BlackRook),
        ((color == White) ? WhiteBishop : BlackBishop)
    };
    int selected = promotion_hover_position(data, sq);
    if (selected == -1)
        return -1;
    return piece[selected];
}

void
promotion_draw(WindowData *data, Square sq, Color color)
{
    data->draw(data);
    promotion_selection_draw(data, sq, color);
    SDL_RenderPresent(data->renderer);
}

int
mode_promotion(WindowData *data, Color color)
{
    int piece;
    int loop = 1;
    int dst = filerank2square(
            rotation_convert(data->mouse.x / data->layout.square.w),
            rotation_convert(data->mouse.y / data->layout.square.w));
    promotion_draw(data, dst, color);
    SDL_Event event;

    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 0);
            switch (event.type) {
            case SDL_WINDOWEVENT:
                promotion_draw(data, dst, color);
                break;

            case SDL_MOUSEMOTION:
                promotion_selection_draw(data, dst, color);
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    piece = promotion_selected_piece(data, dst, color);
                    if (piece != -1) {
                        loop = 0;
                        return piece;
                    }
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    cb_hidden = none;
                    break;
                }
            }
        }
    }
    return Empty;
}

void
editor_draw(WindowData *data, Piece piece)
{
    data->draw(data);
    piece_mouse_position(data, piece);
    SDL_RenderPresent(data->renderer);
}

void
mode_editor(WindowData *data)
{
    int loop = 1;
    char *clipboard = NULL;
    Color color = Black;
    Piece piece = BlackPawn;
    Square sq;
    int replace = 0;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.edit_status);
    data->draw_render(data);
    editor_draw(data, piece);
    while (loop) {
        handle_global_events(&event, data, &loop, 0);
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
            case SDL_WINDOWEVENT:
                editor_draw(data, piece);
                break;

            case SDL_MOUSEMOTION:
                editor_draw(data, piece);
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    sq = filerank2square(
                            rotation_convert(data->mouse.x/data->layout.square.w),
                            rotation_convert(data->mouse.y/data->layout.square.w));
                    if(!(sq & 0x88)){
                        board_square_set(&data->board, sq, piece);
                        replace = 1;
                        editor_draw(data, piece);
                    }
                }

                if (event.button.button == SDL_BUTTON_RIGHT) {
                    sq = filerank2square(
                            rotation_convert(data->mouse.x/data->layout.square.w),
                            rotation_convert(data->mouse.y/data->layout.square.w));
                    if(!(sq & 0x88)){
                        board_square_set(&data->board, sq, Empty);
                        replace = 1;
                        editor_draw(data, piece);
                    }
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    data->loop = 0;
                    loop = 0;
                    break;

                case SDLK_s:
                    board_fen_import(&data->board, FEN_DEFAULT);
                    editor_draw(data, piece);
                    replace = 1;
                    break;

                case SDLK_c:
                    board_clear(&data->board);
                    editor_draw(data, piece);
                    replace = 1;
                    break;

                case SDLK_f:
                    clipboard = SDL_GetClipboardText();
                    if(str_is_fen(clipboard)){
                        board_fen_import(&data->board, clipboard);
                        editor_draw(data, piece);
                        replace = 1;
                    }
                    break;

                case SDLK_ESCAPE:
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    loop = 0;
                    break;

                case SDLK_r:
                    rotation_toggle();
                    editor_draw(data, piece);
                    break;

                case SDLK_0:
                    color = !color;
                    piece = piece + (color ? 6 : -6);
                    editor_draw(data, piece);
                    break;

                case SDLK_1:
                    piece = 1 + color * 6;
                    editor_draw(data, piece);
                    break;

                case SDLK_2:
                    piece = 2 + color * 6;
                    editor_draw(data, piece);
                    break;

                case SDLK_3:
                    piece = 3 + color * 6;
                    editor_draw(data, piece);
                    break;

                case SDLK_4:
                    piece = 4 + color * 6;
                    editor_draw(data, piece);
                    break;

                case SDLK_5:
                    piece = 5 + color * 6;
                    editor_draw(data, piece);
                    break;

                case SDLK_6:
                    piece = 6 + color * 6;
                    editor_draw(data, piece);
                    break;
                }
                break;
            }
        }
    }
    if (replace) {
        machine_position(&data->board);
        notation_free(&data->notation);
        game_init(&data->notation, &data->board);
        snprintf(data->number, data->conf.number_len, "a");
        //notation_init(&data->notation, &data->board);
    }
    data->draw_render(data);
}

void
drag_draw(WindowData *data, int piece)
{
    data->draw(data);
    piece_mouse_position(data, piece);
    SDL_RenderPresent(data->renderer);
}

void
mode_san(WindowData *data)
{
    int loop = 1;
    int pos = 0;
    int old_pos;
    data->status.info[0] = '\0';
    Square src, dst;
    Piece prom_piece;
    Status status;
    SDL_Event event;
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.san_status);
    data->draw_render(data);
    while(loop){
        if(SDL_WaitEvent(&event)){
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, data->status.info,
                    data->conf.status_max_len);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    data->status.info[0] = '\0';
                    data->draw_render(data);
                    break;

                case SDLK_RETURN:
                    old_pos = pos;
                    cursor_remove(&pos, data->status.info);
                    status = board_move_san_status(&data->board,
                            data->status.info, &src, &dst, &prom_piece);
                    if(status != Invalid){
                        chessboard_move_do(data, src, dst, prom_piece, status);
                        machine_position(&data->board);
                        data->status.info[0] = '\0';
                        old_pos = 0;
                    }
                    pos = old_pos;
                    cursor_add(&pos, data->status.info,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;
                }
                break;

            case SDL_USEREVENT:
                data->draw_render(data);
                break;
            }
        }
    }
}

void
mode_training(WindowData *data)
{
    data->notation_hidden = 1;
    Variation *v = data->notation.line_current;
    int move_number = data->notation.line_current->move_current;
    int loop = 1;
    int pos = 0;
    int old_pos;
    data->status.info[0] = '\0';
    Square src, dst;
    Piece prom_piece, piece;
    Status status;
    SDL_Event event;
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.training_status);
    data->draw_render(data);
    while(loop){
        if(SDL_WaitEvent(&event)){
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, data->status.info,
                    data->conf.status_max_len);
            switch (event.type) {
            case SDL_MOUSEMOTION:
                if(cb_drag)
                    drag_draw(data, piece);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    cb_hidden = filerank2square(
                            rotation_convert(data->mouse.x /
                                data->layout.square.w), rotation_convert(
                                    data->mouse.y / data->layout.square.w));
                    piece = cb_hidden & 0x88 ? 0
                        : data->board.position[cb_hidden];
                    if (piece) {
                        if (!cb_drag)
                            cb_drag = 1;
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (cb_drag) {
                        dst = filerank2square(
                                rotation_convert(data->mouse.x /
                                    data->layout.square.w), rotation_convert(
                                        data->mouse.y / data->layout.square.w));

                        if(notation_move_is_present(&data->notation, cb_hidden,
                                    dst, Empty)){
                            chessboard_focus_present(data, cb_hidden, dst,
                                    Empty);
                            chessboard_focus_random(data);
                        }else{
                            cb_hidden = none;
                        }
                    }
                    cb_drag = 0;
                    piece = 0;
                    data->draw_render(data);
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    data->status.info[0] = '\0';
                    data->notation_hidden = 0;
                    data->draw_render(data);
                    break;

                case SDLK_RETURN:
                    old_pos = pos;
                    cursor_remove(&pos, data->status.info);
                    if(!strcmp(data->status.info, "Restart")){
                        data->notation.line_current = v;
                        notation_move_index_set(&data->notation, move_number);
                        data->status.info[0] = '\0';
                        pos = 0;
                        cb_hidden = none;
                        data->board = notation_move_get(
                                &data->notation)->board;
                        cursor_add(&pos, data->status.info,
                                data->conf.status_max_len, data);
                        data->draw_render(data);
                        break;
                    }
                    status = board_move_san_status(&data->board,
                            data->status.info, &src, &dst, &prom_piece);
                    if(status == Invalid)
                        break;

                    if(notation_move_is_present(&data->notation, src, dst,
                                prom_piece)){
                        chessboard_focus_present(data, src, dst,
                                prom_piece);
                        chessboard_focus_random(data);
                        data->status.info[0] = '\0';
                        old_pos = 0;
                    }
                    pos = old_pos;
                    cursor_add(&pos, data->status.info,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;
                }
                break;

            case SDL_USEREVENT:
                data->draw_render(data);
                break;
            }
        }
    }
}

void
chessboard_focus_present(WindowData *data, Square src, Square dst,
        Piece prom_piece)
{
    Move *m;
    int i;
    if(!notation_move_is_last(&data->notation)){
        m = &data->notation.line_current->move_list[
            data->notation.line_current->move_current+1];
        if(m->src == src && m->dst == dst && m->prom_piece == prom_piece){
            variation_move_next(data->notation.line_current);
        }
    }
    m = notation_move_get(&data->notation);
    for(i = 0; i < m->variation_count; i++){
        if(m->variation_list[i]->move_count < 2)
            continue;
        if(m->variation_list[i]->move_list[1].src == src
                && m->variation_list[i]->move_list[1].dst == dst
                && m->variation_list[i]->move_list[1].prom_piece ==
                prom_piece){
            data->notation.line_current = m->variation_list[i];
            data->notation.line_current->move_current = 1;
        }
    }
    data->board = notation_move_get(&data->notation)->board;
}

void
chessboard_focus_random(WindowData *data)
{
    Move *m = notation_move_get(&data->notation);
    int num = !notation_move_is_last(&data->notation) + m->variation_count;
    if(num == 0)
        return;

    int i = rand() % num;
    if(i == 0 && !notation_move_is_last(&data->notation)){
        variation_move_next(data->notation.line_current);
    }else{
        i -= !notation_move_is_last(&data->notation);
        data->notation.line_current = m->variation_list[i];
        data->notation.line_current->move_current = 1;
    }
    data->board = notation_move_get(&data->notation)->board;
}

void
chessboard_move_do(WindowData *data, Square src, Square dst,
        Piece prom_piece, Status status)
{
    char san[SAN_LEN];
    if(notation_move_is_present(&data->notation, src, dst, prom_piece)){
        chessboard_focus_present(data, src, dst, prom_piece);
        return;
    }

    board_move_san_export(&data->board, src, dst, prom_piece, san, SAN_LEN,
            status);
    board_move_do(&data->board, src, dst, prom_piece, status);
    if(notation_move_is_last(&data->notation)){
        variation_move_add(data->notation.line_current, src, dst, prom_piece,
                &data->board, san);
    }else{
        notation_variation_add(&data->notation, src, dst, prom_piece,
                &data->board, san);
    }
}

