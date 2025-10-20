#include "SceneMain.h"
#include "Game.h"
#include "Object.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <numbers>
#include <random>

SceneMain::SceneMain()
    : m_game(Game::instance())
{}

void SceneMain::init()
{
    // 加载并播放背景音乐
    m_bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if (m_bgm == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load background music: %s", Mix_GetError());
    }
    Mix_PlayMusic(m_bgm, -1);

    // 加载音效
    m_sounds["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    m_sounds["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    m_sounds["player_explode"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    m_sounds["enemy_explode"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    m_sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
    m_sounds["get_item"] = Mix_LoadWAV("assets/sound/eff5.wav");

    // 初始化随机数生成器
    std::random_device randomDevice;
    m_randomEngine = std::mt19937{ randomDevice() };
    m_randomDistribution = std::uniform_real_distribution<float>{ 0.0f, 1.0f };

    // 加载玩家飞机的生命值 UI 纹理
    m_healthUI = IMG_LoadTexture(m_game.renderer(), "assets/image/Health UI Black.png");
    if (m_healthUI == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load health UI texture: %s", SDL_GetError());
    }

    // 初始化玩家飞机
    m_player.texture = IMG_LoadTexture(m_game.renderer(), "assets/image/SpaceShip.png");
    if (m_player.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load player texture: %s", SDL_GetError());
    }
    SDL_QueryTexture(m_player.texture, nullptr, nullptr, &m_player.width, &m_player.height);
    m_player.width /= 4;
    m_player.height /= 4;
    m_player.position.x = m_game.windowWidth() / 2 - m_player.width / 2;
    m_player.position.y = m_game.windowHeight() - m_player.height;

    // 初始化玩家子弹模板
    m_playerBulletTemplate.texture = IMG_LoadTexture(m_game.renderer(), "assets/image/laser-1.png");
    if (m_playerBulletTemplate.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to load player bullet texture: %s",
                     SDL_GetError());
    }
    SDL_QueryTexture(m_playerBulletTemplate.texture,
                     nullptr,
                     nullptr,
                     &m_playerBulletTemplate.width,
                     &m_playerBulletTemplate.height);
    m_playerBulletTemplate.width /= 4;
    m_playerBulletTemplate.height /= 4;

    // 初始化敌人模板
    m_enemyTemplate.texture = IMG_LoadTexture(m_game.renderer(), "assets/image/insect-2.png");
    if (m_enemyTemplate.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load enemy texture: %s", SDL_GetError());
    }
    SDL_QueryTexture(m_enemyTemplate.texture,
                     nullptr,
                     nullptr,
                     &m_enemyTemplate.width,
                     &m_enemyTemplate.height);
    m_enemyTemplate.width /= 4;
    m_enemyTemplate.height /= 4;

    // 初始化敌人子弹模板
    m_enemyBulletTemplate.texture = IMG_LoadTexture(m_game.renderer(), "assets/image/bullet-1.png");
    if (m_enemyBulletTemplate.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to load enemy bullet texture: %s",
                     SDL_GetError());
    }
    SDL_QueryTexture(m_enemyBulletTemplate.texture,
                     nullptr,
                     nullptr,
                     &m_enemyBulletTemplate.width,
                     &m_enemyBulletTemplate.height);
    m_enemyBulletTemplate.width /= 4;
    m_enemyBulletTemplate.height /= 4;

    // 初始化爆炸模板
    m_explosionTemplate.texture = IMG_LoadTexture(m_game.renderer(), "assets/effect/explosion.png");
    if (m_explosionTemplate.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load explosion texture: %s", SDL_GetError());
    }
    SDL_QueryTexture(m_explosionTemplate.texture,
                     nullptr,
                     nullptr,
                     &m_explosionTemplate.width,
                     &m_explosionTemplate.height);
    m_explosionTemplate.totalFrames = m_explosionTemplate.width / m_explosionTemplate.height;
    m_explosionTemplate.width = m_explosionTemplate.height;

    // 初始化物品模板
    m_itemLifeTemplate.texture = IMG_LoadTexture(m_game.renderer(), "assets/image/bonus_life.png");
    if (m_itemLifeTemplate.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load item life texture: %s", SDL_GetError());
    }
    SDL_QueryTexture(m_itemLifeTemplate.texture,
                     nullptr,
                     nullptr,
                     &m_itemLifeTemplate.width,
                     &m_itemLifeTemplate.height);
    m_itemLifeTemplate.width /= 4;
    m_itemLifeTemplate.height /= 4;
}

void SceneMain::handleEvent(SDL_Event* event) {}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    updatePlayerBullets(deltaTime);
    updateEnemyBullets(deltaTime);
    spawnEnemy();
    updateEnemies(deltaTime);
    updatePlayer(deltaTime);
    updateExplosions(deltaTime);
    updateItems(deltaTime);
}

