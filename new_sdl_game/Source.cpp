#include <SDL.h>

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
using namespace std;

#include "mad_things.h"
#include "mad_vector.h"
#include "mad_world.h"
using namespace mad;

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>
#include <fpm/math.hpp>
using namespace fpm;

#include "mikey_signature.h"
using namespace mikey;

constexpr uint16_t WINDOW_WIDTH = 640;
constexpr uint16_t WINDOW_HEIGHT = 480;

enum GAME_STATES {
    MAIN_MENU,
    GAME,
    RESULTS
};

struct StateData {
    uint64_t current_tick = 0;
    uint64_t tick_of_current_snapshot = 0;
    uint64_t tick_of_next_snapshot = 0;
    int8_t game_state = MAIN_MENU;
    bool quit = false;
    World world;
    Thing* things[THING_LIMIT] = {};
};

// this a wrapper function for the Thing classes' constructor
// it checks for free space before adding an item
int16_t create_thing(StateData& state_data, int16_t t, FixedVec3D spawn_pos) {
    std::printf("\tCreating thing\n");
    for (uint16_t i = 0; i != THING_LIMIT; i++) {
        if (state_data.things[i] == nullptr) {
            state_data.things[i] = new Thing(t, spawn_pos);
            return i;
        }
    }
    std::printf("\tCould not create thing! No space!");
    return -1;
}

void delete_thing(StateData& state_data, size_t i) {
    if (state_data.things[i] == nullptr) {
        std::printf("\tNothing here to delete - ");
    }
    else {
        std::printf("\tDeleting thing - ");
        delete state_data.things[i];
    }
    std::printf("Setting pointer to nullptr\n");
    state_data.things[i] = nullptr;
}

void delete_all_things(StateData& state_data) {
    std::printf("Deleting all things:\n");
    for (size_t i = 0; i != THING_LIMIT; i++) {
        delete_thing(state_data, i);
    }
    std::printf("All things deleted and pointers set to nullptr\n");
}

// overloaded zone
void snapshot(SDL_Event &e, StateData &state_data);
void render(SDL_Renderer* &renderer, StateData &state_data);
bool init(SDL_Window* &window, SDL_Renderer* &renderer);
void close(SDL_Window* &window, SDL_Renderer* &renderer, StateData& state_data);

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    StateData state_data;

    ascii_signature();

    if (init(window,renderer)) {
        state_data.tick_of_next_snapshot = state_data.current_tick;

        SDL_Event e;

        while (!state_data.quit) {
            state_data.current_tick = SDL_GetTicks64();

            if (state_data.current_tick >= state_data.tick_of_next_snapshot) {
                state_data.tick_of_current_snapshot = state_data.tick_of_next_snapshot;
                snapshot(e,state_data);

                // schedule the next snapshot
                while (state_data.current_tick >= state_data.tick_of_next_snapshot) state_data.tick_of_next_snapshot += TICKS_BETWEEN_SNAPSHOTS;
            }
            
            render(renderer,state_data);
        }
    }
    close(window,renderer,state_data);

    return 0;
}

void render(SDL_Renderer* &renderer, StateData &state_data) {
    const static uint8_t SCALE = 32;
    SDL_SetRenderDrawColor(renderer, 191, 191, 191, 255);
    SDL_RenderClear(renderer);

    uint64_t sub_snapshot_tick = state_data.current_tick - state_data.tick_of_current_snapshot;
    fp lerp{ (fp)sub_snapshot_tick / (fp)TICKS_BETWEEN_SNAPSHOTS };

    switch (state_data.game_state) {
    case MAIN_MENU:
        // TODO: Make a main menu
        break;
    case GAME:
        // render everything
        world_render2D(renderer, state_data.world, lerp, SCALE);

        for (size_t i = 0; i != THING_LIMIT; i++) {
            if (state_data.things[i] != nullptr) {
                state_data.things[i]->render2D(renderer, lerp, SCALE);
            }
        }
        break;
    case RESULTS:
        break;
    default:
        printf("Invalid gamestate! Can't render!\n");
    }

    // by mistake I kept calling the clear function earlier on
    // building up the buffer without, but without rendering it
    // causing a memory leak.
    SDL_RenderPresent(renderer);
}

