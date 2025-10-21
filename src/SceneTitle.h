#pragma once

#include "Scene.h"

#include <SDL2/SDL_mixer.h>

class SceneTitle : public Scene
{
public:
    SceneTitle() = default;
    ~SceneTitle() override = default;

    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

private:
    Mix_Music* m_bgm;
    float m_timer{ 0.0f };
};
