#include "./game.h"
#include "./platform.h"

#define NULL    ((void*)0)

#define RIGHT   6   // 0110
#define LEFT    4   // 0100
#define UP      1   // 0001
#define DOWN    9   // 1001

#define canchdir(d0,d1) (!(d0 & d1))
#define dirtodx(dir)    ((dir & 3) - 1)
#define dirtody(dir)    (((dir >> 2) & 3) - 1)

#define RAND_MAX    0x7FFF
static unsigned int rand_seed;
static unsigned int rand(void)
{
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed >> 16) & RAND_MAX;
}

static int randdir(void)
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

static int key_to_dir(int k)
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

#define MIN_INTERVAL    10
#define MAX_INTERVAL    640

// Game state
static int width, height;
static int dir;
static int foodx, foody;
static struct part *head, *tail;
static long elapsed = 0, last_render = 0;
static int interval = 80;

#define randx() (rand() % width)
#define randy() (rand() % height)

static struct part *create_part(int x, int y)
{
    struct part *p = platform_malloc(sizeof(struct part));
    p->x = x;
    p->y = y;
    p->next = NULL;
    return p;
}

static int hit_test(struct part *tail, int x, int y)
{
    for (; tail->next != NULL; tail = tail->next)
        if (tail->x == x && tail->y == y)
            return 1;
    return 0;
}

void game_init(int w, int h, int seed)
{
    rand_seed = seed;
    width = w;
    height = h;
    dir = randdir();
    head = tail = create_part(randx(), randy());
    platform_draw_cell(head->x, head->y, CELL_SNAKE);
    while (hit_test(tail, foodx = randx(), foody = randy()))
        ;
    platform_draw_cell(foodx, foody, CELL_FOOD);
    platform_render();
}

void game_handle_key(int c)
{
    int newdir = key_to_dir(c);
    if ((newdir) >= 0) {
        if (canchdir(dir, newdir))
            dir = newdir;
    } else if (c == '[') {
        if (interval < MAX_INTERVAL)
            interval <<= 1;
    } else if (c == ']') {
        if (interval > MIN_INTERVAL)
            interval >>= 1;
    }
}

int game_update(int dt)
{
    if ((elapsed += dt) - last_render < interval)
        return 0;
    platform_draw_cell(tail->x, tail->y, CELL_EMPTY);
    int x = head->x + dirtodx(dir);
    int y = head->y + dirtody(dir);
    if (x < 0)
        x = width - 1;
    else if (x >= width)
        x = 0;
    else if (y < 0)
        y = height - 1;
    else if (y >= height)
        y = 0;

    if (x == foodx && y == foody) {
        head->next = create_part(x, y);
        head = head->next;
        while (hit_test(tail, foodx = randx(), foody = randy()))
            ;
        platform_draw_cell(foodx, foody, CELL_FOOD);
    } else {
        if (hit_test(tail, x, y))
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
    last_render = elapsed;
    return 0;
}
