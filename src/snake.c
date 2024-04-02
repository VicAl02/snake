#include "../include/snake.h"

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif 

Piece *new_piece(Point pos, Point dir, Piece *next) {
    Piece *piece = malloc(sizeof(Piece));
    
    piece->pos = pos;
    piece->dir = dir;
    piece->next = next;

    return piece;
}

// Creates a new snake body with the head initialized to pos and dir
Body *s_new_body(Point pos, Point dir) {
    Body *body = malloc(sizeof(Body));

    body->head = body->tail = new_piece(pos, dir, NULL);
    body->length = 1;

    return body;
}

// tail insertion
void s_insert_piece(Body *body) {    
    Point dir = body->tail->dir;
    Point pos = {body->tail->pos.x - dir.x, body->tail->pos.y - dir.y};

    body->tail->next = new_piece(pos, dir, NULL);
    body->tail = body->tail->next;
    body->length++;    
}

void s_move_left(Body *body) {
    body->head->dir.x = -1;
    body->head->dir.y = 0;
}

void s_move_right(Body *body) {
    body->head->dir.x = 1;
    body->head->dir.y = 0;
}

void s_move_up(Body *body) {
    body->head->dir.x = 0;
    body->head->dir.y = -1;
}

void s_move_down(Body *body) {
    body->head->dir.x = 0;
    body->head->dir.y = 1;
}

// Moves the snake forward in the specified direction
void s_forward(Body *body, Point dir) {
    Piece *piece = body->head;

    // shifts the pieces
    Piece prev = *piece;
    while (piece->next != NULL) {
        Piece cur = *(piece->next);
        piece->next->pos = prev.pos;
        piece->next->dir = prev.dir;
        piece = piece->next;
        prev = cur;
    }

    // updates the snake head
    Piece *head = (Piece *) body->head;
    head->pos.x += dir.x;
    head->pos.y += dir.y;
    head->dir.x = dir.x;
    head->dir.y = dir.y;
}

void s_print_body(Body *l) {
    Piece *n = l->head;

    int i = 1;
    while (n != NULL) {
        Piece p = *((Piece *) n);
        printf("%d: {{%d, %d}, {%d, %d}}\n", i, p.pos.x, p.pos.y, p.dir.x, p.dir.y);
        n = n->next;
        i++;
    }
}

int s_is_point_in_body(Body *body, Point pos) {
    Piece *piece = body->head;

    while (piece != NULL) {
        if (piece->pos.x == pos.x && piece->pos.y == pos.y) {
            return 1;
        }
        piece = piece->next;
    }

    return 0;
}

Point s_rand_pos(Body *body, Point upper_left, Point lower_right) {
    Point pos;

    do {
        pos.x = rand() % (lower_right.x - upper_left.x) + upper_left.x;
        pos.y = rand() % (lower_right.y - upper_left.y) + upper_left.y;
    } while (s_is_point_in_body(body, pos));    

    return pos;
}

int s_has_eaten(Body *body, Point apple) {
    return body->head->pos.x == apple.x && body->head->pos.y == apple.y;
}

int s_has_collided(Body *body, Point upper_left, Point lower_right) {
    Piece *piece = body->head;
    Point pos = piece->pos;

    // check if the head is out of bounds
    if (pos.x < upper_left.x || pos.x >= lower_right.x || pos.y < upper_left.y || pos.y >= lower_right.y) {
        return 1;
    }

    // check if the head has collided with the body
    piece = piece->next;
    while (piece != NULL) {
        if (pos.x == piece->pos.x && pos.y == piece->pos.y) {
            return 1;
        }
        piece = piece->next;
    }

    return 0;
}