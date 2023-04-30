#include <time.h>
#include "notation.h"

MoveCoord *nt_move_coords;
int nt_move_coord_len;
int nt_move_coord_index;

void
notation_handle_line_break(WindowData *data, int *x, int *y, int word_width,
        int x_start)
{
    if (*x + word_width >= data->layout.notation.x + data->layout.notation.w){
        *x = x_start;
        *y += data->font_height;
    }
}

void
comment_draw(WindowData *data, Move *m, int *x, int *y, int x_start)
{
    char comment[COMMENT_LEN];
    char *word = NULL;
    char *saveptr;
    int word_width;

    snprintf(comment, COMMENT_LEN, "%s", m->comment);
    word = strtok_r(comment, " ", &saveptr);

    while(word != NULL){
        word_width = FC_GetWidth(data->font, word) + NOTATION_PADDING_MOVE;
        notation_handle_line_break(data, x, y, word_width, x_start);
        FC_DrawColor(data->font, data->renderer, *x, *y,
                data->conf.colors[ColorCommentFont], word);
        word = strtok_r(NULL, " ", &saveptr);
        *x += word_width;
    }
}

void
variation_draw(WindowData *data, Variation *v,  int *x, int *y,
        int x_start, int i, int recursive)
{
    SDL_Rect move_current;
    SDL_Color c = data->conf.colors[ColorNotationActiveBackground];
    Move *m;
    char san[SAN_LEN+MOVENUM_LEN];
    int movelen = SAN_LEN+MOVENUM_LEN + 2 * NAG_LEN;
    int word_width;
    nt_move_coord_len += v->move_count - 1;
    nt_move_coords = realloc(nt_move_coords, sizeof(MoveCoord)*nt_move_coord_len);
    if(nt_move_coords == NULL)
        return;

    for(int j=1; j<v->move_count; j++){
        if(j==1 && i){
            snprintf(san, SAN_LEN+MOVENUM_LEN, "(");
            word_width = FC_GetWidth(data->font, san);
            if(v->move_count > 10){
                *y += data->font_height;
                *x = x_start;
            }
            notation_handle_line_break(data, x, y, word_width, x_start);
            FC_DrawColor(data->font, data->renderer, *x, *y,
                    data->conf.colors[ColorVariationFont], san);
            *x += word_width;
        }

        m = &v->move_list[j];
        if(v->move_list[0].comment != NULL && j==1)
            comment_draw(data, &v->move_list[0], x, y, x_start);

        variation_movenumber_export(v, j, i, san, movelen);
        if(j > 1 && v->move_list[j-2].variation_count > 0
                && m->board.turn == White)
            snprintf(san, movelen, "%d...", (j / 2) + (i / 2 ));
        concate(san, movelen, "%s%s%s", m->san, nag_map[m->nag_move],
            nag_map[m->nag_position]);

        word_width = FC_GetWidth(data->font, san) + NOTATION_PADDING_MOVE;
        notation_handle_line_break(data, x, y, word_width, x_start);
        nt_move_coords[nt_move_coord_index].x = *x;
        nt_move_coords[nt_move_coord_index].y = *y;
        nt_move_coords[nt_move_coord_index].w = word_width;
        nt_move_coords[nt_move_coord_index].h = data->font_height;
        nt_move_coords[nt_move_coord_index].index = j;
        nt_move_coords[nt_move_coord_index].move = m;
        nt_move_coords[nt_move_coord_index].variation = v;
        nt_move_coord_index++;

        if (j == data->game.line_current->move_current
                && data->game.line_current == v){
            move_current.x = *x - NOTATION_CURRENT_MOVE_PADDING_LEFT;
            move_current.y = *y;
            move_current.w = word_width;
            move_current.h = data->font_height;
            SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(data->renderer, &move_current);
            FC_DrawColor(data->font, data->renderer, *x, *y,
                    data->conf.colors[ColorNotationActiveFont], san);
        }else
            FC_DrawColor(data->font, data->renderer, *x, *y, i
                    ? data->conf.colors[ColorVariationFont]
                    : data->conf.colors[ColorNotationFont], san);
        *x += word_width;

        if(m->comment != NULL)
            comment_draw(data, m, x, y, x_start);

        for(int l=0; l < v->move_list[j-1].variation_count && recursive; l++){
            variation_draw(data, v->move_list[j-1].variation_list[l], x, y,
                    x_start, i+j-1, 1);
        }


        if(j+1 == v->move_count && i){
            snprintf(san, SAN_LEN+MOVENUM_LEN, ")");
            word_width = FC_GetWidth(data->font, san) + NOTATION_PADDING_MOVE;
            notation_handle_line_break(data, x, y, word_width, x_start);
            FC_DrawColor(data->font, data->renderer, *x, *y,
                    data->conf.colors[ColorVariationFont], san);
            *x += word_width;
        }
    }
}

int
notation_click(WindowData *data)
{
    return data->mouse.x > data->layout.notation.x
        && data->mouse.x < data->layout.notation.w + data->layout.notation.x
        && data->mouse.y > data->layout.notation.y
        && data->mouse.y < data->layout.notation.h + data->layout.notation.y;
}

