#ifndef PLATFORM_H_
#define PLATFORM_H_

void platform_draw_cell(int x, int y, int cell_type);
void platform_render(void);
void* platform_malloc(unsigned int size);

#endif // PLATFORM_H_