#pragma once

#include "Scene.h"

class SceneMain : public Scene
{
public:
    SceneMain() = default;
    ~SceneMain() override = default;

    void init() override;
    void handleEvent(SDL_Event* event) override;
    void update() override;
    void render() override;
    void clean() override;
};
