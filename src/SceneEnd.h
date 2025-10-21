#pragma once

#include "Scene.h"

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
};
