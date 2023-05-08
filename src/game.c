#include <stdlib.h>

#include "./cell.h"
#include "./game.h"
#include "./platform.h"

#define RIGHT   6   // 0110
#define LEFT    4   // 0100
#define UP      1   // 0001
#define DOWN    9   // 1001

#define canchdir(d0,d1) (!(d0 & d1))
#define dirtodx(dir)    ((dir & 3) - 1)
#define dirtody(dir)    (((dir >> 2) & 3) - 1)

int randdir(void)
{
    switch (rand() & 3) {
        case 0:
            return RIGHT;
        case 1:
            return DOWN;
        case 2:
            return LEFT;
        default:
            return UP;
    }
}

#define KEY_DOWN    258
#define KEY_UP      259
#define KEY_LEFT    260
#define KEY_RIGHT   261

int keytodir(wchar_t k)
{
    switch (k) {
        case KEY_UP:
        case 'W':
        case 'w':
            return UP;
        case KEY_DOWN:
        case 'S':
        case 's':
            return DOWN;
        case KEY_LEFT:
        case 'A':
        case 'a':
            return LEFT;
        case KEY_RIGHT:
        case 'D':
        case 'd':
            return RIGHT;
    }
    return -1;
}

struct part {
    int x;
    int y;
    struct part *next;
};

struct part *snake_create_part(int x, int y)
{
    struct part *p = malloc(sizeof(struct part));
    p->x = x;
    p->y = y;
    p->next = NULL;
    return p;
}

int snake_hit_test(struct part *tail, int x, int y)
{
    for (; tail->next; tail = tail->next) {
        if (tail->x == x && tail->y == y)
            return 1;
    }
    return 0;
}

int width, height, dir, newdir, dx, dy, fdx, fdy;
struct part *head, *tail;

void game_init(int w, int h)
{
    width = w;
    height = h;
    dir = randdir();
    dx = dirtodx(dir);
    dy = dirtody(dir);
    head = tail = snake_create_part(rand() % width, rand() % height);
    platform_draw_cell(head->x, head->y, CELL_SNAKE);
    while (snake_hit_test(tail, fdx = rand() % width, fdy = rand() % height))
        ;
    platform_draw_cell(fdx, fdy, CELL_FOOD);
    platform_render();
}

void game_handle_key(int c)
{
    if ((newdir = keytodir(c)) >= 0 && canchdir(dir, newdir)) {
        dir = newdir;
        dx = dirtodx(dir);
        dy = dirtody(dir);
    }
}

int game_update(void)
{
    platform_draw_cell(tail->x, tail->y, CELL_EMPTY);
    int x = head->x + dx;
    int y = head->y + dy;
    if (x < 0)
        x = width - 1;
    else if (x >= width)
        x = 0;
    else if (y < 0)
        y = height - 1;
    else if (y >= height)
        y = 0;

    if (x == fdx && y == fdy) {
        head->next = snake_create_part(x, y);
        head = head->next;
        while (snake_hit_test(tail, fdx = rand() % width, fdy = rand() % height))
            ;
        platform_draw_cell(fdx, fdy, CELL_FOOD);
    } else {
        if (snake_hit_test(tail, x, y))
            return 1;

        platform_draw_cell(tail->x, tail->y, CELL_EMPTY);
        if (head != tail) {
            struct part *tmp = tail;
            tail = tail->next;
            tmp->next = NULL;
            head->next = tmp;
            head = head->next;
        }
        head->x = x;
        head->y = y;
    }
    platform_draw_cell(head->x, head->y, CELL_SNAKE);
    platform_render();
    return 0;
}
