#include <iostream>
#include <chrono>
#include <ctime>
#include "Chip8.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <algorithm>
#include <thread>

const int SCREEN_WIDTH = 64;
const int SCREEN_HEIGHT = 32;
int SCALE = 16;
std::string filepath;

// Keypad keymap
const uint8_t keymap[16] = {SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a, SDLK_s, SDLK_d, SDLK_y,
                            SDLK_c,
                            SDLK_4, SDLK_r, SDLK_f, SDLK_v,
};

//The window we'll be rendering to
SDL_Window *gWindow = nullptr;
SDL_Renderer *gRenderer = nullptr;

bool init();

void close();

bool HandleEvents(SDL_Event *e, std::vector<bool> *key_pressed);

void UpdateGfx(std::vector<uint32_t> *gfx, std::vector<std::vector<bool>> emulator_gfx);

int main(int argc, char const *argv[]) {
    if(argc==1) {
        printf("\nYou need to specify the file path of the ROM u wish to emulate");
        return -1;
    } else if (argc > 2) {
        SCALE = std::stoi(argv[2]);
    }

    filepath = argv[1];

    std::chrono::time_point<std::chrono::system_clock> beforeCycle;
    long elapsed_millis = 0;

    if (!init()) printf("Failed to initialize!\n");
    else {
        bool quit = false;

        //Event handler
        SDL_Event e;

        std::vector<uint32_t> gfx(SCREEN_WIDTH * SCREEN_HEIGHT);

        SDL_Texture *display_texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
                                                         SCREEN_WIDTH, SCREEN_HEIGHT);

        Emulator::Chip8 chip8(filepath);

        //While application is running
        while (!quit) {
            beforeCycle = std::chrono::system_clock::now();

            //Handle Input
            quit = HandleEvents(&e, &chip8.key_pressed);

            //Handle Logic
            chip8.EmulateCycle();

            //Handle Graphics
            UpdateGfx(&gfx, chip8.GetGfx());

            SDL_UpdateTexture(display_texture, nullptr, &gfx[0], SCREEN_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(gRenderer);
            SDL_RenderCopy(gRenderer, display_texture, nullptr, nullptr);
            SDL_RenderPresent(gRenderer);
            SDL_UpdateWindowSurface(gWindow);

            elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()-beforeCycle).count();
            std::this_thread::sleep_for(std::chrono::microseconds(1000-elapsed_millis*1000));
        }

        SDL_DestroyTexture(display_texture);
    }

    close();

    return 0;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    } else {
        gWindow = SDL_CreateWindow("My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*SCALE,
                                   SCREEN_HEIGHT*SCALE, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
            SDL_RenderSetScale(gRenderer, SCALE, SCALE);
        }
    }

    return true;
}

bool HandleEvents(SDL_Event *e, std::vector<bool> *key_pressed) {
    //Handle events on queue
    while (SDL_PollEvent(e) != 0) {
        //Exit if quit event is triggered
        if (e->type == SDL_QUIT) {
            return true;
        }

        if (e->type == SDL_KEYDOWN) {
            for (int i = 0; i < 16; ++i) {
                if (e->key.keysym.sym == keymap[i]) {
                    key_pressed->at(i) = true;
                }
            }
        }

        if (e->type == SDL_KEYUP) {
            for (int i = 0; i < 16; ++i) {
                if (e->key.keysym.sym == keymap[i]) {
                    key_pressed->at(i) = false;
                }
            }
        }
    }

    return false;
}

void UpdateGfx(std::vector<uint32_t> *gfx, std::vector<std::vector<bool>> emulator_gfx) {
    for (int i = 0; i < SCREEN_HEIGHT; ++i) {
        for (int j = 0; j < SCREEN_WIDTH; ++j) {
            uint32_t pixel = emulator_gfx[i][j];
            gfx->at(i * SCREEN_WIDTH + j) =
                    (0x00FFFFFF * pixel) | 0xFF000000; //To convert the emulator gfx to a uint we can use
        }
    }
}

void close() {
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;

    SDL_Quit();
}