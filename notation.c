#include <time.h>
#include "notation.h"

MoveCoord *nt_move_coords;
int nt_move_coord_len;
int nt_move_coord_index;

void
notation_handle_line_break(WindowData *data, int *x, int *y, int word_width,
        int word_height, int x_start)
{
    if (*x + word_width >= data->layout.notation.x + data->layout.notation.w){
        *x = x_start;
        *y += word_height;
    }
}

void
comment_draw(WindowData *data, Move *m, int *x, int *y, int x_start)
{
    char comment[COMMENT_LEN];
    char *word = NULL;
    char *saveptr;
    int word_width, word_height;

    snprintf(comment, COMMENT_LEN, "%s", m->comment);
    word = strtok_r(comment, " ", &saveptr);

    word_height = FC_GetHeight(data->font, comment);
    while(word != NULL){
        word_width = FC_GetWidth(data->font, word) + NOTATION_PADDING_MOVE;
        notation_handle_line_break(data, x, y, word_width, word_height,
                x_start);
        FC_DrawColor(data->font, data->renderer, *x, *y,
                data->conf.comment_font_color, word);
        word = strtok_r(NULL, " ", &saveptr);
        *x += word_width;
    }
}

void
variation_draw(WindowData *data, Variation *v,  int *x, int *y,
        int x_start, int i)
{
    SDL_Rect move_current;
    Move *m;
    char san[SAN_LEN+MOVENUM_LEN];
    int movelen = SAN_LEN+MOVENUM_LEN + 2 * NAG_LEN;
    int word_width, word_height;
    nt_move_coord_len += v->move_count - 1;
    nt_move_coords = realloc(nt_move_coords, sizeof(MoveCoord)*nt_move_coord_len);
    if(nt_move_coords == NULL)
        return;

    if(i){
        word_height = FC_GetHeight(data->font, "Test");
    }
    for(int j=1; j<v->move_count; j++){
        if(j==1 && i){
            snprintf(san, SAN_LEN+MOVENUM_LEN, "(");
            word_height = FC_GetHeight(data->font, san);
            word_width = FC_GetWidth(data->font, san);
            if(v->move_count > 10){
                *y += word_height;
                *x = x_start;
            }
            notation_handle_line_break(data, x, y, word_width, word_height,
                    x_start);
            FC_DrawColor(data->font, data->renderer, *x, *y,
                    data->conf.variation_font_color, san);
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

        word_height = FC_GetHeight(data->font, san);
        word_width = FC_GetWidth(data->font, san) + NOTATION_PADDING_MOVE;
        notation_handle_line_break(data, x, y, word_width, word_height,
                x_start);
        nt_move_coords[nt_move_coord_index].x = *x;
        nt_move_coords[nt_move_coord_index].y = *y;
        nt_move_coords[nt_move_coord_index].w = word_width;
        nt_move_coords[nt_move_coord_index].h = word_height;
        nt_move_coords[nt_move_coord_index].index = j;
        nt_move_coords[nt_move_coord_index].move = m;
        nt_move_coords[nt_move_coord_index].variation = v;
        nt_move_coord_index++;

        if (j == data->notation.line_current->move_current
                && data->notation.line_current == v){
            move_current.x = *x - NOTATION_CURRENT_MOVE_PADDING_LEFT;
            move_current.y = *y;
            move_current.w = word_width;
            move_current.h = word_height;
            SDL_SetRenderDrawColor(data->renderer,
                    NOTATION_CURRENT_MOVE_BACKGROUND);
            SDL_RenderFillRect(data->renderer, &move_current);
            FC_DrawColor(data->font, data->renderer, *x, *y,
                    data->conf.notation_current_fg, san);
        }else
            FC_DrawColor(data->font, data->renderer, *x, *y, i
                    ? data->conf.variation_font_color
                    : data->conf.notation_font_color, san);
        *x += word_width;

        if(m->comment != NULL)
            comment_draw(data, m, x, y, x_start);

        for(int l=0; l < v->move_list[j-1].variation_count; l++){
            variation_draw(data, v->move_list[j-1].variation_list[l], x, y,
                    x_start, i+j-1);
        }


        if(j+1 == v->move_count && i){
            snprintf(san, SAN_LEN+MOVENUM_LEN, ")");
            word_height = FC_GetHeight(data->font, san);
            word_width = FC_GetWidth(data->font, san) + NOTATION_PADDING_MOVE;
            notation_handle_line_break(data, x, y, word_width, word_height,
                    x_start);
            FC_DrawColor(data->font, data->renderer, *x, *y,
                    data->conf.variation_font_color, san);
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
notation_move_find(WindowData *data)
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

void
notation_draw_tags(WindowData *data, int *x, int *y, int x_start)
{
    int word_width, word_height;
    word_height = 0;
    char word[TAG_LEN*2];
    int i;
    for(i=0; i < data->notation.tag_count; i++){
        if(word_height == 0)
            word_height = FC_GetHeight(data->font,
                    data->notation.tag_list[i].key);
        snprintf(word, TAG_LEN*2, "[%s \"%s\"]",
                data->notation.tag_list[i].key,
                data->notation.tag_list[i].value);
        word_width = FC_GetWidth(data->font, word) + NOTATION_PADDING_TITLE;
        notation_handle_line_break(data, x, y, word_width, word_height,
                x_start);
        FC_DrawColor(data->font, data->renderer, *x, *y,
                data->conf.notation_font_color, word);
        *x += word_width;
    }
    *y += word_height*2;
    *x = x_start;
}

void
notation_draw(WindowData *data)
{
    SDL_SetRenderDrawColor(data->renderer, NOTATION_BACKGROUND);
    SDL_RenderFillRect(data->renderer, &data->layout.notation);
    if(data->notation_hidden)
        return;
    nt_move_coord_index = 0;
    nt_move_coord_len = 0;
    int x_start = data->layout.notation.x + NOTATION_PADDING_LEFT;
    int x = x_start;
    int y = data->layout.notation.y + NOTATION_PADDING_TOP
        + data->notation_scroll;

    notation_draw_tags(data, &x, &y, x_start);
    variation_draw(data, data->notation.line_main, &x, &y, x_start, 0);
}

void
notation_scroll_up(WindowData *data)
{
    data->notation_scroll += data->conf.scroll_step;
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
    data->draw_render(data);
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
                    data->draw_render(data);
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
    data->draw_render(data);
    while(loop){
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    break;

                case SDLK_j:
                case SDLK_DOWN:
                    nag_move_next(&data->notation);
                    data->draw_render(data);
                    break;

                case SDLK_k:
                case SDLK_UP:
                    nag_move_prev(&data->notation);
                    data->draw_render(data);
                    break;

                case SDLK_x:
                case SDLK_BACKSPACE:
                    notation_move_get(&data->notation)->nag_move = 0;
                    loop = 0;
                    break;
                }
            }
        }
    }
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.normal_status);
    data->draw_render(data);
}

