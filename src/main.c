#include <raylib.h>
#include <raymath.h>
#include <dirent.h>

#include "game.h"

int main(void) {
    Game game = create_new_game();
    game_run(&game);
    return 0;
}