int
notation_coord_index_click(WindowData *data)
{
    for(int i=0; i<nt_move_coord_len; i++){
        if(nt_move_coords[i].x <= data->mouse.x
                && nt_move_coords[i].x+nt_move_coords[i].w >= data->mouse.x
                && nt_move_coords[i].y <= data->mouse.y
                && nt_move_coords[i].y+nt_move_coords[i].h >= data->mouse.y)
            return i;
    }
    return -1;
}

int
notation_coord_index_move(WindowData *data, Move *m)
{
    (void)data; //TODO move nt_move_coord data in WindowData
    for(int i=0; i<nt_move_coord_len; i++){
        if(nt_move_coords[i].move == m)
            return i;
    }
    return -1;
}

void
notation_draw_tags(WindowData *data, int *x, int *y, int x_start)
{
    int word_width;
    char word[TAG_LEN*2];
    int i;
    for(i=0; i < data->game.tag_count; i++){
        snprintf(word, TAG_LEN*2, "[%s \"%s\"]",
                data->game.tag_list[i].key,
                data->game.tag_list[i].value);
        word_width = FC_GetWidth(data->font, word) + NOTATION_PADDING_TITLE;
        notation_handle_line_break(data, x, y, word_width, x_start);
        FC_DrawColor(data->font, data->renderer, *x, *y,
                data->conf.colors[ColorNotationFont], word);
        *x += word_width;
    }
    *y += data->font_height * 2;
    *x = x_start;
}

void
notation_draw(WindowData *data)
{
    SDL_Color c = data->conf.colors[ColorNotationBackground];
    SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(data->renderer, &data->layout.notation);
    nt_move_coord_index = 0;
    nt_move_coord_len = 0;
    int x_start = data->layout.notation.x + NOTATION_PADDING_LEFT;
    int x = x_start;
    int y = data->layout.notation.y + NOTATION_PADDING_TOP
        + data->notation_scroll;

    if(data->notation_hidden){
        if(game_move_is_last(&data->game)){
            variation_draw(data, data->game.line_current, &x, &y, x_start,
                    0, 0);
        }
        return;
    }
    notation_draw_tags(data, &x, &y, x_start);
    variation_draw(data, data->game.line_main, &x, &y, x_start, 0, 1);
}

void
notation_scroll_up(WindowData *data)
{
    data->notation_scroll += data->conf.scroll_step;
    if(data->notation_scroll > 0){
        data->notation_scroll = 0;
    }
}

void
notation_scroll_down(WindowData *data)
{
    data->notation_scroll -= data->conf.scroll_step;
}

void
mode_annotate(WindowData *data, Move *move)
{
    int loop = 1;
    SDL_Event event;
    int pos;
    if(move->comment == NULL){
        move->comment = calloc(COMMENT_LEN, sizeof(char));
        pos = 0;
    }else{
        pos = U8_strlen(move->comment);
    }
    cursor_add(&pos, move->comment, COMMENT_LEN, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.annotate_status);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, move->comment,
                    COMMENT_LEN);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    if(U8_strlen(move->comment) == 0){
                        free(move->comment);
                        move->comment = NULL;
                    }
                    draw_render(data);
                    break;
                }
                break;
            }
        }
    }
}

void
mode_move(WindowData *data)
{
    int loop = 1;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.move_annotation_status);
    draw_render(data);
    while(loop){
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_non_input_events(&event, data, &loop);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    break;

                case SDLK_j:
                case SDLK_DOWN:
                    nag_move_next(&data->game);
                    draw_render(data);
                    break;

                case SDLK_k:
                case SDLK_UP:
                    nag_move_prev(&data->game);
                    draw_render(data);
                    break;

                case SDLK_x:
                case SDLK_BACKSPACE:
                    game_move_get(&data->game)->nag_move = 0;
                    loop = 0;
                    break;
                }
            }
        }
    }
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.normal_status);
    draw_render(data);
}

void
mode_position(WindowData *data)
{
    int loop = 1;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.position_annotation_status);
    draw_render(data);
    while(loop){
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_non_input_events(&event, data, &loop);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    break;

                case SDLK_j:
                case SDLK_DOWN:
                    nag_position_next(&data->game);
                    draw_render(data);
                    break;

                case SDLK_k:
                case SDLK_UP:
                    nag_position_prev(&data->game);
                    draw_render(data);
                    break;

                case SDLK_x:
                case SDLK_BACKSPACE:
                    game_move_get(&data->game)->nag_position = 0;
                    loop = 0;
                    break;
                }
            }
        }
    }
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.normal_status);
    draw_render(data);
}

void
mode_tag_edit(WindowData *data, Tag *tag)
{
    int loop = 1;
    SDL_Event event;
    int pos = U8_strlen(tag->value);
    data->notation_scroll = 0;
    cursor_add(&pos, tag->value, TAG_LEN, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            handle_input_events(&event, data, &loop, &pos, tag->value,
                    TAG_LEN);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    if(U8_strlen(tag->value) == 0 && strcmp(tag->key, "Event")
                            && strcmp(tag->key, "Site")
                            && strcmp(tag->key, "Date")
                            && strcmp(tag->key, "Round")
                            && strcmp(tag->key, "White")
                            && strcmp(tag->key, "Black")
                            && strcmp(tag->key, "Result"))
                        game_tag_remove(&data->game, tag->key);
                    data->status.info[0] = '\0';
                    draw_render(data);
                    break;
                }
                break;
            }
        }
    }
}

