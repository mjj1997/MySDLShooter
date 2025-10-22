#include "SceneEnd.h"
#include "Game.h"
#include "SceneMain.h"

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
                if (m_playerName.empty())
                    m_playerName = "Jane Doe";
                m_game.addToLeaderBoard(m_game.finalScore(), m_playerName);
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                if (!m_playerName.empty()) {
                    removeLastUTF8Char(m_playerName);
                }
            }
        }
    } else {
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.scancode == SDL_SCANCODE_J) {
                m_game.changeScene(new SceneMain);
            }
        }
    }
}

void SceneEnd::update(float deltaTime)
{
    m_timer += deltaTime;
    if (m_timer >= 1.0f) {
        m_timer = 0.0f;
    }
}

void SceneEnd::render()
{
    if (m_isTyping == true) {
        renderNameInputUi();
    } else {
        renderLeaderBoard();
    }
}

void SceneEnd::clean()
{
    if (SDL_IsTextInputActive()) {
        SDL_StopTextInput();
    }
}

void SceneEnd::removeLastUTF8Char(std::string& str)
{
    auto lastChar{ str.back() };
    if ((lastChar & 0b10000000) == 0b10000000) { // 检查是否为多字节字符的后续字节
        str.pop_back();
        while ((str.back() & 0b11000000) != 0b11000000) { // 继续弹出直到遇到起始字节
            str.pop_back();
        }
    }
    // 弹出 ASCII 字符
    str.pop_back();
}

void SceneEnd::renderNameInputUi()
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
        SDL_Point cornerUpRigt{ m_game.renderTextCenterred(m_playerName, 0.8f, false) };
        if (m_timer < 0.5f) {
            m_game.renderTextPositioned("_", cornerUpRigt.x, cornerUpRigt.y);
        }
    } else {
        if (m_timer < 0.5f) {
            m_game.renderTextCenterred("_", 0.8f, false);
        }
    }
}

void SceneEnd::renderLeaderBoard()
{
    // 渲染排行榜标题
    m_game.renderTextCenterred("排行榜", 0.05f, true);

    // 渲染排行榜
    auto posY{ 0.2 * m_game.windowHeight() };
    auto i{ 1 };
    for (const auto& item : m_game.leaderBoard()) {
        std::string name{ std::to_string(i) + ". " + item.second };
        std::string score{ std::to_string(item.first) };
        m_game.renderTextPositioned(name, 100, posY);
        m_game.renderTextPositioned(score, 100, posY, false);
        posY += 45;
        ++i;
    }

    // 渲染重新开始游戏提示
    if (m_timer < 0.5f) {
        m_game.renderTextCenterred("按 J 键重新开始游戏", 0.85f, false);
    }
}
