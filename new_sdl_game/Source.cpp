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

SDL_Renderer* gRenderer = nullptr;
SDL_Window* gWindow = nullptr;
SDL_Texture* gTextureList[16];

uint16_t gWindowWidth = 640;
uint16_t gWindowHeight = 480;

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

World gWorld;
Thing* gThings[THING_LIMIT];

// this a wrapper function for the Thing classes' constructor
// it checks for free space before adding an item
int16_t create_thing(int16_t t, FixedVec3D spawnPos) {
    printf("Creating thing\n");
    for (uint16_t i = 0; i != THING_LIMIT; i++) {
        if (gThings[i] == nullptr) {
            gThings[i] = new Thing(t, spawnPos);
            return i;
        }
    }
    printf("Could not create thing! No space!");
    return -1;
}

void delete_thing(size_t i) {
    if (gThings[i] == nullptr) {
        printf("Nothing here to delete - ");
    }
    else {
        printf("Deleting thing - ");
        delete gThings[i];
    }
    printf("Setting pointer to nullptr\n");
    gThings[i] = nullptr;
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
                while (gCurrentTick >= gNextSnapshot) gNextSnapshot += SNAPSHOT_TIME;
                snapshot(e);
            }
            
            render();
        }
    }
    close();

    return 0;
}

void render() {
    SDL_SetRenderDrawColor(gRenderer, 191, 191, 191, 255);
    SDL_RenderClear(gRenderer);
    switch (gState) {
    case MAIN_MENU:
        break;
    case GAME:
        // i could use a callback function for this #1
        for (uint16_t x = 0; x != gWorld.w; x++) {
            for (uint16_t y = 0; y != gWorld.h; y++) {
                static int SCALE = 32;
                const SDL_Rect fillRect = { x * SCALE, y * SCALE, SCALE , SCALE };
                switch (gWorld.tiles[x + (y * gWorld.w)]) {
                    case -1:
                        SDL_SetRenderDrawColor(gRenderer, 127, 35, 76, 255);
                        break;
                    case 69:
                        SDL_SetRenderDrawColor(gRenderer, 69, 69, 69, 69);
                        break;
                    default:
                        SDL_SetRenderDrawColor(gRenderer, 42, 64, 89, 255);
                        break;
                }
                SDL_RenderFillRect(gRenderer, &fillRect);
            }
        }
        // render everything
        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        for (size_t i = 0; i != THING_LIMIT; i++) {
            if (gThings[i] != nullptr) {
                SDL_RenderDrawPoint(gRenderer,((int32_t)(gThings[i]->pos.x*fp(16))), (int32_t)(gThings[i]->pos.y * fp(16)));
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
    SDL_RenderPresent(gRenderer);
}

void set_gamestate(uint8_t s) {
    printf("Changing Gamestate\n");
    gState = s;
}

void start_game() {
    set_gamestate(GAME);
    delete_all_things();
    load_world(gWorld, "maps/map01.txt");
    // i could use a callback function for this #1
    for (uint16_t x = 0; x != gWorld.w; x++) {
        for (uint16_t y = 0; y != gWorld.h; y++) {
            switch (gWorld.tiles[x + (y * gWorld.w)]) {
            case -1:
                break;
            case 69:
                create_thing(PLAYER, { (fp)x,(fp)y,(fp)0 });
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

    switch (gState) {
    case MAIN_MENU:
        if (currentKeyStates[SDL_SCANCODE_SPACE]) {
            start_game();
        }
        break;
    case GAME:
        // simulate everything
        for (size_t i = 0; i != THING_LIMIT; i++) {
            if (gThings[i] != nullptr) {
                switch (gThings[i]->type) {
                    case PLAYER:
                        gThings[i]->vel.x = (fp)0;
                        gThings[i]->vel.y = (fp)0;
                        gThings[i]->vel.z = (fp)0;
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
                        if (!wishDir.is_zero()) {
                            wishDir = wishDir.norm();
                            gThings[i]->vel += wishDir.scale((fp)400 * DELTA);
                        }
                        gThings[i]->move();
                        break;
                    default:
                        break;
                }
                cout << gThings[i]->pos.str() << '\n';
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

    gWindow = SDL_CreateWindow("MADCOM", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gWindowWidth, gWindowHeight, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        printf("Failed to create window: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("Window created %s\n", SDL_GetError());
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gWindow == nullptr) {
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
    destroy_world(gWorld);

    // same ^
    delete_all_things();

    // free everything from memory
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;
    printf("Renderer destroyed\n");

    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    printf("Window destroyed\n");

    // then quit SDL's subsystems
    SDL_Quit();
    printf("Quit SDL\n");

    printf(":) Closing complete\n");
}