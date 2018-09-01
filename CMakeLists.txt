cmake_minimum_required(VERSION 3.10)
project(Chip8_Emulator_C++)

set(CMAKE_CXX_STANDARD 17)

add_executable(Chip8 main.cpp Chip8.cpp Chip8.h)