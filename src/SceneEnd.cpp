#include "SceneEnd.h"
#include "Game.h"

#include <string>

void SceneEnd::init() {}

void SceneEnd::handleEvent(SDL_Event* event) {}

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
}

void SceneEnd::clean() {}
