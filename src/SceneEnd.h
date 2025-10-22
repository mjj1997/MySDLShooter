#pragma once

#include "Scene.h"

#include <string>

class SceneEnd : public Scene
{
public:
    SceneEnd() = default;
    ~SceneEnd() override = default;

    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

private:
    void removeLastUTF8Char(std::string& str);
    void renderNameInputUi();
    void renderLeaderBoard();

    bool m_isTyping{ true };
    std::string m_playerName;
    float m_timer{ 0.0f };
};
