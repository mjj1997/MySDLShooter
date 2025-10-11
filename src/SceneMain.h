#pragma once

#include "Object.h"
#include "Scene.h"

class Game;

class SceneMain : public Scene
{
public:
    SceneMain();
    ~SceneMain() override = default;

    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update() override;
    void render() override;
    void clean() override;

private:
    Game& m_game;
    Player m_player;
};
