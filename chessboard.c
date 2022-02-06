#include "chessboard.h"

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
rotation_toggle(WindowData *data)
{
    data->rotation = data->rotation == RotationWhite
        ? RotationBlack : RotationWhite;
}

int
rotation_convert(WindowData *data, int n) //rank or file
{
    return abs(n - (int)data->rotation);
}

Square
square_rotation(WindowData *data, Square sq)
{
    if(sq == none)
        return none;
    Rank rank = square2rank(sq);
    File file = square2file(sq);
    return filerank2square(rotation_convert(data, file),
            rotation_convert(data, rank));
}

void
position_draw(WindowData *data)
{
    int i;
    int rank = 0;
    int file = 0;
    Board b = game_move_get(&data->game)->board;
    for (i=0; i<128; i++) {
        if (b.position[i] > 0 && i != data->hidden){
            file = rotation_convert(data, square2file(i)) *
                data->layout.square.w;
            rank = rotation_convert(data, square2rank(i)) *
                data->layout.square.w;
            piece_draw(data, file, rank,
                    cb_piece_texture[b.position[i]-1]);
        }
    }
}

void
background_draw(WindowData *data)
{
    int col,row;
    SDL_Color c = data->conf.colors[ColorStatusBackground];
    data->layout.square.x = 0;
    data->layout.square.y = 0;

    Square src = square_rotation(data, game_move_get(&data->game)->src);
    Square dst = square_rotation(data, game_move_get(&data->game)->dst);
    SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(data->renderer, &data->layout.board);
    for (col = 0; col < 8; col++) {
        data->layout.square.y = col * data->layout.square.w;
        for (row = 0; row < 8; row++) {
            c = data->conf.colors[((col + row) % 2) ? ColorSquareBlack : ColorSquareWhite];
            if(filerank2square(row, col) == src
                    || filerank2square(row, col) == dst){
                c = data->conf.colors[((col + row) % 2) ? ColorSquareBlackLast : ColorSquareWhiteLast];
            }
            SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
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
piece_mouse_position(WindowData *data)
{
    piece_draw(data, data->mouse.x - (data->layout.square.w / 2),
            data->mouse.y - (data->layout.square.w / 2),
            cb_piece_texture[data->piece-1]);
}

int
promotion_hover_position(WindowData *data, Square sq)
{
    int i, m_x, m_y;
    int x = rotation_convert(data, square2file(sq)) * data->layout.square.w;
    int y = rotation_convert(data, square2rank(sq)) * data->layout.square.w;
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
    SDL_Color c;
    int texture[] = {
        ((color == White) ? TextureWhiteQueen : TextureBlackQueen),
        ((color == White) ? TextureWhiteKnight : TextureBlackKnight),
        ((color == White) ? TextureWhiteRook : TextureBlackRook),
        (color == White) ? TextureWhiteBishop : TextureBlackBishop,
    };
    int x = rotation_convert(data, square2file(sq)) * data->layout.square.w;
    int y = rotation_convert(data, square2rank(sq)) * data->layout.square.w;
    int diff = (y == 0) ? data->layout.square.w : -data->layout.square.w;
    int hover = promotion_hover_position(data, sq);
    data->layout.square.x = x;

    for (i=0; i<4; i++) {
        data->layout.square.y = y + i*diff;

        c = data->conf.colors[hover == i ? ColorSquareActive : ColorSquareInactive];
        SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
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
    draw(data);
    promotion_selection_draw(data, sq, color);
    SDL_RenderPresent(data->renderer);
}

int
mode_promotion(WindowData *data, Color color)
{
    data->piece = Empty;
    int piece;
    int loop = 1;
    int dst = filerank2square(
            rotation_convert(data, (data->mouse.x / data->layout.square.w)),
            rotation_convert(data, (data->mouse.y / data->layout.square.w)));
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
                    data->hidden = none;
                    break;
                }
                break;

            case SDL_USEREVENT:
                promotion_draw(data, dst, color);
                break;
            }
        }
    }
    return Empty;
}

