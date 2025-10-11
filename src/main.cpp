#include "Game.h"

int main(int, char**)
{
    Game game;
    game.init();
    game.run();
    game.clean();

    return 0;
}
