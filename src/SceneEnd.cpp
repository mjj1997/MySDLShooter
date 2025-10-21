#include "SceneEnd.h"
#include "Game.h"

#include <string>

void SceneEnd::init()
{
    if (!SDL_IsTextInputActive()) {
        SDL_StartTextInput();
        if (!SDL_IsTextInputActive()) {
            SDL_LogError(SDL_LOG_PRIORITY_ERROR,
                         "SDL",
                         "Failed to start text input: %s",
                         SDL_GetError());
        }
    }
}

void SceneEnd::handleEvent(SDL_Event* event)
{
    if (m_isTyping == true) {
        if (event->type == SDL_TEXTINPUT) {
            m_playerName += event->text.text;
        }
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
                m_isTyping = false;
                SDL_StopTextInput();
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                if (!m_playerName.empty()) {
                    m_playerName.pop_back();
                }
            }
        }
    }
}

void SceneEnd::update(float deltaTime) {}

void SceneEnd::render()
{
    std::string scoreText{ "你的最终得分：" + std::to_string(m_game.finalScore()) };
    // 渲染最终得分
    m_game.renderTextCenterred(scoreText, 0.1f, false);
    // 渲染游戏结束文本
    m_game.renderTextCenterred("Game Over", 0.4f, true);
    // 渲染输入提示文本
    m_game.renderTextCenterred("请输入你的名字，按回车键确认：", 0.6f, false);
    // 渲染玩家输入的名字
    if (!m_playerName.empty()) {
        m_game.renderTextCenterred(m_playerName, 0.8f, false);
    }
}

void SceneEnd::clean()
{
    if (SDL_IsTextInputActive()) {
        SDL_StopTextInput();
    }
}