void set_gamestate(StateData &state_data, uint8_t s) {
    printf("Changing Gamestate to: %i\n", s);
    state_data.game_state = s;
}

void start_game(StateData &state_data) { 
    set_gamestate(state_data,GAME);
    delete_all_things(state_data);
    load_world(state_data.world, "maps/map01.txt");
    // i could use a callback function for this #1
    for (uint16_t x = 0; x != state_data.world.w; x++) {
        for (uint16_t y = 0; y != state_data.world.h; y++) {
            switch (state_data.world.tiles[x + (y * state_data.world.w)]) {
            case -1:
                break;
            case 69:
                create_thing(state_data, PLAYER, { (fp)x + (fp)0.5,(fp)y + (fp)0.5,(fp)0});
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

    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    FixedVec3D wishDir{ (fp)0, (fp)0, (fp)0 };
    const static fp MAX_SPEED{ 4 };
    const static fp MAX_ACCEL{ (fp)10 * MAX_SPEED };
    const static fp FRICTION { 8 };
    const static fp STOP_SPEED{ 0.5 };

    switch (state_data.game_state) {
    case MAIN_MENU:
        if (currentKeyStates[SDL_SCANCODE_SPACE]) {
            start_game(state_data);
        }
        break;
    case GAME:
        // simulate everything
        for (size_t i = 0; i != THING_LIMIT; i++) {
            if (state_data.things[i] != nullptr) {
                switch (state_data.things[i]->type) {
                    case PLAYER:
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

                        state_data.things[i]->vel -= state_data.things[i]->vel.scale(FRICTION * DELTA);
                        if (!wishDir.is_zero()) {
                            wishDir = wishDir.norm();

                            // I'm assuming a dot product was done back in the day instead
                            // of a magnitude check since it would be faster
                            // to squeeze out some extra performance
                            // even with my optimizations using trig functions
                            // the dot product takes far less calculations here
                            fp current_speed = state_data.things[i]->vel.dot(wishDir);
                            fp addSpeed = MAX_ACCEL * DELTA;
                            if (addSpeed > MAX_SPEED - current_speed) {
                                addSpeed = MAX_SPEED - current_speed;
                            }
                            state_data.things[i]->vel += wishDir.scale(addSpeed);
                        }
                        else {
                            // ill need to fix this later when accounting for vertical movement
                            if (state_data.things[i]->vel.mag() <= STOP_SPEED) {
                                state_data.things[i]->vel = { (fp)0 ,(fp)0 ,(fp)0 };
                            }
                        }
                        
                        state_data.things[i]->move();
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
        printf("Invalid gamestate! Can't simulate!\n");
    }
}

bool init(SDL_Window* &window, SDL_Renderer* &renderer) {
    bool success = true;
    printf(":? Initializing\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("\tSDL failed to initialze video: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("\tVideo subsytem initialized %s\n", SDL_GetError());
    }

    window = SDL_CreateWindow("MADCOM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        printf("\tFailed to create window: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("\tWindow created %s\n", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        printf("\tFailed to create renderer: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("\tRenderer created %s\n", SDL_GetError());
    }

    if (success) {
        printf(":) Initialization complete\n");
    }
    else {
        printf(":( Initialization failed\n");
    }

    return success;
}

void close(SDL_Window* &window, SDL_Renderer* &renderer, StateData& state_data) {
    printf(":? Closing\n");

    // already has it's own message so theres no printf here
    destroy_world(state_data.world);

    // same ^
    delete_all_things(state_data);

    // free everything from memory
    SDL_DestroyRenderer(renderer);
    renderer = nullptr;
    printf("\tRenderer destroyed\n");

    SDL_DestroyWindow(window);
    window = nullptr;
    printf("\tWindow destroyed\n");

    // then quit SDL's subsystems
    SDL_Quit();
    printf("\tQuit SDL\n");

    printf(":) Closing complete\n");
}