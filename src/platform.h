#ifndef PLATFORM_H_
#define PLATFORM_H_

typedef enum {
    CELL_EMPTY,
    CELL_SNAKE,
    CELL_FOOD
} cell_type;

void platform_draw_cell(int x, int y, cell_type ct);
void platform_render(void);
void* platform_malloc(unsigned int size);

#endif // PLATFORM_H_
