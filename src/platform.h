#ifndef PLATFORM_H_
#define PLATFORM_H_

typedef enum {
    CELL_EMPTY,
    CELL_SNAKE,
    CELL_FOOD
} cell_type;

void platform_draw_cell(int x, int y, cell_type ct);

#endif // PLATFORM_H_
