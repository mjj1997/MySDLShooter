#pragma once

#include "Object.h"
#include "Scene.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string_view>

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

    // 渲染居中文本帮助函数
    SDL_Point renderTextCenterred(std::string_view text, float ratioY, bool isTitle);
    void renderTextPositioned(std::string_view text, int x, int y);

    void updateBackground(float deltaTime);
    void renderBackground();

    SDL_Window* window() { return m_window; }
    SDL_Renderer* renderer() { return m_renderer; }
    int windowWidth() const { return m_windowWidth; }
    int windowHeight() const { return m_windowHeight; }

    int finalScore() const { return m_finalScore; }
    void setFinalScore(int score) { m_finalScore = score; }

private:
    Game() = default;
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    TTF_Font* m_titleFont;
    TTF_Font* m_textFont;

    bool m_isRunning{ true };
    Scene* m_currentScene{ nullptr };
    SDL_Window* m_window{ nullptr };
    SDL_Renderer* m_renderer{ nullptr };
    int m_windowWidth{ 600 };
    int m_windowHeight{ 800 };
    int m_FPS{ 60 };
    Uint32 m_frameTime{ static_cast<Uint32>(1000) / m_FPS };
    float m_deltaTime{ 0.0f };
    int m_finalScore{ 0 };

    Background m_nearStars;
    Background m_farStars;
};
