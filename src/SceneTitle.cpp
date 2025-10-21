#include "SceneTitle.h"
#include "Game.h"

#include <SDL2/SDL.h>

void SceneTitle::init()
{
    // 加载并播放背景音乐
    m_bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (m_bgm == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load background music: %s", Mix_GetError());
    }
    Mix_PlayMusic(m_bgm, -1);
}

void SceneTitle::render()
{
    // 渲染标题
    m_game.renderTextCenterred("SDL 太空战机", 0.4f, true);
    // 渲染开始游戏提示
        m_game.renderTextCenterred("按 J 键开始游戏", 0.8f, false);
}

void SceneTitle::clean()
{
    // 清理背景音乐
    if (m_bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(m_bgm);
    }
}
