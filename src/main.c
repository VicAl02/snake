#include <ncurses.h>
#include "../include/snake.h"

#define MAX_WIDTH 30
#define MAX_HEIGHT 15
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

void set_selection_cursor(WINDOW *menu_win, int option_index, int num_options, int first_option_y, int x) {
    for (int i = 0; i < num_options; i++) {
        if (i == option_index) {
            mvwprintw(menu_win, first_option_y + i, x, ">");
        } else {
            mvwprintw(menu_win, first_option_y + i, x, " ");
        }
    }
    
    wrefresh(menu_win);
}

void clear_menu(WINDOW *menu_win) {
    werase(menu_win);
    wrefresh(menu_win);
    delwin(menu_win);
}

void speed_menu() {
    SpeedSel selection;

    switch (wtime) {
        case WTIME_SLOW:
            selection = SLOW;
            break;
        case WTIME_MEDIUM:
            selection = MEDIUM;
            break;
        case WTIME_FAST:
            selection = FAST;
            break;
    }

    WINDOW *speed_win = newwin(LINES, COLS, 0, 0);
    
    keypad(speed_win, TRUE);
    
    int x = (COLS - 30) / 2;  
    int y = (LINES - 5) / 2;

    mvwprintw(speed_win, y, x, "Speed");
    mvwprintw(speed_win, y + 2 + selection, x, ">");
    mvwprintw(speed_win, y + 2, x + 2, "Slow");
    mvwprintw(speed_win, y + 3, x + 2, "Medium");
    mvwprintw(speed_win, y + 4, x + 2, "Fast");   
    wrefresh(speed_win); 

    timeout(-1);
    int c;
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
                        clear_menu(speed_win);
                        return;
                    case MEDIUM:
                        wtime = WTIME_MEDIUM;
                        clear_menu(speed_win);
                        return;
                    case FAST:
                        wtime = WTIME_FAST;
                        clear_menu(speed_win);
                        return;
                    default:
                        break;
                }
            default:
                break;
        }

        set_selection_cursor(speed_win, selection, NUM_SPEEDS, y + 2, x);
    }
}

WINDOW *create_menu(int y, int x) {
    WINDOW *menu_win = newwin(LINES, COLS, 0, 0);
    
    keypad(menu_win, TRUE);

    mvwprintw(menu_win, y, x, "Welcome to Snake!");
    mvwprintw(menu_win, y + 2, x, ">");
    mvwprintw(menu_win, y + 2, x + 2, "Play");
    mvwprintw(menu_win, y + 3, x + 2, "Speed");
    mvwprintw(menu_win, y + 4, x + 2, "Quit");    
    wrefresh(menu_win);

    return menu_win;
}

void menu() {
    int x = (COLS > 30) ? (COLS - 30) / 2 : 1;
    int y = (LINES - 5) / 2;

    WINDOW *menu_win = create_menu(y, x);

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
                        clear_menu(menu_win);
                        return;
                    case SPEED:
                        clear_menu(menu_win);
                        speed_menu();
                        menu_win = create_menu(y, x);
                        break;
                    case QUIT:
                        endwin();
                        exit(0);
                }
        }

        set_selection_cursor(menu_win, selection, NUM_SELECTIONS, y + 2, x);
    }
}

int main() {
    WINDOW *window;
    int c, width, height;

    initscr();
    curs_set(0);
    noecho();
    cbreak();
    
    while(1) {
        menu();
        
        width = (COLS > MAX_WIDTH) ? MAX_WIDTH : COLS;
        height = (LINES > MAX_HEIGHT) ? MAX_HEIGHT : LINES;

        Point init_pos = (Point){width / 2, height / 2};
        Body *body = s_new_body(init_pos, (Point){0, 0});

        window = newwin(height, width, (LINES - height) / 2, (COLS - width) / 2);
        keypad(window, TRUE);

        mvwprintw(window, init_pos.y, init_pos.x, "*");
        wrefresh(window);
        
        // wait for first input
        wtimeout(window, -1);

        Point apple = s_rand_pos(body, (Point){1, 1}, (Point){width - 2, height - 2});
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
            
            if (s_has_collided(body, (Point){1, 1}, (Point){width - 1, height - 1})) {
                break;
            }

            if (s_has_eaten(body, apple)) {
                s_insert_piece(body);
                mvwprintw(window, apple.y, apple.x, " ");
                apple = s_rand_pos(body, (Point){1, 1}, (Point){width - 2, height - 2});
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