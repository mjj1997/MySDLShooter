#pragma once

#include "Scene.h"

#include <SDL2/SDL.h>

class Game
{
public:
    Game() = default;
    ~Game() = default;

    void init();
    void run();
    void changeScene(Scene* scene);
    void clean();

    void handleEvent(SDL_Event* event);
    void update();
    void render();

private:
    bool m_isRunning{ true };
    Scene* m_currentScene{ nullptr };
    SDL_Window* m_window{ nullptr };
    SDL_Renderer* m_renderer{ nullptr };
    int m_windowWidth{ 600 };
    int m_windowHeight{ 800 };
};
