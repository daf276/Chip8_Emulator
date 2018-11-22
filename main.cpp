#include <iostream>
#include <chrono>
#include <ctime>
#include "Chip8.h"
#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 64*16;
const int SCREEN_HEIGHT = 32*16;

bool init();
void close();

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

        SDL_Texture * texture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, 64, 32);

        auto * pixels = new int[64*32];
        memset(pixels, 0, 64*32* sizeof(int));

        Emulator::Chip8 chip8("../PONG");

        //While application is running
        while (!quit) {

            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(int));

            //Handle events on queue
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                } else if (e.type == SDL_KEYDOWN) {

                }
            }

            chip8.EmulateCycle();

            SDL_RenderClear(gRenderer);

            SDL_RenderCopy(gRenderer, texture, NULL, NULL);

            SDL_RenderPresent(gRenderer);

            //Update the surface
            SDL_UpdateWindowSurface(gWindow);
        }

        delete[] pixels;
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