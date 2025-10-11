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
    m_player.texture = IMG_LoadTexture(m_game.renderer(), "assets/image/SpaceShip.png");
    if (m_player.texture == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load player texture: %s", SDL_GetError());
    }
    SDL_QueryTexture(m_player.texture, nullptr, nullptr, &m_player.width, &m_player.height);
    m_player.width /= 4;
    m_player.height /= 4;
    m_player.position.x = m_game.windowWidth() / 2 - m_player.width / 2;
    m_player.position.y = m_game.windowHeight() - m_player.height;
}

void SceneMain::handleEvent(SDL_Event* event) {}

void SceneMain::update()
{
    keyboardControl();
}

void SceneMain::render()
{
    SDL_Rect playerRect{
        static_cast<int>(m_player.position.x),
        static_cast<int>(m_player.position.y),
        m_player.width,
        m_player.height,
    };
    SDL_RenderCopy(m_game.renderer(), m_player.texture, nullptr, &playerRect);
}

void SceneMain::clean() {}

void SceneMain::keyboardControl()
{
    auto keyboardState{ SDL_GetKeyboardState(nullptr) };
    if (keyboardState[SDL_SCANCODE_W]) {
        m_player.position.y -= 1.0f;
    }
    if (keyboardState[SDL_SCANCODE_S]) {
        m_player.position.y += 1.0f;
    }
    if (keyboardState[SDL_SCANCODE_A]) {
        m_player.position.x -= 1.0f;
    }
    if (keyboardState[SDL_SCANCODE_D]) {
        m_player.position.x += 1.0f;
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
}
