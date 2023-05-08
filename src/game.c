#include "./game.h"
#include "./platform.h"

#define NULL    ((void*)0)

#define RIGHT   6   // 0110
#define LEFT    4   // 0100
#define UP      1   // 0001
#define DOWN    9   // 1001

#define CAN_CHANGE_DIR(d0,d1)   (!((d0) & (d1)))
#define DIR_TO_DX(d)            (((d) & 3) - 1)
#define DIR_TO_DY(d)            ((((d) >> 2) & 3) - 1)

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

struct Part {
    int x;
    int y;
    struct Part *next;
};

#define MIN_INTERVAL    10
#define MAX_INTERVAL    640

// Game state
static int width, height;
static int cur_dir, new_dir;
static int foodx, foody;
static struct Part *head, *tail;
static long elapsed = 0, last_render = 0;
static int interval = 80;

#define randx() (rand() % width)
#define randy() (rand() % height)

#define MAXCELLS    1024
static struct Part parts[MAXCELLS];
static struct Part *partsp = parts;

static struct Part *alloc_part(void)
{
    return (partsp >= parts + MAXCELLS) ? NULL : partsp++;
}

static struct Part *create_part(int x, int y)
{
    struct Part *p = alloc_part();
    if (p != NULL) {
        p->x = x;
        p->y = y;
        p->next = NULL;
    }
    return p;
}

static int hit_test(struct Part *tail, int x, int y)
{
    for (; tail->next != NULL; tail = tail->next)
        if (tail->x == x && tail->y == y)
            return 1;
    return 0;
}

int game_init(int w, int h, int seed)
{
    if (w * h > MAXCELLS)
        return 0;
    rand_seed = seed;
    width = w;
    height = h;
    cur_dir = new_dir = randdir();
    if ((head = tail = create_part(randx(), randy())) == NULL)
        return 0;
    platform_draw_cell(head->x, head->y, CELL_SNAKE);
    while (hit_test(tail, foodx = randx(), foody = randy()))
        ;
    platform_draw_cell(foodx, foody, CELL_FOOD);
    return 1;
}

void game_handle_key(int c)
{
    int dir = key_to_dir(c);
    if ((dir) >= 0) {
        if (CAN_CHANGE_DIR(cur_dir, dir))
            new_dir = dir;
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
        return 1;
    platform_draw_cell(tail->x, tail->y, CELL_EMPTY);
    int x = head->x + DIR_TO_DX(cur_dir = new_dir);
    int y = head->y + DIR_TO_DY(cur_dir);
    if (x < 0)
        x = width - 1;
    else if (x >= width)
        x = 0;
    else if (y < 0)
        y = height - 1;
    else if (y >= height)
        y = 0;

    if (x == foodx && y == foody) {
        if ((head->next = create_part(x, y)) == NULL)
            return 0;
        head = head->next;
        while (hit_test(tail, foodx = randx(), foody = randy()))
            ;
        platform_draw_cell(foodx, foody, CELL_FOOD);
    } else {
        if (hit_test(tail, x, y))
            return 0;

        platform_draw_cell(tail->x, tail->y, CELL_EMPTY);
        if (head != tail) {
            struct Part *tmp = tail;
            tail = tail->next;
            tmp->next = NULL;
            head->next = tmp;
            head = head->next;
        }
        head->x = x;
        head->y = y;
    }
    platform_draw_cell(head->x, head->y, CELL_SNAKE);
    last_render = elapsed;
    return 1;
}
