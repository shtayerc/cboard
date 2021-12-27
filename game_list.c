#include "game_list.h"

void
mode_game_search(WindowData *data)
{
    int loop = 1;
    SDL_Event event;
    GameList new_gl;
    int pos = 0;
    data->message = 0;
    data->status.info[0] = '\0';
    cursor_add(&pos, data->status.info, data->conf.status_max_len, data);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if(event.type != SDL_KEYUP || event.key.keysym.sym != SDLK_ESCAPE){
                handle_input_events(&event, data, &loop, &pos,
                        data->status.info, data->conf.status_max_len);
            }
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    data->status.info[0] = '\0';
                    draw_render(data);
                    break;

                case SDLK_RETURN:
                    cursor_remove(&pos, data->status.info);
                    game_list_search_str(&data->game_list, &new_gl,
                            data->status.info);
                    game_list_free(&data->game_list);
                    data->game_list = new_gl;
                    data->game_list_current = 0;
                    loop = 0;
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
mode_game_list(WindowData *data)
{
    int loop = 1;
    FILE *f;
    GameList new_gl;
    data->notation_mode = ModeGameList;
    int index = 0;
    char *comment;
    SDL_Event event;
    snprintf(data->status.mode, data->conf.status_max_len, "%s",
            data->conf.game_list_status);
    machine_stop(data, 0);
    machine_stop(data, 1);
    draw_render(data);
    while (loop) {
        if (SDL_WaitEvent(&event)) {
            handle_global_events(&event, data, &loop, 1);
            if(event.type == SDL_KEYUP && event.key.keysym.sym != SDLK_r)
                handle_non_input_events(&event, data, &loop);
            switch (event.type) {
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    loop = 0;
                    data->notation_mode = ModeMoves;
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    draw_render(data);
                    break;

                case SDLK_DOWN:
                case SDLK_j:
                    if(data->game_list_current+1 < data->game_list.count){
                        data->game_list_current++;
                    }
                    game_list_focus_current_game(data);
                    draw_render(data);
                    break;

                case SDLK_PAGEUP:
                    data->game_list_scroll += data->conf.scroll_step;
                    draw_render(data);
                    break;

                case SDLK_PAGEDOWN:
                    data->game_list_scroll -= data->conf.scroll_step;
                    draw_render(data);
                    break;

                case SDLK_UP:
                case SDLK_k:
                    if(data->game_list_current-1 >= 0){
                        data->game_list_current--;
                    }
                    game_list_focus_current_game(data);
                    draw_render(data);

                case SDLK_LEFT:
                    break;

                case SDLK_RIGHT:
                    break;

                case SDLK_r:
                    f = fopen(data->filename, "r");
                    if(f == NULL){
                        message_add(data, &event, "Error reading file");
                        break;
                    }
                    game_list_read_pgn(&data->game_list, f);
                    fclose(f);
                    data->game_list_sorting = Ascending;
                    if(!(event.key.keysym.mod & KMOD_SHIFT)){
                        game_list_reverse(&data->game_list);
                        data->game_list_sorting = Descending;
                    }
                    data->game_list_current = 0;
                    draw_render(data);
                    break;

                case SDLK_p:
                    f = fopen(data->filename, "r");
                    game_list_search_board(&data->game_list, &new_gl, f,
                            &game_move_get(&data->game)->board);
                    fclose(f);
                    game_list_free(&data->game_list);
                    data->game_list = new_gl;
                    data->game_list_current = 0;
                    if(data->game_list_sorting == Descending)
                        game_list_reverse(&data->game_list);
                    draw_render(data);
                    break;

                case SDLK_c:
                    comment = game_move_get(&data->game)->comment;
                    if(comment != NULL)
                        free(comment);
                    index = data->game_list_current;
                    comment = strdup(data->game_list.list[index].title);
                    game_move_get(&data->game)->comment = comment;
                    draw_render(data);
                    break;

                case SDLK_t:
                    if(event.key.keysym.mod & KMOD_SHIFT){
                        index = rand() % data->game_list.count;
                        game_list_game_load(data, index);
                        loop = 0;
                        data->notation_mode = ModeMoves;
                        data->from_game_list = 1;
                        mode_training(data);
                    }
                    break;

                case SDLK_RETURN:
                    game_list_game_load(data, -1);
                    loop = 0;
                    data->notation_mode = ModeMoves;
                    snprintf(data->status.mode, data->conf.status_max_len,
                            "%s", data->conf.normal_status);
                    draw_render(data);
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
    SDL_Color c = data->conf.colors[ColorNotationBackground];
    SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRect(data->renderer, &data->layout.notation);
    int i, x, y;
    x = data->layout.notation.x + NOTATION_PADDING_LEFT;
    y = data->layout.notation.y + NOTATION_PADDING_TOP
        + data->game_list_scroll;
    if(data->game_list.count == 0){
        FC_DrawColor(data->font, data->renderer, x, y,
                data->conf.colors[ColorNotationFont], "No games");
        return;
    }
    c = data->conf.colors[ColorNotationActiveBackground];
    for(i = 0; i < data->game_list.count; i++){
        if(i == data->game_list_current){
            game_current.x = x;
            game_current.y = y;
            game_current.w = data->layout.notation.w;
            game_current.h = data->font_height;
            SDL_SetRenderDrawColor(data->renderer, c.r, c.g, c.b, c.a);
            SDL_RenderFillRect(data->renderer, &game_current);
            FC_DrawColor(data->font, data->renderer, x, y,
                data->conf.colors[ColorNotationActiveFont], data->game_list.list[i].title);
        }
        else{
            FC_DrawColor(data->font, data->renderer, x, y,
                    data->conf.colors[ColorNotationFont],
                    data->game_list.list[i].title);
        }
        y += data->font_height;
        if(y > data->layout.notation.y + data->layout.notation.h)
            break;
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

void
game_list_focus_current_game(WindowData *data)
{
    int y = data->layout.notation.y + NOTATION_PADDING_TOP
        + data->game_list_scroll;
    y += data->font_height * data->game_list_current;
    int top = data->layout.notation.y + NOTATION_PADDING_TOP;
    int bot = data->layout.notation.y + data->layout.notation.h;
    if(y + data->font_height > bot){
        data->game_list_scroll -= y - bot;
        data->game_list_scroll -= data->font_height;
    }else if(y < top){
        data->game_list_scroll -= y - NOTATION_PADDING_TOP;
    }
}

void
game_list_game_load(WindowData *data, int index)
{
    Board tmp_b = game_move_get(&data->game)->board;
    int gl_index = index == -1 ? data->game_list_current : index;
    index = data->game_list.list[gl_index].index;
    game_free(&data->game);
    game_init(&data->game, NULL);
    snprintf(data->number, data->conf.number_len, "%d", index);
    FILE *f = fopen(data->filename, "r");
    pgn_read_file(f, &data->game, index);
    fclose(f);
    game_board_find(&data->game, &tmp_b);
}
