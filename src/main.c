#include <ncurses.h>
#include "../include/snake.h"

#define HEIGHT 20
#define WIDTH 30
#define WTIME 200 

typedef enum selection_e {
    PLAY,
    SPEED,
    QUIT,
    NUM_SELECTIONS
} Selection;

void print_snake(WINDOW *window, Body *body) {
    Piece *piece = body->head;
    Point pos;

    while (piece != NULL) {
        pos = piece->pos;
        mvwprintw(window, pos.y, pos.x, "*");
        piece = piece->next;
    }
}

// TODO: Refactor to be more concise.
void set_selection_cursor(WINDOW *menu_win, Selection selection) {
    switch (selection) {
        case PLAY:
            mvwprintw(menu_win, 3, 1, ">");
            mvwprintw(menu_win, 4, 1, " ");
            mvwprintw(menu_win, 5, 1, " ");
            break;
        case SPEED:
            mvwprintw(menu_win, 3, 1, " ");
            mvwprintw(menu_win, 4, 1, ">");
            mvwprintw(menu_win, 5, 1, " ");
            break;
        case QUIT:
            mvwprintw(menu_win, 3, 1, " ");
            mvwprintw(menu_win, 4, 1, " ");
            mvwprintw(menu_win, 5, 1, ">");
            break;
        default:
            break;
    }
    
    wrefresh(menu_win);

}

void menu() {
    WINDOW *menu_win = newwin(HEIGHT, WIDTH, 0, 0);
    
    keypad(menu_win, TRUE);
    box(menu_win, 0, 0);
    
    mvwprintw(menu_win, 1, 1, "Welcome to Snake!");
    mvwprintw(menu_win, 3, 1, ">");
    mvwprintw(menu_win, 3, 3, "Play");
    mvwprintw(menu_win, 4, 3, "Speed");
    mvwprintw(menu_win, 5, 3, "Quit");    

    timeout(-1);
    int c;
    Selection selection = PLAY;
    int cursor_y = 1;
    while(1) {
        c = wgetch(menu_win);

        switch (c) {
            case KEY_UP:
                selection = (selection - 1) % NUM_SELECTIONS;
                break;
            case KEY_DOWN:
                selection = (selection + 1) % NUM_SELECTIONS;
                break;
            case 10:
                // Enter
                switch (selection) {
                    case PLAY:
                        return;
                    case SPEED:
                        // TODO: Implement speed menu selection.
                        break;
                    case QUIT:
                        endwin();
                        exit(0);
                        break;
                    default:
                        break;
                }
            default:
                break;
        }

        set_selection_cursor(menu_win, selection);
    }

    wrefresh(menu_win);
    wgetch(menu_win);
    delwin(menu_win);
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

    menu();

    window = newwin(HEIGHT, WIDTH, 0, 0);
    keypad(window, TRUE);

    mvwprintw(window, init_pos.y, init_pos.x, "*");
    wrefresh(window);
    
    // wait for first input
    wtimeout(window, -1);

    Point apple = s_rand_pos(body, (Point){1, 1}, (Point){WIDTH - 2, HEIGHT - 2});
    mvwprintw(window, apple.y, apple.x, "O");

    while (1) {
        box(window, 0, 0); 

        c = wgetch(window);
        wtimeout(window, 0);

        // TODO: Prevent spamming keys.

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
            default:
                break;            
        }

        Point head = body->head->pos;    
        Point tail = body->tail->pos;
        mvwprintw(window, tail.y, tail.x, " ");
        s_forward(body, body->head->dir);
        
        if (s_has_collided(body, (Point){1, 1}, (Point){WIDTH - 1, HEIGHT - 1})) {
            break;
        }

        if (s_has_eaten(body, apple)) {
            s_insert_piece(body);
            mvwprintw(window, apple.y, apple.x, " ");
            apple = s_rand_pos(body, (Point){1, 1}, (Point){WIDTH - 2, HEIGHT - 2});
            mvwprintw(window, apple.y, apple.x, "O");
        }

        print_snake(window, body);
        wrefresh(window);

        napms(WTIME);
    }
    
    s_free_body(body);
    delwin(window);

    menu();

    endwin();

    return 0;
}