void SceneMain::render()
{
    // 渲染玩家子弹
    renderPlayerBullets();
    // 渲染敌人子弹
    renderEnemyBullets();

    // 渲染玩家飞机
    if (m_isPlayerAlive) {
        const SDL_Rect playerRect{
            static_cast<int>(m_player.position.x),
            static_cast<int>(m_player.position.y),
            m_player.width,
            m_player.height,
        };
        SDL_RenderCopy(m_game.renderer(), m_player.texture, nullptr, &playerRect);
    }

    // 渲染敌人
    renderEnemies();
    // 渲染物品
    renderItems();
    // 渲染爆炸
    renderExplosions();
    // 渲染 UI
    renderUI();
}

void SceneMain::clean()
{
    // 清理音效
    for (auto& sound : m_sounds) {
        if (sound.second != nullptr) {
            Mix_FreeChunk(sound.second);
        }
    }
    m_sounds.clear();

    // 清理玩家子弹
    for (auto& playerBullet : m_playerBullets) {
        if (playerBullet != nullptr) {
            delete playerBullet;
        }
    }
    m_playerBullets.clear();

    // 清理敌人
    for (auto& enemy : m_enemies) {
        if (enemy != nullptr) {
            delete enemy;
        }
    }
    m_enemies.clear();

    // 清理敌人子弹
    for (auto& enemyBullet : m_enemyBullets) {
        if (enemyBullet != nullptr) {
            delete enemyBullet;
        }
    }
    m_enemyBullets.clear();

    // 清理爆炸
    for (auto& explosion : m_explosions) {
        if (explosion != nullptr) {
            delete explosion;
        }
    }
    m_explosions.clear();

    // 清理物品
    for (auto& item : m_items) {
        if (item != nullptr) {
            delete item;
        }
    }
    m_items.clear();

    // 清理纹理
    if (m_player.texture != nullptr) {
        SDL_DestroyTexture(m_player.texture);
    }
    if (m_playerBulletTemplate.texture != nullptr) {
        SDL_DestroyTexture(m_playerBulletTemplate.texture);
    }
    if (m_enemyTemplate.texture != nullptr) {
        SDL_DestroyTexture(m_enemyTemplate.texture);
    }
    if (m_enemyBulletTemplate.texture != nullptr) {
        SDL_DestroyTexture(m_enemyBulletTemplate.texture);
    }
    if (m_explosionTemplate.texture != nullptr) {
        SDL_DestroyTexture(m_explosionTemplate.texture);
    }
    if (m_itemLifeTemplate.texture != nullptr) {
        SDL_DestroyTexture(m_itemLifeTemplate.texture);
    }
    if (m_healthUI != nullptr) {
        SDL_DestroyTexture(m_healthUI);
    }

    // 清理背景音乐
    if (m_bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(m_bgm);
    }
}

