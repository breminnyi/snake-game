#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

#include "./game.h"
#include "./platform.h"

int cell_type_to_char(cell_type type)
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

void platform_draw_cell(int x, int y, cell_type ct)
{
    mvaddch(y + 1, x + 1, cell_type_to_char(ct));
}

static void draw_borders(int w, int h)
{
    for (int x = 0; x < w; x++) {
        mvaddch(0, x, '-');
        mvaddch(h - 1, x, '-');
    }
    for (int y = 0; y < h; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, w - 1, '|');
    }
}

#define MIN(A, B)   ({ typeof(A) _a = (A); typeof(B) _b = (B); _a < _b ? _a : _b; })

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
    h = MIN(h, 32 + 2); // 32x32 and borders
    w = MIN(w, 32 + 2);
    draw_borders(w, h);
    refresh();

    if (game_init(w - 2, h - 2, time(NULL))) {
        struct timespec start, end;
        int diff;
        clock_gettime(CLOCK_MONOTONIC, &start);
        while (1) {
            wchar_t c;
            if ((c = wgetch(pw)) != ERR)
                game_handle_key(c);
            clock_gettime(CLOCK_MONOTONIC, &end);
            diff = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
            if (!game_update(diff))
                // TODO: print GAME OVER in the middle of a screen
                break;
            refresh();
            start = end;
            usleep(1000); // throttling
        }
    }
    if (curs_st != ERR)
        curs_set(1);
    endwin();
    printf("GAME OVER\n");
    return 0;
}
