#include <SDL.h>

#include <iostream>
#include <cstdio>
#include <cstdint>
#include <string>
#include <sstream>
using namespace std;

#include "mad_render.h"
#include "mad_vector.h"
#include "mad_world.h"
using namespace mad;

#include <fpm/fixed.hpp>
#include <fpm/ios.hpp>
#include <fpm/math.hpp>
using namespace fpm;

// it seeems these using declarations have to go after all the include directives
using fp = fixed_16_16;
using FixedVec2D = Vector2D<fp>; 	
using FixedVec3D = Vector3D<fp>;

SDL_Renderer* gRenderer = NULL;
SDL_Window* gWindow = NULL;
SDL_Texture* gTextureList[16];

uint16_t gWindowWidth = 640;
uint16_t gWindowHeight = 480;

bool gQuit = false;

uint64_t gCurrentTick = SDL_GetTicks64();
uint64_t gNextSnapshot = gCurrentTick;

const uint8_t SNAPSHOT_TIME = 15;

enum GAME_STATES {
    MAIN_MENU,
    GAME,
    RESULTS
};
int8_t gState = MAIN_MENU;

World gWorld;

void render();

void snapshot(SDL_Event& e);

bool init();

void close();

void testing();

void ascii_signature();

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
        for (size_t x = 0; x != gWorld.w; x++) {
            for (size_t y = 0; y != gWorld.h; y++) {
                static unsigned int SCALE = 32;
                const SDL_Rect fillRect = { x * SCALE, y * SCALE, SCALE , SCALE };
                if (gWorld.tiles[x + y * gWorld.w]!=-1) {
                    SDL_SetRenderDrawColor(gRenderer, 127, 35, 76, 255);
                }
                else {
                    SDL_SetRenderDrawColor(gRenderer, 42, 64, 89, 255);
                }
                SDL_RenderFillRect(gRenderer, &fillRect);
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

void snapshot(SDL_Event &e) {
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            gQuit = true;
        }
    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

    switch (gState) {
    case MAIN_MENU:
        if (currentKeyStates[SDL_SCANCODE_SPACE]) {
            printf("Changing Gamestate\n");
            load_world(gWorld, "maps/map01.txt");
            gState = GAME;
        }
        break;
    case GAME:
        if (currentKeyStates[SDL_SCANCODE_A]) {
            printf("%llu\n", gCurrentTick);
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
    if (gWindow == NULL) {
        printf("Failed to create window: %s\n", SDL_GetError());
        success = false;
    }
    else {
        printf("Window created %s\n", SDL_GetError());
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gWindow == NULL) {
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

    // free everything from memory
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    printf("Renderer destroyed\n");

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    printf("Window destroyed\n");

    // then quit SDL's subsystems
    SDL_Quit();
    printf("Quit SDL\n");

    printf(":) Closing complete\n");
}

void testing() {
    fp x{ 413 }, y{ 612 }, z{ 1025 };
    FixedVec2D vec1{ x, y };
    FixedVec3D vec2{ x, y, z };
    printf("\n%s\n", vec1.str().c_str());
    printf("\n%f\n", (double)vec1.mag());
    printf("\n%s\n", vec1.norm().str().c_str());
    printf("\n%s\n", vec2.str().c_str());
    printf("\n%f\n", (double)vec2.mag());
    printf("\n%s\n", vec2.norm().str().c_str());
    if (load_world(gWorld, "maps/map01.txt")) {
        int z = 0;
        for (size_t i = 0; i != gWorld.tile_num; i++) {
            printf("%i ", gWorld.tiles[i]);
            z++;
        }
        cout << '\n' << z << '\n';
    }
    
    //printf("\n%i\n", gWorld.tiles[0]);
}

void ascii_signature() {
    stringstream signature;
    signature << "oooooooooooooooooodddddddddddddddddddddooooooooo\n";
    signature << "oooooooooooooooooodddddddddddooooooooooooooooooo\n";
    signature << "oooooooooooooooooooooooolooooloOKOxooooooooooooo\n";
    signature << "oooooooooooolclooooooooooll:;,;xNXoloooooooooooo\n";
    signature << "ooooooooooool,',;;::;;,,''.....;ko'':loooooooooo\n";
    signature << "ooooooooooodl,.................;kd'.';oooooooooo\n";
    signature << "ooooooooooodl,',,'.............oNNd'':oooooooooo\n";
    signature << "ooooooooooodl;,;;::;,.........'o00o:cooooooooooo\n";
    signature << "ooooooooooooo:,::::::;;;;::::::::clooooooooooooo\n";
    signature << "oooooooooooooc,;ccc:;;;;;;;;;;;;:loooooooooooooo\n";
    signature << "ooooooooooool;,;:ccccc:::::::::::::coooooooooooo\n";
    signature << "oooooooooo:;;'';:ccccccccccc:::::;'.,coooooooooo\n";
    signature << "oooooooool;'..';:cccllccccccc::;,'..,:oooooooooo\n";
    signature << "ooooooooooc,...',;;:::cccccc:,'...':oxxooooooooo\n";
    signature << "oooooooooool:'....'''',,,,,'..'''';dOkdooooooooo\n";
    signature << "oooooooool:;::;,,,'',,,,,,,'''..   'cllooooooooo\n";
    signature << "ooooooooo;.    'c;:dl;lxxc;lc..;'.'.  'ldooooooo\n";
    signature << "oooooooool'    :Okk0OxO00OO00OO0OOO; .:doooooooo\n";
    signature << "ooooooooool'  .o0OOkkkO0000000Okkkk; ;oooooooooo\n";
    signature << "oooooooooodl.  ;dc;;,,;codkkxl;,,...'ldooooooooo\n";
    signature << "ooooooooooool' .;llc::lodxxkxdlc:'.,cooooooooooo\n";
    signature << "oooooooooooooc. 'd00000xoodddkOdc:cooooooooooooo\n";
    signature << "ooooooooooooo,   .,:clolllllccc:cooooooooooooooo\n";
    signature << "ooooooooooool.              .:odoooooooooooooooo\n";
    signature << "https://www.ascii-art-generator.org/\n";
    printf("%s", signature.str().c_str());
}