void SceneMain::keyboardControl(float deltaTime)
{
    if (!m_isPlayerAlive)
        return;

    const auto* keyboardState{ SDL_GetKeyboardState(nullptr) };
    if (keyboardState[SDL_SCANCODE_W]) {
        m_player.position.y -= m_player.speed * deltaTime;
    }
    if (keyboardState[SDL_SCANCODE_S]) {
        m_player.position.y += m_player.speed * deltaTime;
    }
    if (keyboardState[SDL_SCANCODE_A]) {
        m_player.position.x -= m_player.speed * deltaTime;
    }
    if (keyboardState[SDL_SCANCODE_D]) {
        m_player.position.x += m_player.speed * deltaTime;
    }

    // 限制玩家位置在窗口内
    if (m_player.position.x < 0) {
        m_player.position.x = 0;
    }
    if (m_player.position.x > m_game.windowWidth() - m_player.width) {
        m_player.position.x = m_game.windowWidth() - m_player.width;
    }
    if (m_player.position.y < 0) {
        m_player.position.y = 0;
    }
    if (m_player.position.y > m_game.windowHeight() - m_player.height) {
        m_player.position.y = m_game.windowHeight() - m_player.height;
    }

    // 控制子弹发射
    if (keyboardState[SDL_SCANCODE_J]) {
        auto currentTime{ SDL_GetTicks() };
        if (currentTime - m_player.lastFireTime > m_player.coolDown) {
            shootPlayerBullet();
            m_player.lastFireTime = currentTime;
        }
    }
}

void SceneMain::updatePlayer(float deltaTime)
{
    if (!m_isPlayerAlive) {
        return;
    }
    if (m_player.currentHealth <= 0) {
        const auto currentTime{ SDL_GetTicks() };
        m_isPlayerAlive = false;
        auto* explosion{ new Explosion{ m_explosionTemplate } };
        explosion->position.x = m_player.position.x + m_player.width / 2 - explosion->width / 2;
        explosion->position.y = m_player.position.y + m_player.height / 2 - explosion->height / 2;
        explosion->startTime = currentTime;
        m_explosions.push_back(explosion);
        // 播放玩家爆炸音效
        Mix_PlayChannel(-1, m_sounds["player_explode"], 0);
        return;
    }
    for (auto* enemy : m_enemies) {
        const SDL_Rect enemyRect{
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height,
        };
        const SDL_Rect playerRect{
            static_cast<int>(m_player.position.x),
            static_cast<int>(m_player.position.y),
            m_player.width,
            m_player.height,
        };
        if (SDL_HasIntersection(&enemyRect, &playerRect)) {
            m_player.currentHealth -= 1;
            enemy->currentHealth = 0;
        }
    }
}

void SceneMain::shootPlayerBullet()
{
    // 创建玩家子弹
    auto* playerBullet{ new PlayerBullet{ m_playerBulletTemplate } };
    // 设置玩家子弹位置在玩家飞机顶部中央
    playerBullet->position.x = m_player.position.x + m_player.width / 2 - playerBullet->width / 2;
    playerBullet->position.y = m_player.position.y;
    // 添加玩家子弹到列表
    m_playerBullets.push_back(playerBullet);
    // 播放玩家射击音效
    Mix_PlayChannel(0, m_sounds["player_shoot"], 0);
}

void SceneMain::updatePlayerBullets(float deltaTime)
{
    const int margin{ 32 }; // 子弹超出窗口边界的距离
    for (auto it{ m_playerBullets.begin() }; it != m_playerBullets.end();) {
        auto* playerBullet{ *it };
        // 更新玩家子弹位置
        playerBullet->position.y -= playerBullet->speed * deltaTime;
        // 如果玩家子弹超出窗口顶部，移除玩家子弹
        if (playerBullet->position.y < -margin) {
            delete playerBullet;
            it = m_playerBullets.erase(it);
        } else {
            // 检查玩家子弹是否击中敌人
            bool hit{ false };
            for (auto* enemy : m_enemies) {
                const SDL_Rect enemyRect{
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height,
                };
                const SDL_Rect playerBulletRect{
                    static_cast<int>(playerBullet->position.x),
                    static_cast<int>(playerBullet->position.y),
                    playerBullet->width,
                    playerBullet->height,
                };
                if (SDL_HasIntersection(&enemyRect, &playerBulletRect)) {
                    enemy->currentHealth -= playerBullet->damage;
                    delete playerBullet;
                    it = m_playerBullets.erase(it);
                    hit = true;
                    // 播放击中音效
                    Mix_PlayChannel(-1, m_sounds["hit"], 0);
                    break;
                }
            }
            if (!hit) {
                ++it;
            }
        }
    }
}

