#include "Game.h"
#include "SceneTitle.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

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
    m_window = SDL_CreateWindow("SDL 太空战机",
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
    // 初始化SDL_image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_image could not initialize! SDL_image Error: %s\n",
                     IMG_GetError());
        m_isRunning = false;
    }
    // 初始化SDL_mixer
    if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                     Mix_GetError());
        m_isRunning = false;
    }
    // 打开音频设备
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_mixer could not initialize! SDL_mixer Error: %s\n",
                     Mix_GetError());
        m_isRunning = false;
    }
    // 设置音效channel数量
    Mix_AllocateChannels(32);
    // 初始化字体
    if (TTF_Init() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                     TTF_GetError());
        m_isRunning = false;
    }

    // 初始化背景
    m_nearStars.texture = IMG_LoadTexture(m_renderer, "assets/image/Stars-A.png");
    SDL_QueryTexture(m_nearStars.texture, nullptr, nullptr, &m_nearStars.width, &m_nearStars.height);
    m_nearStars.width /= 2;
    m_nearStars.height /= 2;

    m_farStars.texture = IMG_LoadTexture(m_renderer, "assets/image/Stars-B.png");
    SDL_QueryTexture(m_farStars.texture, nullptr, nullptr, &m_farStars.width, &m_farStars.height);
    m_farStars.width /= 2;
    m_farStars.height /= 2;
    m_farStars.speed = 20;

    // 打开字体
    m_titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    m_textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
    if (m_titleFont == nullptr || m_textFont == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_ttf could not load font! SDL_ttf Error: %s\n",
                     TTF_GetError());
        m_isRunning = false;
    }

    m_currentScene = new SceneTitle;
    m_currentScene->init();
}

void Game::run()
{
    while (m_isRunning) {
        auto frameStart{ SDL_GetTicks() };
        SDL_Event event;
        handleEvent(&event);
        update(m_deltaTime);
        render();
        auto frameEnd{ SDL_GetTicks() };
        auto frameTime{ frameEnd - frameStart };

        if (frameTime < m_frameTime) {
            SDL_Delay(m_frameTime - frameTime);
            m_deltaTime = m_frameTime / 1000.0f;
        } else {
            m_deltaTime = frameTime / 1000.0f;
        }
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
    // 清理背景
    if (m_nearStars.texture != nullptr)
        SDL_DestroyTexture(m_nearStars.texture);
    if (m_farStars.texture != nullptr)
        SDL_DestroyTexture(m_farStars.texture);
    // 清理SDL_image
    IMG_Quit();
    // 清理SDL_mixer
    Mix_CloseAudio();
    Mix_Quit();
    // 清理字体
    if (m_titleFont != nullptr) {
        TTF_CloseFont(m_titleFont);
    }
    if (m_textFont != nullptr) {
        TTF_CloseFont(m_textFont);
    }
    TTF_Quit();
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

void Game::update(float deltaTime)
{
    updateBackground(deltaTime);
    m_currentScene->update(deltaTime);
}

void Game::render()
{
    // 清屏
    SDL_RenderClear(m_renderer);
    // 渲染背景
    renderBackground();
    m_currentScene->render();
    // 更新屏幕
    SDL_RenderPresent(m_renderer);
}

void Game::renderTextCenterred(std::string_view text, float ratioY, bool isTitle)
{
    TTF_Font* font = isTitle ? m_titleFont : m_textFont;
    SDL_Color color{ 255, 255, 255, 255 };
    SDL_Surface* surface{ TTF_RenderUTF8_Solid(font, text.data(), color) };
    SDL_Texture* texture{ SDL_CreateTextureFromSurface(m_renderer, surface) };
    int posX{ (m_windowWidth - surface->w) / 2 };
    int posY{ static_cast<int>(ratioY * (m_windowHeight - surface->h)) };
    SDL_Rect destRect{ posX, posY, surface->w, surface->h };
    SDL_RenderCopy(m_renderer, texture, nullptr, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

}

void Game::renderTextPositioned(std::string_view text, int x, int y)
{
    SDL_Color color{ 255, 255, 255, 255 };
    SDL_Surface* surface{ TTF_RenderUTF8_Solid(m_textFont, text.data(), color) };
    SDL_Texture* texture{ SDL_CreateTextureFromSurface(m_renderer, surface) };
    SDL_Rect destRect{ x, y, surface->w, surface->h };
    SDL_RenderCopy(m_renderer, texture, nullptr, &destRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::updateBackground(float deltaTime)
{
    m_nearStars.offset += m_nearStars.speed * deltaTime;
    // 近处星空的 Y 坐标初始值为负的纹理高度，当 Y 坐标为 0 时，重置 offset
    if (m_nearStars.offset >= 0)
        m_nearStars.offset -= m_nearStars.height;

    m_farStars.offset += m_farStars.speed * deltaTime;
    // 远处星空的 Y 坐标初始值为负的纹理高度，当 Y 坐标为 0 时，重置 offset
    if (m_farStars.offset >= 0)
        m_farStars.offset -= m_farStars.height;
}

void Game::renderBackground()
{
    // 渲染远处星空
    for (int posY{ static_cast<int>(m_farStars.offset) }; posY < m_windowHeight;
         posY += m_farStars.height) {
        for (int posX{ 0 }; posX < m_windowWidth; posX += m_farStars.width) {
            SDL_Rect destRect{ posX, posY, m_farStars.width, m_farStars.height };
            SDL_RenderCopy(m_renderer, m_farStars.texture, nullptr, &destRect);
        }
    }

    // 渲染近处星空
    for (int posY{ static_cast<int>(m_nearStars.offset) }; posY < m_windowHeight;
         posY += m_nearStars.height) {
        for (int posX{ 0 }; posX < m_windowWidth; posX += m_nearStars.width) {
            SDL_Rect destRect{ posX, posY, m_nearStars.width, m_nearStars.height };
            SDL_RenderCopy(m_renderer, m_nearStars.texture, nullptr, &destRect);
        }
    }
}
