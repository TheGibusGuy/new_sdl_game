#include <SDL.h>

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>

#include "mad_simulation.h"
#include "mad_input.h"
#include "mad_things.h"
#include "mad_vector.h"
#include "mad_world.h"

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>
#include <fpm/math.hpp>

#include "mikey_signature.h"

constexpr std::uint16_t WINDOW_WIDTH = 640;
constexpr std::uint16_t WINDOW_HEIGHT = 480;

enum GAME_STATES {
    MAIN_MENU,
    GAME,
    RESULTS
};

// overloaded zone
/*
void snapshot(SDL_Event &e, StateData &state_data);
void render(SDL_Renderer* &renderer, StateData &state_data);
*/
bool init(SDL_Window* &window, SDL_Renderer* &renderer);
void close(SDL_Window* &window, SDL_Renderer* &renderer, mad::Simulation& sim);

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::int8_t game_state = MAIN_MENU;
    mad::Simulation sim;

    mikey::ascii_signature();


    if (init(window,renderer)) {
        sim.advance_time( SDL_GetTicks64() );
        /*
        state_data.tick_next_snapshot = state_data.tick_current;

        SDL_Event e;

        while (!state_data.quit) {
            state_data.tick_current = SDL_GetTicks64();

            if (state_data.tick_current >= state_data.tick_next_snapshot) {
                state_data.tick_current_snapshot = state_data.tick_next_snapshot;
                snapshot(e,state_data);

                // schedule the next snapshot
                while (state_data.tick_current >= state_data.tick_next_snapshot) state_data.tick_next_snapshot += state_data.ticks_per_snapshot;
            }
            
            render(renderer,state_data);
        }
        */
    }
    close(window,renderer,sim);

    return 0;
}