void SceneMain::renderPlayerBullets()
{
    for (auto* playerBullet : m_playerBullets) {
        const SDL_Rect playerBulletRect{
            static_cast<int>(playerBullet->position.x),
            static_cast<int>(playerBullet->position.y),
            playerBullet->width,
            playerBullet->height,
        };
        SDL_RenderCopy(m_game.renderer(), playerBullet->texture, nullptr, &playerBulletRect);
    }
}

void SceneMain::spawnEnemy()
{
    // 通过随机数决定是否生成敌人 (每帧 1/60 概率生成)
    if (m_randomDistribution(m_randomEngine) > 1 / 60.0f)
        return;

    // 创建敌人
    auto* enemy{ new Enemy{ m_enemyTemplate } };
    // 设置敌人位置在窗口随机位置
    enemy->position.x = m_randomDistribution(m_randomEngine)
                        * (m_game.windowWidth() - enemy->width);
    enemy->position.y = -enemy->height;
    // 添加敌人到列表
    m_enemies.push_back(enemy);
}

void SceneMain::updateEnemies(float deltaTime)
{
    auto currentTime{ SDL_GetTicks() };
    for (auto it{ m_enemies.begin() }; it != m_enemies.end();) {
        auto* enemy{ *it };
        // 更新敌人位置
        enemy->position.y += enemy->speed * deltaTime;
        // 如果敌人超出窗口底部，移除敌人
        if (enemy->position.y > m_game.windowHeight()) {
            delete enemy;
            it = m_enemies.erase(it);
        } else {
            // 控制敌人子弹发射
            if (currentTime - enemy->lastFireTime > enemy->coolDown && m_isPlayerAlive != false) {
                shootEnemyBullet(enemy);
                enemy->lastFireTime = currentTime;
            }
            // 如果敌人生命值小于等于0，爆炸敌人并移除敌人
            if (enemy->currentHealth <= 0) {
                explodeEnemy(enemy);
                if (m_randomDistribution(m_randomEngine) > 0.5f) {
                    dropItem(enemy);
                }
                delete enemy;
                it = m_enemies.erase(it);
                // 播放敌人爆炸音效
                Mix_PlayChannel(-1, m_sounds["enemy_explode"], 0);
            } else {
                ++it;
            }
        }
    }
}

void SceneMain::renderEnemies()
{
    for (auto* enemy : m_enemies) {
        const SDL_Rect enemyRect{
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height,
        };
        SDL_RenderCopy(m_game.renderer(), enemy->texture, nullptr, &enemyRect);
    }
}

void SceneMain::shootEnemyBullet(Enemy* enemy)
{
    // 创建敌人子弹
    auto* enemyBullet{ new EnemyBullet{ m_enemyBulletTemplate } };
    // 设置敌人子弹位置在敌人飞机中心
    enemyBullet->position.x = enemy->position.x + enemy->width / 2 - enemyBullet->width / 2;
    enemyBullet->position.y = enemy->position.y + enemy->height / 2 - enemyBullet->height / 2;
    // 设置敌人子弹方向
    enemyBullet->direction = getEnemyBulletDirection(enemy);
    // 添加敌人子弹到列表
    m_enemyBullets.push_back(enemyBullet);
    // 播放敌人射击音效
    Mix_PlayChannel(-1, m_sounds["enemy_shoot"], 0);
}

