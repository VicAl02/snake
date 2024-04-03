#include <ncurses.h>
#include "../include/snake.h"

#define HEIGHT 20
#define WIDTH 30
#define WTIME_SLOW 400
#define WTIME_MEDIUM 200
#define WTIME_FAST 100

typedef enum menu_sel_e {
    PLAY,
    SPEED,
    QUIT,
    NUM_SELECTIONS
} MenuSel;

typedef enum speed_sel_e {
    SLOW,
    MEDIUM,
    FAST,
    NUM_SPEEDS
} SpeedSel;

int wtime = WTIME_MEDIUM; 

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
void set_selection_cursor(WINDOW *menu_win, MenuSel selection) {
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

void speed_menu() {
    WINDOW *speed_win = newwin(HEIGHT, WIDTH, 0, 0);
    
    keypad(speed_win, TRUE);
    box(speed_win, 0, 0);
    
    mvwprintw(speed_win, 1, 1, "Speed");
    mvwprintw(speed_win, 3, 3, "Slow");
    mvwprintw(speed_win, 4, 1, ">");
    mvwprintw(speed_win, 4, 3, "Medium");
    mvwprintw(speed_win, 5, 3, "Fast");    

    timeout(-1);
    int c;
    SpeedSel selection = MEDIUM;
    while(1) {
        c = wgetch(speed_win);

        switch (c) {
            case KEY_UP:
                selection = selection == 0 ? NUM_SPEEDS - 1 : (selection - 1);
                break;
            case KEY_DOWN:
                selection = (selection + 1) % NUM_SPEEDS;
                break;
            case 10:
                // Enter
                switch (selection) {
                    case SLOW:
                        wtime = WTIME_SLOW;
                        delwin(speed_win);
                        return;
                    case MEDIUM:
                        wtime = WTIME_MEDIUM;
                        delwin(speed_win);
                        return;
                    case FAST:
                        wtime = WTIME_FAST;
                        delwin(speed_win);
                        return;
                    default:
                        break;
                }
            default:
                break;
        }

        set_selection_cursor(speed_win, selection);
    }
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
    wrefresh(menu_win);

    timeout(-1);
    int c;
    MenuSel selection = PLAY;
    while(1) {
        c = wgetch(menu_win);

        switch (c) {
            case KEY_UP:
                selection = selection == 0 ? NUM_SELECTIONS - 1 : (selection - 1);
                break;
            case KEY_DOWN:
                selection = (selection + 1) % NUM_SELECTIONS;
                break;
            case 10:
                // Enter
                switch (selection) {
                    case PLAY:
                        delwin(menu_win);
                        return;
                    case SPEED:
                        delwin(menu_win);
                        speed_menu();
                        menu_win = newwin(HEIGHT, WIDTH, 0, 0);
                        keypad(menu_win, TRUE);
                        box(menu_win, 0, 0);
                        mvwprintw(menu_win, 1, 1, "Welcome to Snake!");
                        mvwprintw(menu_win, 3, 1, ">");
                        mvwprintw(menu_win, 3, 3, "Play");
                        mvwprintw(menu_win, 4, 3, "Speed");
                        mvwprintw(menu_win, 5, 3, "Quit");
                        wrefresh(menu_win);
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
}

int main() {
    WINDOW *window;
    int c;
    Point init_pos = {WIDTH / 2, HEIGHT / 2};
    Point init_dir = {0, -1};

    initscr();
    curs_set(0);
    noecho();
    cbreak();

    while(1) {
        menu();

        Body *body = s_new_body(init_pos, init_dir);

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

            napms(wtime);
        }
        
        s_free_body(body);
        delwin(window);
    }

    endwin();

    return 0;
}