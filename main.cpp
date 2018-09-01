#include <iostream>
#include <chrono>
#include <ctime>
#include "Chip8.h"

int main(int argc, char const *argv[])
{
    //std::chrono::time_point<std::chrono::system_clock> start, end;
    //start = std::chrono::system_clock::now();

    Emulator::Chip8 chip8("../PONG");

    for (int i = 0; i < 1000; ++i) {
        chip8.EmulateCycle();
    }

    //end = std::chrono::system_clock::now();
    //long elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    //std::cout << "Elapsed Milliseconds: " << elapsed_millis << std::endl;
	return 0;
}