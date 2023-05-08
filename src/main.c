#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

#include "./cell.h"
#include "./game.h"
#include "./platform.h"

int cell_type_to_char(int type)
{
    switch(type) {
        case CELL_EMPTY:
            return ' ';
        case CELL_SNAKE:
            return '0';
        case CELL_FOOD:
            return '*';
        default:
            assert(0 && "unreachable");
            return 0;
    }
}

void platform_draw_cell(int x, int y, int cell_type)
{
    mvaddch(y, x, cell_type_to_char(cell_type));
}

void platform_render(void)
{
    refresh();
}

void *platform_malloc(unsigned int size)
{
    return malloc(size);
}

int main(void)
{
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
    int h, w;
    getmaxyx(pw, h, w);

    game_init(w, h, time(NULL));
    struct timespec start, end;
    int diff;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (1) {
        wchar_t c;
        if ((c = wgetch(pw)) != ERR)
            game_handle_key(c);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
        if (game_update(diff))
            break;
        start = end;
        usleep(1000); // throttling
    }
    if (curs_st != ERR)
        curs_set(1);
    endwin();
    printf("GAME OVER\n");
    return 0;
}
