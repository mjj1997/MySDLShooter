#include "Game.h"
#include "SceneTitle.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <fstream>
#include <sstream>

void Game::init()
{
    // SDL初始化
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL could not initialize! SDL_Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }
    // 创建窗口
    m_window = SDL_CreateWindow("SDL 太空战机", m_windowWidth, m_windowHeight, SDL_WINDOW_RESIZABLE);
    if (m_window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Window could not be created! SDL_Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }
    // 创建渲染器
    m_renderer = SDL_CreateRenderer(m_window, NULL);
    if (m_renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Renderer could not be created! SDL_Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }
    // 设置逻辑分辨率
    SDL_SetRenderLogicalPresentation(m_renderer,
                                     m_windowWidth,
                                     m_windowHeight,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // 不再需要初始化SDL_image

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
    if (!TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_ttf could not initialize! SDL_ttf Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }

    // 初始化背景
    m_nearStars.texture = IMG_LoadTexture(m_renderer, "assets/image/Stars-A.png");
    SDL_GetTextureSize(m_nearStars.texture, &m_nearStars.width, &m_nearStars.height);
    m_nearStars.width /= 2;
    m_nearStars.height /= 2;

    m_farStars.texture = IMG_LoadTexture(m_renderer, "assets/image/Stars-B.png");
    SDL_GetTextureSize(m_farStars.texture, &m_farStars.width, &m_farStars.height);
    m_farStars.width /= 2;
    m_farStars.height /= 2;
    m_farStars.speed = 20;

    // 打开字体
    m_titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    m_textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
    if (m_titleFont == nullptr || m_textFont == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_ttf could not load font! SDL_ttf Error: %s\n",
                     SDL_GetError());
        m_isRunning = false;
    }

    // 加载排行榜数据
    loadData();

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
    // 保存排行榜数据
    saveData();

    if (m_currentScene != nullptr) {
        m_currentScene->clean();
        delete m_currentScene;
    }
    // 清理背景
    if (m_nearStars.texture != nullptr)
        SDL_DestroyTexture(m_nearStars.texture);
    if (m_farStars.texture != nullptr)
        SDL_DestroyTexture(m_farStars.texture);
    // 不再需要清理SDL_image
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
        if (event->type == SDL_EVENT_QUIT) {
            m_isRunning = false;
        }

        if (event->type == SDL_EVENT_KEY_DOWN) {
            if (event->key.scancode == SDL_SCANCODE_F4) {
                m_isFullscreen = !m_isFullscreen;
                if (m_isFullscreen) {
                    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);
                } else {
                    SDL_SetWindowFullscreen(m_window, 0);
                }
            }
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

SDL_FPoint Game::renderTextCenterred(std::string_view text, float ratioY, bool isTitle)
{
    TTF_Font* font = isTitle ? m_titleFont : m_textFont;
    SDL_Color color{ 255, 255, 255, 255 };
    SDL_Surface* surface{ TTF_RenderText_Solid(font, text.data(), 0, color) };
    SDL_Texture* texture{ SDL_CreateTextureFromSurface(m_renderer, surface) };
    float posX{ (m_windowWidth - surface->w) / 2.0f };
    float posY{ ratioY * (m_windowHeight - surface->h) };
    SDL_FRect destRect{ posX, posY, static_cast<float>(surface->w), static_cast<float>(surface->h) };
    SDL_RenderTexture(m_renderer, texture, nullptr, &destRect);
    SDL_DestroySurface(surface);
    SDL_DestroyTexture(texture);

    return { destRect.x + destRect.w, destRect.y };
}

void Game::renderTextPositioned(std::string_view text, float x, float y, bool isLeftAligned)
{
    SDL_Color color{ 255, 255, 255, 255 };
    SDL_Surface* surface{ TTF_RenderText_Solid(m_textFont, text.data(), 0, color) };
    SDL_Texture* texture{ SDL_CreateTextureFromSurface(m_renderer, surface) };
    SDL_FRect destRect{ x, y, static_cast<float>(surface->w), static_cast<float>(surface->h) };
    if (isLeftAligned == false) {
        destRect.x = m_windowWidth - x - surface->w;
    }
    SDL_RenderTexture(m_renderer, texture, nullptr, &destRect);
    SDL_DestroySurface(surface);
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
    for (float posY{ m_farStars.offset }; posY < m_windowHeight; posY += m_farStars.height) {
        for (float posX{ 0 }; posX < m_windowWidth; posX += m_farStars.width) {
            SDL_FRect destRect{ posX, posY, m_farStars.width, m_farStars.height };
            SDL_RenderTexture(m_renderer, m_farStars.texture, nullptr, &destRect);
        }
    }

    // 渲染近处星空
    for (float posY{ m_nearStars.offset }; posY < m_windowHeight; posY += m_nearStars.height) {
        for (float posX{ 0 }; posX < m_windowWidth; posX += m_nearStars.width) {
            SDL_FRect destRect{ posX, posY, m_nearStars.width, m_nearStars.height };
            SDL_RenderTexture(m_renderer, m_nearStars.texture, nullptr, &destRect);
        }
    }
}

void Game::addToLeaderBoard(int score, std::string_view name)
{
    m_leaderBoard.insert({ score, std::string{ name } });
    if (m_leaderBoard.size() > 8)
        m_leaderBoard.erase(--m_leaderBoard.end());
}

void Game::saveData()
{
    std::ofstream file{ "assets/save.dat" };
    if (!file.is_open()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open save file");
        return;
    }

    for (const auto& item : m_leaderBoard) {
        file << item.first << " " << item.second << std::endl;
    }
}

void Game::loadData()
{
    std::ifstream file{ "assets/save.dat" };
    if (!file.is_open()) {
        SDL_Log("Failed to open save file");
        return;
    }

    m_leaderBoard.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss{ line };
        int score;
        if (iss >> score) {
            iss >> std::ws; // 跳过分数和名字之间的空格
            std::string name;
            std::getline(iss, name);
            m_leaderBoard.insert({ score, name });
        }
    }
}
