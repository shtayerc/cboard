#include "game_list.h"

void
mode_game_search(WindowData *data)
{
    int loop = 1;
    SDL_Event event;
    GameList new_gl;
    int pos = 0;
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    data->draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                loop = 0;
                data->loop = 0;
                break;

            case SDL_WINDOWEVENT:
                handle_resize(data, &event);
                data->draw_render(data);
                break;

            case SDL_MOUSEMOTION:
                data->mouse.x = event.button.x;
                data->mouse.y = event.button.y;
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    data->status.info[0] = '\0';
                    data->draw_render(data);
                    break;

                case SDLK_BACKSPACE:
                    textedit_backspace(&pos, data->status.info,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;

                case SDLK_DELETE:
                    textedit_delete(&pos, data->status.info);
                    data->draw_render(data);
                    break;

                case SDLK_LEFT:
                    textedit_left(&pos, data->status.info,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;

                case SDLK_RIGHT:
                    textedit_right(&pos, data->status.info,
                            data->conf.status_max_len, data);
                    data->draw_render(data);
                    break;

                case SDLK_RETURN:
                    cursor_remove(&pos, data->status.info);
                    game_list_search_str(&data->game_list, &new_gl,
                            data->status.info);
                    data->game_list = new_gl;
                    data->game_list_current = 0;
                    loop = 0;
                    data->status.info[0] = '\0';
                    data->draw_render(data);
                    break;
                }
                break;

            case SDL_TEXTINPUT:
                textedit_input(&pos, data->status.info,
                        data->conf.status_max_len, data, event.text.text);
                data->draw_render(data);
                break;
            }
        }
    }
}

void
mode_game_list(WindowData *data)
{
    int loop = 1;
    FILE *f;
    GameList new_gl;
    data->game_list_show = 1;
    int index = 0;
    char *comment;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.game_list_status);
    data->draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                loop = 0;
                data->loop = 0;
                break;

            case SDL_WINDOWEVENT:
                handle_resize(data, &event);
                data->draw_render(data);
                break;

            case SDL_MOUSEMOTION:
                data->mouse.x = event.button.x;
                data->mouse.y = event.button.y;
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    loop = 0;
                    data->loop = 0;
                    break;

                case SDLK_ESCAPE:
                    loop = 0;
                    data->game_list_show = 0;
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    data->draw_render(data);
                    break;

                case SDLK_DOWN:
                case SDLK_j:
                    if(data->game_list_current+1 < data->game_list.count){
                        data->game_list_current++;
                    }
                    data->draw_render(data);
                    break;

                case SDLK_PAGEUP:
                    data->game_list_scroll += data->conf.scroll_step;
                    data->draw_render(data);
                    break;

                case SDLK_PAGEDOWN:
                    data->game_list_scroll -= data->conf.scroll_step;
                    data->draw_render(data);
                    break;

                case SDLK_UP:
                case SDLK_k:
                    if(data->game_list_current-1 >= 0){
                        data->game_list_current--;
                    }
                    data->draw_render(data);

                case SDLK_LEFT:
                    break;

                case SDLK_RIGHT:
                    break;

                case SDLK_r:
                    f = fopen(data->filename, "r");
                    game_list_read_pgn(&data->game_list, f);
                    if(!(event.key.keysym.mod & KMOD_SHIFT))
                        game_list_reverse(&data->game_list);
                    fclose(f);
                    data->game_list_current = 0;
                    data->draw_render(data);
                    break;

                case SDLK_p:
                    f = fopen(data->filename, "r");
                    game_list_search_board(&data->game_list, &new_gl, f,
                            &data->board);
                    data->game_list = new_gl;
                    data->game_list_current = 0;
                    fclose(f);
                    data->draw_render(data);
                    break;

                case SDLK_c:
                    comment = notation_move_get(&data->notation)->comment;
                    if(comment != NULL)
                        free(comment);
                    index = data->game_list_current;
                    comment = strdup(data->game_list.list[index].title);
                    notation_move_get(&data->notation)->comment = comment;
                    data->draw_render(data);
                    break;


                case SDLK_RETURN:
                    board_fen_import(&data->board, FEN_DEFAULT);
                    notation_free(&data->notation);
                    notation_init(&data->notation, &data->board);
                    index = data->game_list.list[data->game_list_current].index;
                    f = fopen(data->filename, "r");
                    snprintf(data->number, data->conf.number_len, "%d", index);
                    pgn_read_file(f, &data->notation, index);
                    fclose(f);
                    loop = 0;
                    data->game_list_show = 0;
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    data->draw_render(data);
                    break;
                }

                break;

            case SDL_TEXTINPUT:
                switch(event.text.text[0]){
                case '/':
                    mode_game_search(data);
                    break;
                }
                break;
            }
        }
    }
}

void
game_list_draw(WindowData *data)
{
    SDL_Rect game_current;
    SDL_SetRenderDrawColor(data->renderer, NOTATION_BACKGROUND);
    SDL_RenderFillRect(data->renderer, &data->layout.notation);
    int i, x, y, word_height;
    x = data->layout.notation.x + NOTATION_PADDING_LEFT;
    y = data->layout.notation.y + NOTATION_PADDING_TOP
        + data->game_list_scroll;
    for(i = 0; i < data->game_list.count; i++){
        word_height = FC_GetHeight(data->font, data->game_list.list[i].title);
        if(i == data->game_list_current){
            game_current.x = x;
            game_current.y = y;
            game_current.w = data->layout.notation.w;
            game_current.h = word_height;
            SDL_SetRenderDrawColor(data->renderer,
                    NOTATION_CURRENT_MOVE_BACKGROUND);
            SDL_RenderFillRect(data->renderer, &game_current);
            FC_DrawColor(data->font, data->renderer, x, y,
                data->conf.notation_current_fg, data->game_list.list[i].title);
        }
        else{
            FC_DrawColor(data->font, data->renderer, x, y,
                    data->conf.notation_font_color,
                    data->game_list.list[i].title);
        }
        y += word_height;
    }
}

void
game_list_reverse(GameList *gl)
{
    int start = 0;
    int end = gl->count -1;
    GameRow tmp;
    if(end <= start)
        return;
    while(start < end){
        tmp = gl->list[start];
        gl->list[start] = gl->list[end];
        gl->list[end] = tmp;
        start++;
        end--;
    }
}
