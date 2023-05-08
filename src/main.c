#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

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

void pltf_draw_empty(int x, int y)
{
    mvaddch(y, x, ' ');
}

void pltf_draw_snake(int x, int y)
{
    mvaddch(y, x, '0');
}

void pltf_draw_food(int x, int y)
{
    mvaddch(y, x, '*');
}

int main(void)
{
    srand(time(0));
    WINDOW *pw;
    if ((pw = initscr()) == NULL) {
        fprintf(stderr, "ERROR: could not initialize curses mode\n");
        return 1;
    }
    cbreak();           // sets input mode to cbreak
    keypad(stdscr, 1);  // allows to get arrow keys
    noecho();           // suppresses output on getch
    nodelay(pw, 1);     // no delay mode
    int curs_st = curs_set(0);    // hides the cursor
    int w, h;
    getmaxyx(pw, h, w);
    int dir = RIGHT, newdir;
    struct part *head, *tail, *tmp;
    head = tail = snake_create_part(rand() % w, rand() % h);
    for (tmp = tail; tmp != NULL; tmp = tmp->next) {
        pltf_draw_snake(tmp->x, tmp->y);
        head = tmp;
    }
    int fdx = rand() % w, fdy = rand() % h;
    pltf_draw_food(fdx, fdy);
    refresh();
    int x, y, dx = dirtodx(dir), dy = dirtody(dir);
    wchar_t c;
    while (1) {
        if ((c = wgetch(pw)) != ERR && (newdir = keytodir(c)) >= 0 && canchdir(dir, newdir)) {
            dir = newdir;
            dx = dirtodx(dir);
            dy = dirtody(dir);
        }
        pltf_draw_empty(tail->x, tail->y);
        x = head->x + dx;
        y = head->y + dy;
        if (x < 0)
            x = w - 1;
        else if (x >= w)
            x = 0;
        else if (y < 0)
            y = h - 1;
        else if (y >= h)
            y = 0;

        if (x == fdx && y == fdy) {
            head->next = snake_create_part(x, y);
            head = head->next;
            while (snake_hit_test(tail, fdx = rand() % w, fdy = rand() % h))
                ;
            pltf_draw_food(fdx, fdy);
        } else {
            pltf_draw_empty(tail->x, tail->y);
            if (head != tail) {
                tmp = tail;
                tail = tail->next;
                tmp->next = NULL;
                head->next = tmp;
                head = head->next;
            }
            head->x = x;
            head->y = y;
        }
        pltf_draw_snake(head->x, head->y);

        refresh();
        usleep(80000);
    }
    if (curs_st != ERR)
        curs_set(1);
    endwin();
    return 0;
}
