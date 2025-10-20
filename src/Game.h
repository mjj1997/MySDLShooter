#pragma once

#include "Object.h"
#include "Scene.h"

#include <SDL2/SDL.h>

class Game
{
public:
    static Game& instance()
    {
        static Game s_instance;
        return s_instance;
    }
    ~Game() = default;

    void init();
    void run();
    void changeScene(Scene* scene);
    void clean();

    void handleEvent(SDL_Event* event);
    void update(float deltaTime);
    void render();

    void updateBackground(float deltaTime);
    void renderBackground();

    SDL_Window* window() { return m_window; }
    SDL_Renderer* renderer() { return m_renderer; }
    int windowWidth() const { return m_windowWidth; }
    int windowHeight() const { return m_windowHeight; }

private:
    Game() = default;
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool m_isRunning{ true };
    Scene* m_currentScene{ nullptr };
    SDL_Window* m_window{ nullptr };
    SDL_Renderer* m_renderer{ nullptr };
    int m_windowWidth{ 600 };
    int m_windowHeight{ 800 };
    int m_FPS{ 60 };
    Uint32 m_frameTime{ static_cast<Uint32>(1000) / m_FPS };
    float m_deltaTime{ 0.0f };

    Background m_nearStars;
    Background m_farStars;
};
