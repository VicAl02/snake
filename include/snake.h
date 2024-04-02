#include <stdio.h>
#include <stdlib.h>

typedef struct point_s {
    int x, y;
} Point;

typedef struct piece_s {
    Point pos, dir;
    struct piece_s *next;    
} Piece;

typedef struct body_s {
    Piece *head;
    Piece *tail;
    int length;
} Body;

Body *s_new_body(Point, Point);
void s_insert_piece(Body *);
void s_free_body(Body *);
void s_move_left(Body *);
void s_move_right(Body *);
void s_move_up(Body *);
void s_move_down(Body *);
void s_forward(Body *, Point); 
void s_print_body(Body *);
Point s_rand_pos(Body *body, Point upper_left, Point lower_right);
int s_has_eaten(Body *body, Point apple);
int s_has_collided(Body *body, Point upper_left, Point lower_right);