void
mode_position(WindowData *data)
{
    int loop = 1;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.position_annotation_status);
    data->draw_render(data);
    while(loop){
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    break;

                case SDLK_j:
                case SDLK_DOWN:
                    nag_position_next(&data->notation);
                    data->draw_render(data);
                    break;

                case SDLK_k:
                case SDLK_UP:
                    nag_position_prev(&data->notation);
                    data->draw_render(data);
                    break;

                case SDLK_x:
                case SDLK_BACKSPACE:
                    notation_move_get(&data->notation)->nag_position = 0;
                    loop = 0;
                    break;
                }
            }
        }
    }
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.normal_status);
    data->draw_render(data);
}

void
mode_tag_edit(WindowData *data, Tag *tag)
{
    int loop = 1;
    SDL_Event event;
    int pos = U8_strlen(tag->value);
    cursor_add(&pos, tag->value, TAG_LEN, data);
    data->draw_render(data);
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
                        notation_tag_remove(&data->notation, tag->key);
                    data->status.info[0] = '\0';
                    data->draw_render(data);
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
    data->draw_render(data);
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
                    data->draw_render(data);
                    break;

                case SDLK_RETURN:
                    cursor_remove(&pos, data->status.info);
                    tag = notation_tag_get(&data->notation, data->status.info);
                    if(tag == NULL){
                        notation_tag_set(&data->notation, data->status.info,
                                "");
                        tag = notation_tag_get(&data->notation,
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
    data->draw_render(data);
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
                    board_fen_export(&notation_move_get(
                                &data->notation)->board, fen);
                    SDL_SetClipboardText(fen);
                    message_add(data, &event, "FEN copied to clipboard");
                    break;

                case SDLK_ESCAPE:
                default:
                    data->status.info[0] = '\0';
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    loop = 0;
                    data->draw_render(data);
                    break;

                }
                break;
            }
        }
    }
}

void
game_init(Notation *n, Board *b)
{
    char fen[FEN_LEN];
    char date[20];
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(date, 20, "%Y.%m.%d", timeinfo);
    board_fen_export(b, fen);
    notation_init(n, b);
    if(strcmp(fen, FEN_DEFAULT))
        notation_tag_set(n, "FEN", fen);
    notation_tag_set(n, "Date", date);
}