void SceneMain::updateEnemyBullets(float deltaTime)
{
    const int margin{ 32 }; // 子弹超出窗口边界的距离
    for (auto it{ m_enemyBullets.begin() }; it != m_enemyBullets.end();) {
        auto* enemyBullet{ *it };
        // 更新敌人子弹位置
        enemyBullet->position.x += enemyBullet->direction.x * enemyBullet->speed * deltaTime;
        enemyBullet->position.y += enemyBullet->direction.y * enemyBullet->speed * deltaTime;
        // 如果敌人子弹超出窗口边界，移除敌人子弹
        if (enemyBullet->position.x < -margin
            || enemyBullet->position.x > m_game.windowWidth() + margin
            || enemyBullet->position.y < -margin
            || enemyBullet->position.y > m_game.windowHeight() + margin) {
            delete enemyBullet;
            it = m_enemyBullets.erase(it);
        } else {
            // 检查敌人子弹是否击中玩家
            const SDL_Rect playerRect{
                static_cast<int>(m_player.position.x),
                static_cast<int>(m_player.position.y),
                m_player.width,
                m_player.height,
            };
            const SDL_Rect enemyBulletRect{
                static_cast<int>(enemyBullet->position.x),
                static_cast<int>(enemyBullet->position.y),
                enemyBullet->width,
                enemyBullet->height,
            };
            if (SDL_HasIntersection(&playerRect, &enemyBulletRect) && m_isPlayerAlive != false) {
                m_player.currentHealth -= enemyBullet->damage;
                delete enemyBullet;
                it = m_enemyBullets.erase(it);
                // 播放击中音效
                Mix_PlayChannel(-1, m_sounds["hit"], 0);
            } else {
                ++it;
            }
        }
    }
}

void SceneMain::renderEnemyBullets()
{
    for (auto* enemyBullet : m_enemyBullets) {
        const SDL_Rect enemyBulletRect{
            static_cast<int>(enemyBullet->position.x),
            static_cast<int>(enemyBullet->position.y),
            enemyBullet->width,
            enemyBullet->height,
        };
        auto angle{ SDL_atan2(enemyBullet->direction.y, enemyBullet->direction.x)
                        * (180 / std::numbers::pi)
                    - 90 };
        SDL_RenderCopyEx(m_game.renderer(),
                         enemyBullet->texture,
                         nullptr,
                         &enemyBulletRect,
                         angle,
                         nullptr,
                         SDL_FLIP_NONE);
    }
}

SDL_FPoint SceneMain::getEnemyBulletDirection(Enemy* enemy) const
{
    // 计算敌人子弹方向向量
    SDL_FPoint direction{
        (m_player.position.x + m_player.width / 2) - (enemy->position.x + enemy->width / 2),
        (m_player.position.y + m_player.height / 2) - (enemy->position.y + enemy->height / 2),
    };
    // 归一化方向向量
    float length{ SDL_sqrtf(direction.x * direction.x + direction.y * direction.y) };
    direction.x /= length;
    direction.y /= length;

    return direction;
}

void SceneMain::explodeEnemy(Enemy* enemy)
{
    const auto currentTime{ SDL_GetTicks() };
    auto* explosion{ new Explosion{ m_explosionTemplate } };
    explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
    explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    m_explosions.push_back(explosion);
}

void SceneMain::updateExplosions(float deltaTime)
{
    const auto currentTime{ SDL_GetTicks() };
    for (auto it{ m_explosions.begin() }; it != m_explosions.end();) {
        auto* explosion{ *it };
        // 更新爆炸当前帧
        explosion->currentFrame = explosion->FPS * (currentTime - explosion->startTime) / 1000;
        // 如果爆炸当前帧超过总帧数，移除爆炸
        if (explosion->currentFrame >= explosion->totalFrames) {
            delete explosion;
            it = m_explosions.erase(it);
        } else {
            ++it;
        }
    }
}

void SceneMain::renderExplosions()
{
    for (auto* explosion : m_explosions) {
        const SDL_Rect srcRect{
            explosion->currentFrame * explosion->width,
            0,
            explosion->width,
            explosion->height,
        };
        const SDL_Rect dstRect{
            static_cast<int>(explosion->position.x),
            static_cast<int>(explosion->position.y),
            explosion->width,
            explosion->height,
        };
        SDL_RenderCopy(m_game.renderer(), explosion->texture, &srcRect, &dstRect);
    }
}

