#include "SceneTitle.h"
#include "Game.h"
#include "SceneMain.h"

#include <SDL3/SDL.h>

void SceneTitle::init()
{
    // 加载并播放背景音乐
    m_bgm = MIX_LoadAudio(m_game.mixer(), "assets/music/06_Battle_in_Space_Intro.ogg", false);
    if (m_bgm == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load background music: %s", SDL_GetError());
    }
    m_bgmTrack = MIX_CreateTrack(m_game.mixer());
    MIX_SetTrackAudio(m_bgmTrack, m_bgm);
    MIX_SetTrackGain(m_bgmTrack, 2.0f);
    MIX_SetTrackLoops(m_bgmTrack, -1);
    MIX_PlayTrack(m_bgmTrack, 0);
}

void SceneTitle::handleEvent(SDL_Event* event)
{
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.scancode == SDL_SCANCODE_J) {
            m_game.changeScene(new SceneMain);
        }
    }
}

void SceneTitle::update(float deltaTime)
{
    m_timer += deltaTime;
    if (m_timer > 1.0f) {
        m_timer = 0.0f;
    }
}

void SceneTitle::render()
{
    // 渲染标题
    m_game.renderTextCenterred("SDL 太空战机", 0.4f, true);
    // 渲染开始游戏提示
    if (m_timer < 0.5f)
        m_game.renderTextCenterred("按 J 键开始游戏", 0.8f, false);
}

void SceneTitle::clean()
{
    // 清理背景音乐
    if (m_bgmTrack != nullptr) {
        MIX_StopTrack(m_bgmTrack, 0);
        MIX_DestroyTrack(m_bgmTrack);
    }
    if (m_bgm != nullptr) {
        MIX_DestroyAudio(m_bgm);
    }
}
