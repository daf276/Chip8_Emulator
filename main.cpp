#include <iostream>
#include <chrono>
#include <ctime>
#include "Chip8.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <algorithm>
#include <thread>

const int SCREEN_WIDTH = 64*16;
const int SCREEN_HEIGHT = 32*16;

bool init();
void close();

// Keypad keymap
uint8_t keymap[16] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_y,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

//The window we'll be rendering to
SDL_Window *gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

int main(int argc, char const *argv[]) {
    //std::chrono::time_point<std::chrono::system_clock> start, end;
    //start = std::chrono::system_clock::now();

    //Emulator::Chip8 chip8("../PONG");

    //for (int i = 0; i < 1000; ++i) {
    //    chip8.EmulateCycle();
    //}

    //end = std::chrono::system_clock::now();
    //long elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    //std::cout << "Elapsed Milliseconds: " << elapsed_millis << std::endl;

    //Start up SDL and create window

    if (!init()) printf("Failed to initialize!\n");
    else {
        bool quit = false;

        //Event handler
        SDL_Event e;

        uint32_t pixels[64*32];

        SDL_Texture * texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 64, 32);

        Emulator::Chip8 chip8("../PONG");

        //While application is running
        while (!quit) {

            //Handle events on queue
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
                if (e.type == SDL_KEYDOWN) {
                    for (int i = 0; i < 16; ++i) {
                        if (e.key.keysym.sym == keymap[i]) {
                            chip8.key_pressed[i] = 1;
                        }
                    }
                }
                if (e.type == SDL_KEYUP) {
                    for (int i = 0; i < 16; ++i) {
                        if (e.key.keysym.sym == keymap[i]) {
                            chip8.key_pressed[i] = 0;
                        }
                    }
                }
            }

            chip8.EmulateCycle();

            std::vector<bool> gfx = chip8.GetGfx();

            for (int i = 0; i < 2048; ++i) {
                uint32_t pixel = gfx[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(int));

            SDL_RenderClear(gRenderer);

            SDL_RenderCopy(gRenderer, texture, NULL, NULL);

            SDL_RenderPresent(gRenderer);

            //Update the surface
            SDL_UpdateWindowSurface(gWindow);

            std::this_thread::sleep_for(std::chrono::microseconds(1200));
        }

        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(gRenderer);

    }

    close();

    return 0;
}

bool init() {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    } else {
        //Create window
        gWindow = SDL_CreateWindow("My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                   SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
            SDL_RenderSetScale(gRenderer, 16, 16);
        }
    }

    return true;
}

void close() {
    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = nullptr;

    //Quit SDL subsystems
    SDL_Quit();
}