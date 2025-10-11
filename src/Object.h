#pragma once

#include <SDL2/SDL.h>

struct Player
{
    SDL_Texture* texture{ nullptr };
    SDL_FPoint position{ 0.0f, 0.0f };
    int width{ 0 };
    int height{ 0 };
    int speed{ 200 };
};
