#include "Game.h"

int main(int, char**)
{
    Game& game{ Game::instance() };
    game.init();
    game.run();
    game.clean();

    return 0;
}
