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

SDL_Renderer* g_renderer = nullptr;
SDL_Window* g_window = nullptr;
SDL_Texture* g_textureList[16];

uint16_t g_windowWidth = 640;
uint16_t g_windowHeight = 480;

bool gQuit = false;

uint64_t gCurrentTick = SDL_GetTicks64();
uint64_t gNextSnapshot = gCurrentTick;

enum GAME_STATES {
    MAIN_MENU,
    GAME,
    RESULTS
};

int8_t gState = MAIN_MENU;

uint8_t gMenuSelection = 0;
bool gMenuOpen = false;

World g_world;
Thing* g_things[THING_LIMIT];

// this a wrapper function for the Thing classes' constructor
// it checks for free space before adding an item
int16_t create_thing(int16_t t, FixedVec3D spawnPos) {
    printf("Creating thing\n");
    for (uint16_t i = 0; i != THING_LIMIT; i++) {
        if (g_things[i] == nullptr) {
            g_things[i] = new Thing(t, spawnPos);
            return i;
        }
    }
    printf("Could not create thing! No space!");
    return -1;
}

void delete_thing(size_t i) {
    if (g_things[i] == nullptr) {
        printf("Nothing here to delete - ");
    }
    else {
        printf("Deleting thing - ");
        delete g_things[i];
    }
    printf("Setting pointer to nullptr\n");
    g_things[i] = nullptr;
}

void delete_all_things() {
    for (size_t i = 0; i != THING_LIMIT; i++) {
        delete_thing(i);
    }
    printf("All things deleted and pointers set to nullptr\n");
}

// overloaded zone
void snapshot(SDL_Event& e);
void render();
bool init();
void close();

int main(int argc, char* args[]) {
    ascii_signature();

    gCurrentTick = SDL_GetTicks64();
    gNextSnapshot = gCurrentTick;

    if (init()) {
        gCurrentTick = SDL_GetTicks64();
        gNextSnapshot = gCurrentTick;

        SDL_Event e;

        while (!gQuit) {
            gCurrentTick = SDL_GetTicks64();

            if (gCurrentTick >= gNextSnapshot) {
                while (gCurrentTick >= gNextSnapshot) gNextSnapshot += SNAPSHOT_TICKS;
                snapshot(e);
            }
            
            render();
        }
    }
    close();

    return 0;
}

