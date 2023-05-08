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
    srand(time(0));

    game_init(w, h);
    while (1) {
        wchar_t c;
        if ((c = wgetch(pw)) != ERR)
            game_handle_key(c);
        if (game_update())
            break;
        usleep(80000);
    }
    if (curs_st != ERR)
        curs_set(1);
    endwin();
    printf("GAME OVER\n");
    return 0;
}