void
mode_tag(WindowData *data)
{
    int loop = 1;
    int pos = 0;
    SDL_Event event;
    Tag *tag;
    data->message = 0;
    data->status.info[0] = '\0';
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.tag_status);
    draw_render(data);
    while(loop){
        if (SDL_WaitEvent(&event)) {
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
                    cursor_remove(&pos, data->status.info);
                    tag = game_tag_get(&data->game, data->status.info);
                    if(tag == NULL){
                        game_tag_set(&data->game, data->status.info,
                                "");
                        tag = game_tag_get(&data->game,
                                data->status.info);
                    }
                    mode_tag_edit(data, tag);
                    loop = 0;
                    data->status.info[0] = '\0';
                    break;
                }
                break;
            }
        }
    }
}

void
mode_clipboard(WindowData *data)
{
    int loop = 1;
    char fen[FEN_LEN];
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.clipboard_status);
    draw_render(data);
    while(loop){
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    loop = 0;
                    data->loop = 0;
                    break;

                case SDLK_f:
                    board_fen_export(&game_move_get(
                                &data->game)->board, fen);
                    SDL_SetClipboardText(fen);
                    message_add(data, &event, "FEN copied to clipboard");
                    break;

                case SDLK_ESCAPE:
                default:
                    data->status.info[0] = '\0';
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    loop = 0;
                    draw_render(data);
                    break;

                }
                break;
            }
        }
    }
}

void
game_init_default(Game *g, Board *b)
{
    char fen[FEN_LEN];
    char date[20];
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date, 20, "%Y.%m.%d", timeinfo);
    board_fen_export(b, fen);
    game_init(g, b);
    if(strcmp(fen, FEN_DEFAULT))
        game_tag_set(g, "FEN", fen);
    game_tag_set(g, "Date", date);
}

void
undo_init(Game *list[])
{
    memset(list, 0, sizeof(Game*)*UNDO_COUNT);
}

void
undo_add(WindowData *data)
{
    int i = (data->undo_current + 1) % UNDO_COUNT;
    if(data->undo_list[i] != NULL
            && data->undo_list[i]->line_main != data->game.line_main){
        game_free(data->undo_list[i]);
        free(data->undo_list[i]);
    }
    data->undo_list[i] = game_clone(&data->game);
    data->undo_current = i;
}

void
undo_do(WindowData *data)
{
    int i = data->undo_current;
    if(i < 0)
        return;
    if(data->undo_list[i] != NULL){
        redo_add(data);
        game_free(&data->game);
        data->game = *data->undo_list[i];
        free(data->undo_list[i]);
        data->undo_list[i] = NULL;
        i--;
        if(i < 0)
            i = UNDO_COUNT - 1;
        data->undo_current = i;
    }
}

void
redo_add(WindowData *data)
{
    int i = (data->redo_current + 1) % UNDO_COUNT;
    if(data->redo_list[i] != NULL
            && data->redo_list[i]->line_main != data->game.line_main){
        game_free(data->redo_list[i]);
        free(data->redo_list[i]);
    }
    data->redo_list[i] = game_clone(&data->game);
    data->redo_current = i;
}

void
redo_do(WindowData *data)
{
    int i = data->redo_current;
    if(i < 0)
        return;
    if(data->redo_list[i] != NULL){
        game_free(&data->game);
        data->game = *data->redo_list[i];
        free(data->redo_list[i]);
        data->redo_list[i] = NULL;
        i--;
        if(i < 0)
            i = UNDO_COUNT - 1;
        data->redo_current = i;
    }
}

void
undo_free(Game *list[])
{
    int i;
    for(i = 0; i < UNDO_COUNT; i++){
        if(list[i] != NULL){
            game_free(list[i]);
            free(list[i]);
        }
    }
}

void
notation_focus_current_move(WindowData *data)
{
    int index = notation_coord_index_move(data,
            game_move_get(&data->game));
    if(index < 0){
        data->notation_scroll = 0;
        return;
    }
    int bot = data->layout.notation.y + data->layout.notation.h;
    if(nt_move_coords[index].y > bot){
        data->notation_scroll -= nt_move_coords[index].y - bot;
        data->notation_scroll -= data->font_height;
    }else if(nt_move_coords[index].y < 0){
        data->notation_scroll -= nt_move_coords[index].y;
    }
}

void
custom_text_draw(WindowData *data)
{
    SDL_Color c = data->conf.colors[ColorNotationBackground];
    SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(data->renderer, &data->layout.notation);
    int x = data->layout.notation.x + NOTATION_PADDING_LEFT;
    int y = data->layout.notation.y + NOTATION_PADDING_TOP
        + data->game_list_scroll;
    FC_DrawColor(data->font, data->renderer, x, y,
                data->conf.colors[ColorNotationFont], data->custom_text);
}
