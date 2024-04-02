#include "../include/snake.h"

int main() {
    Body *body = s_new_body((Point) {0, 0}, (Point) {0, 0});
    char c;

    s_print_body(body);
    while(1) {
        scanf(" %c", &c);

        switch (c) {
            case 'w':
                s_move_up(body);
                break;
            case 'a':
                s_move_left(body);
                break;
            case 's':
                s_move_down(body);
                break;
            case 'd':
                s_move_right(body);
                break;
            case 'z':
                s_insert_piece(body);
                break;
        }

        s_forward(body, body->head->dir);
        
        s_print_body(body);
    }

    return 0;
}