void
mode_editor(WindowData *data)
{
    int loop = 1;
    char *clipboard = NULL;
    Color color = Black;
    data->piece = BlackPawn;
    Square sq;
    int replace = 0;
    SDL_Event event;
    Board b;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.edit_status);
    machine_stop(data, 0);
    machine_stop(data, 1);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_non_input_events(&event, data, &loop);
            switch (event.type) {
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    sq = filerank2square(
                            rotation_convert(data, (data->mouse.x /
                                    data->layout.square.w)),
                            rotation_convert(data, (data->mouse.y /
                                    data->layout.square.w)));
                    if(!(sq & 0x88)){
                        board_square_set(
                                &game_move_get(&data->game)->board, sq,
                                data->piece);
                        replace = 1;
                        draw_render(data);
                    }
                }

                if (event.button.button == SDL_BUTTON_RIGHT) {
                    sq = filerank2square(
                            rotation_convert(data, (data->mouse.x /
                                    data->layout.square.w)),
                            rotation_convert(data, (data->mouse.y /
                                    data->layout.square.w)));
                    if(!(sq & 0x88)){
                        board_square_set(
                                &game_move_get(&data->game)->board, sq,
                                Empty);
                        replace = 1;
                        draw_render(data);
                    }
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_s:
                    board_fen_import(&game_move_get(&data->game)->board,
                            FEN_DEFAULT);
                    draw_render(data);
                    replace = 1;
                    break;

                case SDLK_c:
                    board_clear(&game_move_get(&data->game)->board);
                    draw_render(data);
                    replace = 1;
                    break;

                case SDLK_f:
                    clipboard = SDL_GetClipboardText();
                    if(str_is_fen(clipboard)){
                        board_fen_import(&game_move_get(
                                    &data->game)->board, clipboard);
                        draw_render(data);
                        replace = 1;
                    }
                    break;

                case SDLK_ESCAPE:
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    loop = 0;
                    break;

                case SDLK_0:
                    color = !color;
                    data->piece = data->piece + (color ? 6 : -6);
                    draw_render(data);
                    break;

                case SDLK_1:
                    data->piece = 1 + color * 6;
                    draw_render(data);
                    break;

                case SDLK_2:
                    data->piece = 2 + color * 6;
                    draw_render(data);
                    break;

                case SDLK_3:
                    data->piece = 3 + color * 6;
                    draw_render(data);
                    break;

                case SDLK_4:
                    data->piece = 4 + color * 6;
                    draw_render(data);
                    break;

                case SDLK_5:
                    data->piece = 5 + color * 6;
                    draw_render(data);
                    break;

                case SDLK_6:
                    data->piece = 6 + color * 6;
                    draw_render(data);
                    break;
                }
                break;
            }
        }
    }
    if (replace) {
        b = game_move_get(&data->game)->board;
        game_free(&data->game);
        game_init_default(&data->game, &b);
        handle_position_change(data);
        snprintf(data->number, data->conf.number_len, "a");
    }
    data->piece = Empty;
    draw_render(data);
}

void
mode_san(WindowData *data)
{
    int loop = 1;
    int pos = 0;
    int old_pos;
    Square src, dst;
    Piece prom_piece;
    Status status;
    SDL_Event event;
    data->message = 0;
    data->status.info[0] = '\0';
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.san_status);
    draw_render(data);
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
                    draw_render(data);
                    break;

                case SDLK_RETURN:
                    old_pos = pos;
                    cursor_remove(&pos, data->status.info);
                    status = game_move_san_status(&data->game,
                            data->status.info, &src, &dst, &prom_piece);
                    if(status != Invalid){
                        chessboard_move_do(data, src, dst, prom_piece, status);
                        data->status.info[0] = '\0';
                        old_pos = 0;
                    }
                    pos = old_pos;
                    cursor_add(&pos, data->status.info,
                            data->conf.status_max_len, data);
                    draw_render(data);
                    break;
                }
                break;

            case SDL_USEREVENT:
                draw_render(data);
                break;
            }
        }
    }
}

