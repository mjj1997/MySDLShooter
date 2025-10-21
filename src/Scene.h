#pragma once

#include <SDL2/SDL_events.h>

class Game;

class Scene
{
public:
    Scene();
    virtual ~Scene() = default;

    virtual void init() = 0;
    virtual void handleEvent(SDL_Event* event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void clean() = 0;

protected:
    Game& m_game;
};