void render() {
    const static int SCALE = 32;
    SDL_SetRenderDrawColor(g_renderer, 191, 191, 191, 255);
    SDL_RenderClear(g_renderer);
    switch (gState) {
    case MAIN_MENU:
        break;
    case GAME:
        // i could use a callback function for this #1
        for (uint16_t x = 0; x != g_world.w; x++) {
            for (uint16_t y = 0; y != g_world.h; y++) {
                const SDL_Rect fillRect = { x * SCALE, y * SCALE, SCALE , SCALE };
                switch (g_world.tiles[x + (y * g_world.w)]) {
                    case -1:
                        SDL_SetRenderDrawColor(g_renderer, 127, 35, 76, 255);
                        break;
                    case 69:
                        SDL_SetRenderDrawColor(g_renderer, 69, 69, 69, 69);
                        break;
                    default:
                        SDL_SetRenderDrawColor(g_renderer, 42, 64, 89, 255);
                        break;
                }
                SDL_RenderFillRect(g_renderer, &fillRect);
            }
        }
        // render everything
        for (size_t i = 0; i != THING_LIMIT; i++) {
            if (g_things[i] != nullptr) {
                SDL_Rect objRect;
                const static int THING_SCALE = SCALE / 4;
                const static int HALF_THING_WIDTH = THING_SCALE / 2;

                SDL_SetRenderDrawColor(g_renderer, 0, 0, 255, 255);
                const int32_t posX = (int32_t)(g_things[i]->pos.x * fp(SCALE)) - HALF_THING_WIDTH;
                const int32_t posY = (int32_t)(g_things[i]->pos.y * fp(SCALE)) - HALF_THING_WIDTH;
                objRect = { posX,posY, THING_SCALE , THING_SCALE };
                SDL_RenderFillRect(g_renderer, &objRect);

                SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 255);
                const int32_t prev_posX = (int32_t)(g_things[i]->prev_pos.x * fp(SCALE)) - HALF_THING_WIDTH;
                const int32_t prev_posY = (int32_t)(g_things[i]->prev_pos.y * fp(SCALE)) - HALF_THING_WIDTH;
                objRect = { prev_posX,prev_posY, THING_SCALE , THING_SCALE };
                SDL_RenderFillRect(g_renderer, &objRect);


                // i think theres a more efficient way to do this calculating the sub time
                // but this is more intuitive to me
                uint64_t subSnapshotTick = SNAPSHOT_TICKS - (gNextSnapshot - gCurrentTick);
                fp lerp{ (fp)subSnapshotTick/(fp)SNAPSHOT_TICKS};

                FixedVec3D subVec = g_things[i]->pos - g_things[i]->prev_pos;
                subVec = subVec.scale(lerp);
                FixedVec3D interpPos = g_things[i]->prev_pos + subVec;
    
                SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 255);
                const int32_t interpX = (int32_t)(interpPos.x * fp(SCALE)) - HALF_THING_WIDTH;
                const int32_t interpY = (int32_t)(interpPos.y * fp(SCALE)) - HALF_THING_WIDTH;
                objRect = { interpX,interpY, THING_SCALE , THING_SCALE };
                SDL_RenderFillRect(g_renderer, &objRect);
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
    SDL_RenderPresent(g_renderer);
}

void set_gamestate(uint8_t s) {
    printf("Changing Gamestate\n");
    gState = s;
}

void start_game() {
    set_gamestate(GAME);
    delete_all_things();
    load_world(g_world, "maps/map01.txt");
    // i could use a callback function for this #1
    for (uint16_t x = 0; x != g_world.w; x++) {
        for (uint16_t y = 0; y != g_world.h; y++) {
            switch (g_world.tiles[x + (y * g_world.w)]) {
            case -1:
                break;
            case 69:
                create_thing(PLAYER, { (fp)x + (fp)0.5,(fp)y + (fp)0.5,(fp)0});
                break;
            default:
                break;
            }
        }
    }
    // i could use a callback function for this #2
}

void snapshot(SDL_Event &e) {
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            gQuit = true;
        }
    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    FixedVec3D wishDir{ (fp)0, (fp)0, (fp)0 };
    const static fp MAX_SPEED{ 4 };
    const static fp MAX_ACCEL{ (fp)10 * MAX_SPEED };
    const static fp FRICTION { 8 };
    const static fp STOP_SPEED{ 0.5 };

    switch (gState) {
    case MAIN_MENU:
        if (currentKeyStates[SDL_SCANCODE_SPACE]) {
            start_game();
        }
        break;
    case GAME:
        // simulate everything
        for (size_t i = 0; i != THING_LIMIT; i++) {
            if (g_things[i] != nullptr) {
                switch (g_things[i]->type) {
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

                        g_things[i]->vel -= g_things[i]->vel.scale(FRICTION * DELTA);
                        if (!wishDir.is_zero()) {
                            wishDir = wishDir.norm();

                            // I'm assuming a dot product was done back in the day instead
                            // of a magnitude check since it would be faster
                            // to squeeze out some extra performance
                            // even with my optimizations using trig functions
                            // the dot product takes far less calculations here
                            fp currentSpeed = g_things[i]->vel.dot(wishDir);
                            fp addSpeed = MAX_ACCEL * DELTA;
                            if (addSpeed > MAX_SPEED - currentSpeed) {
                                addSpeed = MAX_SPEED - currentSpeed;
                            }
                            g_things[i]->vel += wishDir.scale(addSpeed);
                        }
                        else {
                            // ill need to fix this later when accounting for vertical movement
                            if (g_things[i]->vel.mag() <= STOP_SPEED) {
                                g_things[i]->vel = { (fp)0 ,(fp)0 ,(fp)0 };
                            }
                        }
                        
                        g_things[i]->move();
                        break;
                    default:
                        break;
                }
                // std::cout << g_things[i]->pos.str() << '\n';
                // std::cout << g_things[i]->vel.mag() << '\n';
            }
        }
        break;
    case RESULTS:
        break;
    default:
        printf("Invalid gamestate! Can't simulate!\n");
    }
}

bool init() {
    bool success = true;
    printf(":? Initializing\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL failed to initialze video: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("Video subsytem initialized\n");
    }

    g_window = SDL_CreateWindow("MADCOM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_windowWidth, g_windowHeight, SDL_WINDOW_SHOWN);
    if (g_window == nullptr) {
        printf("Failed to create window: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("Window created %s\n", SDL_GetError());
    }

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (g_window == nullptr) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("Renderer created %s\n", SDL_GetError());
    }

    if (success) {
        printf(":) Initialization complete\n");
    }
    else {
        printf(":( Initialization failed\n");
    }

    return success;
}

void close() {
    printf(":? Closing\n");

    // already has it's own message so theres no printf here
    destroy_world(g_world);

    // same ^
    delete_all_things();

    // free everything from memory
    SDL_DestroyRenderer(g_renderer);
    g_renderer = nullptr;
    printf("Renderer destroyed\n");

    SDL_DestroyWindow(g_window);
    g_window = nullptr;
    printf("Window destroyed\n");

    // then quit SDL's subsystems
    SDL_Quit();
    printf("Quit SDL\n");

    printf(":) Closing complete\n");
}