/*

void render(SDL_Renderer* &renderer, StateData &state_data) {
    const static std::uint8_t SCALE = 32;
    SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
    SDL_RenderClear(renderer);

    uint64_t sub_snapshot_tick = state_data.tick_current - state_data.tick_current_snapshot;
    mad::fp lerp{ (mad::fp)sub_snapshot_tick / (mad::fp)state_data.ticks_per_snapshot };

    switch (state_data.game_state) {
    case MAIN_MENU:
        // TODO: Make a main menu
        break;
    case GAME:
        // render everything
        world_render2D(renderer, state_data.world, lerp, SCALE);

        for (std::size_t i = 0; i != state_data.thing_limit; i++) {
            if (state_data.things[i] != nullptr) {
                state_data.things[i]->render2D(renderer, lerp, SCALE);
            }
        }
        break;
    case RESULTS:
        break;
    default:
        std::printf("Invalid gamestate! Can't render!\n");
    }

    // by mistake I kept calling the clear function earlier on
    // building up the buffer without, but without rendering it
    // causing a memory leak.
    SDL_RenderPresent(renderer);
}

void set_gamestate(StateData &state_data, std::uint8_t s) {
    std::printf("Changing Gamestate to: %i\n", s);
    state_data.game_state = s;
}

void start_game(StateData &state_data) { 
    set_gamestate(state_data,GAME);
    delete_all_things(state_data);
    load_world(state_data.world, "maps/map01.txt");
    // i could use a callback function for this #1
    for (std::uint16_t x = 0; x != state_data.world.w; x++) {
        for (std::uint16_t y = 0; y != state_data.world.h; y++) {
            switch (state_data.world.tiles[x + (y * state_data.world.w)]) {
            case -1:
                break;
            case 69:
                create_thing(state_data, mad::PLAYER, { (mad::fp)x + (mad::fp)0.5,(mad::fp)y + (mad::fp)0.5,(mad::fp)0});
                break;
            default:
                break;
            }
        }
    }
}

void snapshot(SDL_Event &e, StateData &state_data) {
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            state_data.quit = true;
        }
    }

    const std::uint8_t* currentKeyStates = SDL_GetKeyboardState(nullptr);
    mad::FixedVec3D wishDir{ (mad::fp)0, (mad::fp)0, (mad::fp)0 };
    const static mad::fp MAX_SPEED{ 4 };
    const static mad::fp MAX_ACCEL{ (mad::fp)10 * MAX_SPEED };
    const static mad::fp FRICTION { 8 };
    const static mad::fp STOP_SPEED{ 0.5 };

    switch (state_data.game_state) {
    case MAIN_MENU:
        if (currentKeyStates[SDL_SCANCODE_SPACE]) {
            start_game(state_data);
        }
        break;
    case GAME:
        // simulate everything
        for (std::size_t i = 0; i != state_data.thing_limit; i++) {
            if (state_data.things[i] != nullptr) {
                switch (state_data.things[i]->type) {
                    case mad::PLAYER:
                        if (currentKeyStates[SDL_SCANCODE_D]) {
                            wishDir.x +=1 ;
                        }
                        if (currentKeyStates[SDL_SCANCODE_A]) {
                            wishDir.x -= 1;
                        }
                        if (currentKeyStates[SDL_SCANCODE_W]) {
                            wishDir.y -= 1;
                        }
                        if (currentKeyStates[SDL_SCANCODE_S]) {
                            wishDir.y += 1;
                        }

                        state_data.things[i]->vel -= state_data.things[i]->vel.scale(FRICTION * state_data.delta_time);
                        if (!wishDir.is_zero()) {
                            wishDir = wishDir.norm();

                            // I'm assuming a dot product was done back in the day instead
                            // of a magnitude check since it would be faster
                            // to squeeze out some extra performance
                            // even with my optimizations using trig functions
                            // the dot product takes far less calculations here
                            mad::fp current_speed = state_data.things[i]->vel.dot(wishDir);
                            mad::fp addSpeed = MAX_ACCEL * state_data.delta_time;
                            if (addSpeed > MAX_SPEED - current_speed) {
                                addSpeed = MAX_SPEED - current_speed;
                            }
                            state_data.things[i]->vel += wishDir.scale(addSpeed);
                        }
                        else {
                            // ill need to fix this later when accounting for vertical movement
                            if (state_data.things[i]->vel.mag() <= STOP_SPEED) {
                                state_data.things[i]->vel = { (mad::fp)0 ,(mad::fp)0 ,(mad::fp)0 };
                            }
                        }
                        
                        state_data.things[i]->move(state_data.delta_time);
                        break;
                    default:
                        break;
                }
                // std::cout << state_data.things[i]->pos.str() << '\n';
                // std::cout << state_data.things[i]->vel.mag() << '\n';
            }
        }
        break;
    case RESULTS:
        break;
    default:
        std::printf("Invalid gamestate! Can't simulate!\n");
    }
}

*/

bool init(SDL_Window* &window, SDL_Renderer* &renderer) {
    bool success = true;
    std::printf(":? Initializing\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::printf("\tSDL failed to initialze video: %s\n", SDL_GetError());
        success = false;
    }
    else {
        std::printf("\tVideo subsytem initialized %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("MADCOM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::printf("\tFailed to create window: %s\n", SDL_GetError());
        success = false;
    }
    else {
        std::printf("\tWindow created %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::printf("\tFailed to create renderer: %s\n", SDL_GetError());
        success = false;
    }
    else {
        std::printf("\tRenderer created %s\n", SDL_GetError());
    }

    if (success) {
        std::printf(":) Initialization complete\n");
    }
    else {
        std::printf(":( Initialization failed\n");
    }

    return success;
}

void close(SDL_Window* &window, SDL_Renderer* &renderer, mad::Simulation& sim) {
    std::printf(":? Closing\n");

    // already has it's own message so theres no printf here
    // destroy_world(state_data.world);

    // same ^
    // delete_all_things(state_data);

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
}