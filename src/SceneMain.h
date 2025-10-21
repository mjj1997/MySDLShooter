#pragma once

#include "Object.h"
#include "Scene.h"

#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <list>
#include <random>
#include <unordered_map>

class SceneMain : public Scene
{
public:
    SceneMain() = default;
    ~SceneMain() override = default;

    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

private:
    // 更新相关
    void keyboardControl(float deltaTime);
    void updatePlayerBullets(float deltaTime);
    void updateEnemyBullets(float deltaTime);
    void spawnEnemy();
    void updateEnemies(float deltaTime);
    void updatePlayer(float deltaTime);
    void updateExplosions(float deltaTime);
    void updateItems(float deltaTime);
    // 渲染相关
    void renderPlayerBullets();
    void renderEnemyBullets();
    void renderEnemies();
    void renderItems();
    void renderExplosions();
    void renderUI();
    // 其它
    void shootPlayerBullet();
    void shootEnemyBullet(Enemy* enemy);
    SDL_FPoint getEnemyBulletDirection(Enemy* enemy) const;
    void explodeEnemy(Enemy* enemy);
    void dropItem(Enemy* enemy);
    void processItemPickup(Item* item);

private:
    Player m_player;
    Mix_Music* m_bgm;
    std::unordered_map<std::string, Mix_Chunk*> m_sounds;
    SDL_Texture* m_healthUI;
    TTF_Font* m_scoreFont;

    bool m_isPlayerAlive{ true };
    int m_score{ 0 };

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
