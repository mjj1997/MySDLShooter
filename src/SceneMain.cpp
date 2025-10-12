#include "SceneMain.h"
#include "Game.h"
#include "Object.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SceneMain::SceneMain()
    : m_game(Game::instance())
{}

void SceneMain::init()
{
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
}

void SceneMain::handleEvent(SDL_Event* event) {}

void SceneMain::update(float deltaTime)
{
    keyboardControl(deltaTime);
    updatePlayerBullets(deltaTime);
}

void SceneMain::render()
{
    // 渲染玩家子弹
    renderPlayerBullets();

    // 渲染玩家飞机
    const SDL_Rect playerRect{
        static_cast<int>(m_player.position.x),
        static_cast<int>(m_player.position.y),
        m_player.width,
        m_player.height,
    };
    SDL_RenderCopy(m_game.renderer(), m_player.texture, nullptr, &playerRect);
}

void SceneMain::clean()
{
    // 清理玩家子弹
    for (auto& playerBullet : m_playerBullets) {
        if (playerBullet != nullptr) {
            delete playerBullet;
        }
    }
    m_playerBullets.clear();

    // 清理纹理
    if (m_player.texture != nullptr) {
        SDL_DestroyTexture(m_player.texture);
    }
    if (m_playerBulletTemplate.texture != nullptr) {
        SDL_DestroyTexture(m_playerBulletTemplate.texture);
    }
}

void SceneMain::keyboardControl(float deltaTime)
{
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

void SceneMain::shootPlayerBullet()
{
    // 创建玩家子弹
    auto* playerBullet{ new PlayerBullet{ m_playerBulletTemplate } };
    // 设置玩家子弹位置在玩家飞机顶部中央
    playerBullet->position.x = m_player.position.x + m_player.width / 2 - playerBullet->width / 2;
    playerBullet->position.y = m_player.position.y;
    // 添加玩家子弹到列表
    m_playerBullets.push_back(playerBullet);
}

void SceneMain::updatePlayerBullets(float deltaTime)
{
    const int margin{ 32 }; // 子弹超出窗口边界的距离
    for (auto it{ m_playerBullets.begin() }; it != m_playerBullets.end();) {
        auto* playerBullet{ *it };
        // 更新玩家子弹位置
        playerBullet->position.y -= playerBullet->speed * deltaTime;
        // 如果玩家子弹超出窗口顶部，移除玩家子弹
        if (playerBullet->position.y + margin < 0) {
            delete playerBullet;
            it = m_playerBullets.erase(it);
        } else {
            ++it;
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
