#include <SDL.h>

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>

#include "mikey_signature.h"
#include "mikey_rng.h"

constexpr std::uint16_t WINDOW_WIDTH = 640;
constexpr std::uint16_t WINDOW_HEIGHT = 480;

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    bool quit = false;

    mikey::ascii_signature();

    //initialize
    bool initialized = true;
    std::printf(":? Initializing\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("\tSDL failed to initialze video: %s\n", SDL_GetError());
        initialized = false;
    }
    else {
        std::printf("\tVideo subsytem initialized %s\n", SDL_GetError());

        window = SDL_CreateWindow("MADCOM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::printf("\tFailed to create window: %s\n", SDL_GetError());
            initialized = false;
        }
        else {
            std::printf("\tWindow created %s\n", SDL_GetError());

            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (renderer == nullptr) {
                std::printf("\tFailed to create renderer: %s\n", SDL_GetError());
                initialized = false;
            }
            else {
                std::printf("\tRenderer created %s\n", SDL_GetError());
            }
        }
    }

    if (not initialized) {
        std::printf(":( Initialization failed %s\n", SDL_GetError());
    } else {

        std::printf(":) Initialization finished %s\n", SDL_GetError());

        constexpr std::uint64_t TICK_INTERVAL = 15;
        constexpr double DT = (double)TICK_INTERVAL / (double)1000;
        // getTicks really returns milliseconds
        // to avoid naming confusion between game ticks and SDL_tick
        std::uint64_t time = 0;
        std::uint64_t nextTickTime = SDL_GetTicks64();
        std::uint64_t tick = 0;

        SDL_Event e;

        struct Thing {
            bool active = true;

            struct Vector3 {
                double x = 0;
                double y = 0;
                double z = 0;
            } position, velocity;
        };

        constexpr std::uint64_t THING_LIMIT = 32;
        Thing things[THING_LIMIT] = {};

        for (size_t i = 0; i < THING_LIMIT; i++) {
            Thing& thing = things[i];

            thing.position.x = mikey::randPercent() * (double)WINDOW_WIDTH;
            thing.position.y = mikey::randPercent() * (double)WINDOW_HEIGHT;
           
            thing.velocity.x = mikey::randPercentWide() * 64;
            thing.velocity.y = mikey::randPercentWide() * 64;
        }

        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }

            time = SDL_GetTicks64();

            if (time >= nextTickTime) {
                // schedule the next tick
                nextTickTime += (((time - nextTickTime) / TICK_INTERVAL) + 1) * TICK_INTERVAL;
                tick += 1;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

                SDL_RenderClear(renderer);

                std::uint8_t r = (time * 1) % 256;
                std::uint8_t g = (time * 2) % 256;
                std::uint8_t b = (time * 4) % 256;
                SDL_SetRenderDrawColor(renderer, r, g,  b, 255);

                // simulate this tick
                for (size_t i = 0; i < THING_LIMIT; i++) {
                    Thing& thing = things[i];

                    if (not thing.active) continue;

                    thing.position.x += thing.velocity.x * DT;
                    thing.position.y += thing.velocity.y * DT;
                    thing.position.z += thing.velocity.z * DT;

                    SDL_RenderDrawPoint(renderer, thing.position.x, thing.position.y);
                } 

                SDL_RenderPresent(renderer);
            }
        }
    }
        

    std::printf(":? Closing\n");

    // free everything from memory
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    std::printf("\tRenderer destroyed\n");

    SDL_DestroyWindow(window);
    window = nullptr;
    std::printf("\tWindow destroyed\n");

    // then quit SDL's subsystems
    SDL_Quit();
    std::printf("\tQuit SDL\n");

    std::printf(":) Closing complete\n");

    return 0;
}