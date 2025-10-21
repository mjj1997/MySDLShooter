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
    bool m_isTyping{ true };
    std::string m_playerName;
};
