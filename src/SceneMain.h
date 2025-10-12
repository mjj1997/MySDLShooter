#pragma once

#include "Object.h"
#include "Scene.h"

#include <list>
#include <random>

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
    void spawnEnemy();
    void updateEnemies(float deltaTime);
    void renderEnemies();

private:
    Game& m_game;
    Player m_player;
    std::mt19937 m_randomEngine;
    std::uniform_real_distribution<float> m_randomDistribution;
    // 创建子弹模板
    PlayerBullet m_playerBulletTemplate;
    // 创建敌人模板
    Enemy m_enemyTemplate;
    // 存储活动子弹的列表
    std::list<PlayerBullet*> m_playerBullets;
    // 存储活动敌人的列表
    std::list<Enemy*> m_enemies;
};