void SceneMain::dropItem(Enemy* enemy)
{
    auto* item{ new Item{ m_itemLifeTemplate } };
    item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
    item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;

    // 生成物品随机方向
    float angle{ static_cast<float>(m_randomDistribution(m_randomEngine) * 2 * std::numbers::pi) };
    item->direction.x = SDL_cosf(angle);
    item->direction.y = SDL_sinf(angle);

    m_items.push_back(item);
}

void SceneMain::updateItems(float deltaTime)
{
    for (auto it{ m_items.begin() }; it != m_items.end();) {
        auto* item{ *it };
        item->position.x += item->direction.x * item->speed * deltaTime;
        item->position.y += item->direction.y * item->speed * deltaTime;

        // 如果物品到达窗口边界，反弹
        if (item->bounceCount > 0) {
            // 标记是否已经反弹过，避免一次碰撞多次减少反弹次数
            bool bounced{ false };
            if (item->position.x < 0) {
                item->direction.x *= -1;
                bounced = true;
            }
            if (item->position.x + item->width > m_game.windowWidth()) {
                item->direction.x *= -1;
                bounced = true;
            }
            if (item->position.y < 0) {
                item->direction.y *= -1;
                bounced = true;
            }
            if (item->position.y + item->height > m_game.windowHeight()) {
                item->direction.y *= -1;
                bounced = true;
            }

            // 只在发生了反弹时减少一次反弹次数
            if (bounced) {
                --item->bounceCount;
            }
        }

        if (item->position.x + item->width < 0 || item->position.y + item->height < 0
            || item->position.x > m_game.windowWidth()
            || item->position.y > m_game.windowHeight()) {
            delete item;
            it = m_items.erase(it);
        } else {
            const SDL_Rect itemRect{
                static_cast<int>(item->position.x),
                static_cast<int>(item->position.y),
                item->width,
                item->height,
            };
            const SDL_Rect playerRect{
                static_cast<int>(m_player.position.x),
                static_cast<int>(m_player.position.y),
                m_player.width,
                m_player.height,
            };
            if (SDL_HasIntersection(&itemRect, &playerRect) && m_isPlayerAlive == true) {
                processItemPickup(item);
                delete item;
                it = m_items.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void SceneMain::renderItems()
{
    for (auto* item : m_items) {
        const SDL_Rect itemRect{
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height,
        };
        SDL_RenderCopy(m_game.renderer(), item->texture, nullptr, &itemRect);
    }
}

void SceneMain::processItemPickup(Item* item)
{
    switch (item->type) {
    case ItemType::Life:
        ++m_player.currentHealth;
        if (m_player.currentHealth > m_player.maxHealth) {
            m_player.currentHealth = m_player.maxHealth;
        }
        // 播放拾取物品音效
        Mix_PlayChannel(-1, m_sounds["get_item"], 0);
        break;
    default:
        break;
    }
}

void SceneMain::renderUI()
{
    int x{ 10 };
    int y{ 10 };
    int size{ 32 };
    int offset{ 40 };

    SDL_SetTextureColorMod(m_healthUI, 128, 128, 128); // 使颜色变淡
    for (int i{ 0 }; i < m_player.maxHealth; ++i) {
        SDL_Rect rect{ x + i * offset, y, size, size };
        SDL_RenderCopy(m_game.renderer(), m_healthUI, nullptr, &rect);
    }

    SDL_SetTextureColorMod(m_healthUI, 255, 255, 255); // 恢复颜色
    for (int i{ 0 }; i < m_player.currentHealth; ++i) {
        SDL_Rect rect{ x + i * offset, y, size, size };
        SDL_RenderCopy(m_game.renderer(), m_healthUI, nullptr, &rect);
    }
}
