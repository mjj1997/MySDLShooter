#pragma once

#include "Object.h"
#include "Scene.h"

#include <list>

class Game;

class SceneMain : public Scene
{
public:
    SceneMain();
    ~SceneMain() override = default;

    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

    void keyboardControl(float deltaTime);
    void shootPlayerBullet();
    void updatePlayerBullets(float deltaTime);
    void renderPlayerBullets();

private:
    Game& m_game;
    Player m_player;
    // 创建子弹模板
    PlayerBullet m_playerBulletTemplate;

    // 存储活动子弹的列表
    std::list<PlayerBullet*> m_playerBullets;
};
