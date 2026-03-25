#include "Game.h"

int main(int, char**)
{
    Game& game{ Game::instance() };
    game.init();
    game.run();

    return 0;
}
