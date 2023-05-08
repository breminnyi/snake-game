#ifndef GAME_H_
#define GAME_H_

void game_init(int width, int height, int rand_seed);
void game_handle_key(int key);
int game_update(int dt);

#endif // GAME_H_
