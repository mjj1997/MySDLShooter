#pragma once

#include "Object.h"
#include "Scene.h"

#include <SDL2/SDL_mixer.h>

#include <list>
#include <random>
#include <unordered_map>

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
    void updatePlayer(float deltaTime);
    void shootPlayerBullet();
    void updatePlayerBullets(float deltaTime);
    void renderPlayerBullets();
    void spawnEnemy();
    void updateEnemies(float deltaTime);
    void renderEnemies();
    void shootEnemyBullet(Enemy* enemy);
    void updateEnemyBullets(float deltaTime);
    void renderEnemyBullets();
    SDL_FPoint getEnemyBulletDirection(Enemy* enemy) const;
    void explodeEnemy(Enemy* enemy);
    void updateExplosions(float deltaTime);
    void renderExplosions();
    void dropItem(Enemy* enemy);
    void updateItems(float deltaTime);
    void renderItems();
    void processItemPickup(Item* item);

private:
    Game& m_game;
    Player m_player;
    Mix_Music* m_bgm;
    std::unordered_map<std::string, Mix_Chunk*> m_sounds;

    bool m_isPlayerAlive{ true };
    std::mt19937 m_randomEngine;
    std::uniform_real_distribution<float> m_randomDistribution;
    // 创建所有 Object 的模板
    PlayerBullet m_playerBulletTemplate;
    Enemy m_enemyTemplate;
    EnemyBullet m_enemyBulletTemplate;
    Explosion m_explosionTemplate;
    Item m_itemLifeTemplate;

    // 存储所有活动 Object 的列表
    std::list<PlayerBullet*> m_playerBullets;
    std::list<Enemy*> m_enemies;
    std::list<EnemyBullet*> m_enemyBullets;
    std::list<Explosion*> m_explosions;
    std::list<Item*> m_items;
};
