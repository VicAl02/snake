#include <ncurses.h>
#include "../include/snake.h"

#define HEIGHT 20
#define WIDTH 30
#define WTIME 500 

void print_snake(WINDOW *window, Body *body) {
    Piece *piece = body->head;
    Point pos;

    while (piece != NULL) {
        pos = piece->pos;
        mvwprintw(window, pos.y, pos.x, "*");
        piece = piece->next;
    }
}

int main() {
    WINDOW *window;
    int c;
    Point init_pos = {WIDTH / 2, HEIGHT / 2};
    Point init_dir = {0, -1};
    Body *body = s_new_body(init_pos, init_dir);

    initscr();
    curs_set(0);
    noecho();
    cbreak();

    window = newwin(HEIGHT, WIDTH, 0, 0);
    keypad(window, TRUE);

    mvwprintw(window, init_pos.y, init_pos.x, "*");
    wrefresh(window);
    
    // wait for first input
    wtimeout(window, -1);

    while (1) {
        box(window, '|', '-'); 

        int c = wgetch(window);
        wtimeout(window, 0);

        switch (c) {
            case KEY_LEFT:
                s_move_left(body);
                break;
            case KEY_UP:
                s_move_up(body);
                break;
            case KEY_RIGHT:
                s_move_right(body);
                break;
            case KEY_DOWN:
                s_move_down(body);
                break;
            case KEY_BACKSPACE:
                // insert new piece
                s_insert_piece(body);
            default:
                break;            
        }

        Point head = body->head->pos;    
        Point tail = body->tail->pos;
        mvwprintw(window, tail.y, tail.x, " ");
        s_forward(body, body->head->dir);
        print_snake(window, body);
        wrefresh(window);

        napms(WTIME);
    }

    delwin(window);

    endwin();

    return 0;
}