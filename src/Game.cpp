#include "Game.h"
#include "SceneMain.h"

#include <SDL2/SDL.h>

void Game::init()
{
    // SDL初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL could not initialize! SDL_Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }
    // 创建窗口
    m_window = SDL_CreateWindow("Hello World!",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                m_windowWidth,
                                m_windowHeight,
                                SDL_WINDOW_SHOWN);
    if (m_window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Window could not be created! SDL_Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }
    // 创建渲染器
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (m_renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Renderer could not be created! SDL_Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }

    m_currentScene = new SceneMain;
}

void Game::run()
{
    while (m_isRunning) {
        SDL_Event event;

        handleEvent(&event);

        update();

        render();
    }
}

void Game::changeScene(Scene* scene)
{
    if (m_currentScene != nullptr) {
        m_currentScene->clean();
        delete m_currentScene;
    }
    m_currentScene = scene;
    m_currentScene->init();
}

void Game::clean()
{
    if (m_currentScene != nullptr) {
        m_currentScene->clean();
        delete m_currentScene;
    }

    // 清理并退出
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Game::handleEvent(SDL_Event* event)
{
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            m_isRunning = false;
        }
        m_currentScene->handleEvent(event);
    }
}

void Game::update()
{
    m_currentScene->update();
}

void Game::render()
{
    // 清屏
    SDL_RenderClear(m_renderer);
    m_currentScene->render();
    // 更新屏幕
    SDL_RenderPresent(m_renderer);
}
