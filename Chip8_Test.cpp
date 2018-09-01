//
// Created by attila on 01.09.18.
//

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "Chip8.h"

TEST_CASE("JUMP") {
    Emulator::Chip8 parenttest("");

    parenttest.WriteToMemory(0x200, 0x18);
    parenttest.WriteToMemory(0x201, 0x54);

    parenttest.EmulateCycle();

    REQUIRE(0x0854 == parenttest.GetProgramCounter());
}
