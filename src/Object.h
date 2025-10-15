#pragma once

#include <SDL2/SDL.h>

struct Player
{
    SDL_Texture* texture{ nullptr };
    SDL_FPoint position{ 0.0f, 0.0f };
    int width{ 0 };
    int height{ 0 };
    int speed{ 300 };
    int currentHealth{ 3 };
    Uint32 coolDown{ 300 };
    Uint32 lastFireTime{ 0 };
};

struct Enemy
{
    SDL_Texture* texture{ nullptr };
    SDL_FPoint position{ 0.0f, 0.0f };
    int width{ 0 };
    int height{ 0 };
    int speed{ 150 };
    int currentHealth{ 1 };
    Uint32 coolDown{ 2000 };
    Uint32 lastFireTime{ 0 };
};

struct PlayerBullet
{
    SDL_Texture* texture{ nullptr };
    SDL_FPoint position{ 0.0f, 0.0f };
    int width{ 0 };
    int height{ 0 };
    int speed{ 600 };
    int damage{ 1 };
};

struct EnemyBullet
{
    SDL_Texture* texture{ nullptr };
    SDL_FPoint position{ 0.0f, 0.0f };
    SDL_FPoint direction{ 0.0f, 0.0f };
    int width{ 0 };
    int height{ 0 };
    int speed{ 400 };
    int damage{ 1 };
};

struct Explosion
{
    SDL_Texture* texture{ nullptr };
    SDL_FPoint position{ 0.0f, 0.0f };
    int width{ 0 };
    int height{ 0 };
    int currentFrame{ 0 };
    int totalFrames{ 0 };
    Uint32 startTime{ 0 };
    int FPS{ 10 };
};