void
mode_training(WindowData *data)
{
    Square src, dst;
    Piece prom_piece;
    Status status;
    SDL_Event event;
    VariationSequence vs_tmp;

    data->notation_hidden = 1;
    Variation *v = data->game.line_current;
    Color color = game_move_get(&data->game)->board.turn;
    vs_generate_first(&data->vs, v, color);
    int move_number = data->game.line_current->move_current;
    int loop = 1;
    int pos = 0;
    int old_pos;
    int vs_index = 0;
    int gl_index = 0;
    int not_move = 0;
    data->message = 0;
    data->status.info[0] = '\0';
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.training_status);
    draw_render(data);
    while(loop){
        if(SDL_WaitEvent(&event)){
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, data->status.info,
                    data->conf.status_max_len);
            switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    data->hidden = chessboard_mouse_square(data);
                    data->piece = data->hidden & 0x88 ? Empty
                        : game_move_get(
                                &data->game)->board.position[data->hidden];
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if(data->piece != Empty){
                        dst = chessboard_mouse_square(data);
                        if(game_move_is_present(&data->game,
                                    data->hidden, dst, Empty)){
                            chessboard_focus_present(data, data->hidden, dst,
                                    Empty);
                            chessboard_vs_next(data, &vs_index);
                        }
                    }
                    data->hidden = none;
                    data->piece = Empty;
                    draw_render(data);
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    data->notation_mode = ModeMoves;
                    data->status.info[0] = '\0';
                    data->notation_hidden = 0;
                    draw_render(data);
                    break;

                case SDLK_RETURN:
                    data->notation_mode = ModeMoves;
                    old_pos = pos;
                    cursor_remove(&pos, data->status.info);
                    if(!strcmp(data->status.info, "Restart")){
                        not_move = 1;
                        vs_free(&data->vs);
                        vs_init(&data->vs);
                        vs_index = 0;
                        gl_index = 0;
                        data->game.line_current = v;
                        game_move_index_set(&data->game, move_number);
                        if(data->from_game_list){
                            game_list_game_load(data, 0);
                            v = data->game.line_current;
                            move_number = data->game.line_current->move_current;
                        }
                        vs_generate_first(&data->vs, v, color);
                    }

                    if(!strcmp(data->status.info, "Repeat")){
                        not_move = 1;
                        data->game.line_current = v;
                        game_move_index_set(&data->game, move_number);
                        vs_index = 0;
                    }

                    if(!strcmp(data->status.info, "Next")){
                        not_move = 1;
                        data->game.line_current = v;
                        game_move_index_set(&data->game, move_number);
                        vs_print(&data->vs);
                        if(vs_can_generate_next(&data->vs)){
                            vs_index = 0;
                            vs_tmp = data->vs;
                            vs_init(&data->vs);
                            vs_generate_next(&data->vs, v, &vs_tmp, color);
                            vs_free(&vs_tmp);
                        }else if(data->from_game_list && gl_index + 1 < data->game_list.count){
                            vs_free(&data->vs);
                            vs_init(&data->vs);
                            gl_index++;
                            game_list_game_load(data, gl_index);
                            v = data->game.line_current;
                            vs_generate_first(&data->vs, v, color);
                            move_number = data->game.line_current->move_current;
                        }else{
                            data->notation_mode = ModeCustomText;
                            snprintf(data->custom_text, data->conf.status_max_len, "All done.");
                        }
                    }

                    if(not_move){
                        not_move = 0;
                        data->status.info[0] = '\0';
                        pos = 0;
                        data->hidden = none;
                        cursor_add(&pos, data->status.info,
                                data->conf.status_max_len, data);
                        draw_render(data);
                        break;
                    }

                    status = game_move_san_status(&data->game,
                            data->status.info, &src, &dst, &prom_piece);
                    if(status == Invalid)
                        break;

                    if(game_move_is_present(&data->game, src, dst,
                                prom_piece)){
                        chessboard_focus_present(data, src, dst,
                                prom_piece);
                        chessboard_vs_next(data, &vs_index);
                        data->status.info[0] = '\0';
                        old_pos = 0;
                    }
                    pos = old_pos;
                    cursor_add(&pos, data->status.info,
                            data->conf.status_max_len, data);
                    draw_render(data);
                    break;
                }
                break;

            case SDL_USEREVENT:
                draw_render(data);
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
    if(!game_move_is_last(&data->game)){
        m = &data->game.line_current->move_list[
            data->game.line_current->move_current+1];
        if(m->src == src && m->dst == dst && m->prom_piece == prom_piece){
            variation_move_next(data->game.line_current);
        }
    }
    m = game_move_get(&data->game);
    for(i = 0; i < m->variation_count; i++){
        if(m->variation_list[i]->move_count < 2)
            continue;
        if(m->variation_list[i]->move_list[1].src == src
                && m->variation_list[i]->move_list[1].dst == dst
                && m->variation_list[i]->move_list[1].prom_piece ==
                prom_piece){
            data->game.line_current = m->variation_list[i];
            data->game.line_current->move_current = 1;
        }
    }
}

void
chessboard_vs_next(WindowData *data, int *vs_index)
{
    Move *m = game_move_get(&data->game);
    int num = !game_move_is_last(&data->game) + m->variation_count;
    if(num == 0)
        return;

    int i =  m->variation_count ? data->vs.list[*vs_index].index : 0;
    if(i == 0 && !game_move_is_last(&data->game)){
        variation_move_next(data->game.line_current);
    }else{
        i -= !game_move_is_last(&data->game);
        data->game.line_current = m->variation_list[i];
        data->game.line_current->move_current = 1;
    }
    if(m->variation_count){
        (*vs_index)++;
    }
}

void
chessboard_move_do(WindowData *data, Square src, Square dst,
        Piece prom_piece, Status status)
{
    if(game_move_is_present(&data->game, src, dst, prom_piece)){
        chessboard_focus_present(data, src, dst, prom_piece);
        handle_position_change(data);
        return;
    }

    if(game_move_is_last(&data->game)){
        game_move_add(&data->game, src, dst, prom_piece, status);
    }else{
        game_variation_add(&data->game, src, dst, prom_piece, status);
    }
    handle_position_change(data);
}

Square
chessboard_mouse_square(WindowData *data)
{
    if(!chessboard_mouse_is_inside(data))
        return none;

    Square sq = filerank2square(
            rotation_convert(data, (data->mouse.x / data->layout.square.w)),
            rotation_convert(data, (data->mouse.y / data->layout.square.w)));
    return (sq & 0x88) == 0 ? sq : none;
}

int chessboard_mouse_is_inside(WindowData *data)
{
    return (SDL_PointInRect(&data->mouse, &data->layout.board) == SDL_TRUE);
}
