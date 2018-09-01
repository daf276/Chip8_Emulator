//
// Created by attila on 01.09.18.
//

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "Chip8.h"

TEST_CASE("Call subroutine") {
    Emulator::Chip8 parenttest("Unit Test, supress error");

    parenttest.WriteToMemory(0x200, 0x28);
    parenttest.WriteToMemory(0x201, 0x54);

    parenttest.EmulateCycle();

    REQUIRE(0x0854 == parenttest.GetProgramCounter());
    REQUIRE(0x0200 == parenttest.GetStack(0));
    REQUIRE(1 == parenttest.GetStackPointer());
}

TEST_CASE("JUMP") {
    Emulator::Chip8 parenttest("Unit Test, supress error");

    parenttest.WriteToMemory(0x200, 0x18);
    parenttest.WriteToMemory(0x201, 0x54);

    parenttest.EmulateCycle();

    REQUIRE(0x0854 == parenttest.GetProgramCounter());
}

TEST_CASE("Skip instruction if register equals number") {
    Emulator::Chip8 parenttest("Unit Test, supress error");

    parenttest.WriteToMemory(0x200, 0x38);
    parenttest.WriteToMemory(0x201, 0x54);

    parenttest.SetCpuRegister(8, 0x54);

    parenttest.EmulateCycle();

    REQUIRE(0x0204 == parenttest.GetProgramCounter());

    Emulator::Chip8 parenttest2("Unit Test, supress error");

    parenttest2.WriteToMemory(0x200, 0x38);
    parenttest2.WriteToMemory(0x201, 0x54);

    parenttest2.SetCpuRegister(8, 0x64);

    parenttest2.EmulateCycle();

    REQUIRE(0x0202 == parenttest2.GetProgramCounter());
}

TEST_CASE("Skip instruction if register doesnt equal number") {
    Emulator::Chip8 parenttest("Unit Test, supress error");

    parenttest.WriteToMemory(0x200, 0x48);
    parenttest.WriteToMemory(0x201, 0x54);

    parenttest.SetCpuRegister(8, 0x54);

    parenttest.EmulateCycle();

    REQUIRE(0x0202 == parenttest.GetProgramCounter());

    Emulator::Chip8 parenttest2("Unit Test, supress error");

    parenttest2.WriteToMemory(0x200, 0x48);
    parenttest2.WriteToMemory(0x201, 0x54);

    parenttest2.SetCpuRegister(8, 0x64);

    parenttest2.EmulateCycle();

    REQUIRE(0x0204 == parenttest2.GetProgramCounter());
}

TEST_CASE("Skip instruction if register equals other register") {
    Emulator::Chip8 parenttest("Unit Test, supress error");

    parenttest.WriteToMemory(0x200, 0x58);
    parenttest.WriteToMemory(0x201, 0x50);

    parenttest.SetCpuRegister(8, 0x54);
    parenttest.SetCpuRegister(5, 0x54);

    parenttest.EmulateCycle();

    REQUIRE(0x0204 == parenttest.GetProgramCounter());

    Emulator::Chip8 parenttest2("Unit Test, supress error");

    parenttest2.WriteToMemory(0x200, 0x58);
    parenttest2.WriteToMemory(0x201, 0x50);

    parenttest2.SetCpuRegister(8, 0x64);
    parenttest2.SetCpuRegister(5, 0x54);

    parenttest2.EmulateCycle();

    REQUIRE(0x0202 == parenttest2.GetProgramCounter());
}

TEST_CASE("load constant into register") {
    Emulator::Chip8 parenttest("Unit Test, supress error");

    parenttest.WriteToMemory(0x200, 0x61);
    parenttest.WriteToMemory(0x201, 0x05);

    parenttest.EmulateCycle();

    REQUIRE(5 == parenttest.GetCpuRegister(1));
}

TEST_CASE("add constant to register") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 5);

    parenttest.WriteToMemory(0x200, 0x71);
    parenttest.WriteToMemory(0x201, 0x05);

    parenttest.EmulateCycle();

    REQUIRE(10 == parenttest.GetCpuRegister(1));
}

TEST_CASE("set register x to value y") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 4);
    parenttest.SetCpuRegister(2, 7);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x20);

    parenttest.EmulateCycle();

    REQUIRE(7 == parenttest.GetCpuRegister(1));
}

TEST_CASE("or register x and y") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 0b0001);
    parenttest.SetCpuRegister(2, 0b1100);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x21);

    parenttest.EmulateCycle();

    REQUIRE(0b1101 == parenttest.GetCpuRegister(1));
}

TEST_CASE("and register x and y") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 0b0011);
    parenttest.SetCpuRegister(2, 0b1010);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x22);

    parenttest.EmulateCycle();

    REQUIRE(0b0010 == parenttest.GetCpuRegister(1));
}

TEST_CASE("xor register x and y") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 0b0011);
    parenttest.SetCpuRegister(2, 0b1010);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x23);

    parenttest.EmulateCycle();

    REQUIRE(0b1001 == parenttest.GetCpuRegister(1));
}

TEST_CASE("add register x and y") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 5);
    parenttest.SetCpuRegister(2, 6);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x24);

    parenttest.EmulateCycle();

    REQUIRE(11 == parenttest.GetCpuRegister(1));
    REQUIRE(0 == parenttest.GetCpuRegister(15));

    Emulator::Chip8 parenttest2("Unit Test, supress error");
    parenttest2.SetCpuRegister(1, 240);
    parenttest2.SetCpuRegister(2, 100);

    parenttest2.WriteToMemory(0x200, 0x81);
    parenttest2.WriteToMemory(0x201, 0x24);

    parenttest2.EmulateCycle();

    REQUIRE(84 == parenttest2.GetCpuRegister(1));
    REQUIRE(1 == parenttest2.GetCpuRegister(15));
}

TEST_CASE("sub register x and y") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 7);
    parenttest.SetCpuRegister(2, 2);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x25);

    parenttest.EmulateCycle();

    REQUIRE(5 == parenttest.GetCpuRegister(1));
    REQUIRE(1 == parenttest.GetCpuRegister(15));

    Emulator::Chip8 parenttest2("Unit Test, supress error");
    parenttest2.SetCpuRegister(1, 5);
    parenttest2.SetCpuRegister(2, 7);

    parenttest2.WriteToMemory(0x200, 0x81);
    parenttest2.WriteToMemory(0x201, 0x25);

    parenttest2.EmulateCycle();

    REQUIRE(254 == parenttest2.GetCpuRegister(1));
    REQUIRE(0 == parenttest2.GetCpuRegister(15));
}

TEST_CASE("shr register x") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 0b0110);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x26);

    parenttest.EmulateCycle();

    REQUIRE(3 == parenttest.GetCpuRegister(1));
    REQUIRE(0 == parenttest.GetCpuRegister(15));

    Emulator::Chip8 parenttest2("Unit Test, supress error");
    parenttest2.SetCpuRegister(1, 0b1111);

    parenttest2.WriteToMemory(0x200, 0x81);
    parenttest2.WriteToMemory(0x201, 0x26);

    parenttest2.EmulateCycle();

    REQUIRE(7 == parenttest2.GetCpuRegister(1));
    REQUIRE(1 == parenttest2.GetCpuRegister(15));
}

TEST_CASE("subn register x and y") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 2);
    parenttest.SetCpuRegister(2, 7);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x27);

    parenttest.EmulateCycle();

    REQUIRE(5 == parenttest.GetCpuRegister(1));
    REQUIRE(1 == parenttest.GetCpuRegister(15));

    Emulator::Chip8 parenttest2("Unit Test, supress error");
    parenttest2.SetCpuRegister(1, 7);
    parenttest2.SetCpuRegister(2, 5);

    parenttest2.WriteToMemory(0x200, 0x81);
    parenttest2.WriteToMemory(0x201, 0x27);

    parenttest2.EmulateCycle();

    REQUIRE(254 == parenttest2.GetCpuRegister(1));
    REQUIRE(0 == parenttest2.GetCpuRegister(15));
}

TEST_CASE("shl register x") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.SetCpuRegister(1, 0b0010);

    parenttest.WriteToMemory(0x200, 0x81);
    parenttest.WriteToMemory(0x201, 0x2E);

    parenttest.EmulateCycle();

    REQUIRE(4 == parenttest.GetCpuRegister(1));
    REQUIRE(0 == parenttest.GetCpuRegister(15));

    Emulator::Chip8 parenttest2("Unit Test, supress error");
    parenttest2.SetCpuRegister(1, 0b10000010);

    parenttest2.WriteToMemory(0x200, 0x81);
    parenttest2.WriteToMemory(0x201, 0x2E);

    parenttest2.EmulateCycle();

    REQUIRE(4 == parenttest2.GetCpuRegister(1));
    REQUIRE(1 == parenttest2.GetCpuRegister(15));
}

TEST_CASE("skip instruction if register doesnt equal other register") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.WriteToMemory(0x200, 0x98);
    parenttest.WriteToMemory(0x201, 0x50);

    parenttest.SetCpuRegister(8, 0x54);
    parenttest.SetCpuRegister(5, 0x64);

    parenttest.EmulateCycle();

    REQUIRE(0x204 == parenttest.GetProgramCounter());

    Emulator::Chip8 parenttest2("Unit Test, supress error");
    parenttest2.WriteToMemory(0x200, 0x98);
    parenttest2.WriteToMemory(0x201, 0x50);

    parenttest2.SetCpuRegister(8, 0x64);
    parenttest2.SetCpuRegister(5, 0x64);

    parenttest2.EmulateCycle();

    REQUIRE(0x202 == parenttest2.GetProgramCounter());
}

TEST_CASE("set index register") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.WriteToMemory(0x200, 0xA5);
    parenttest.WriteToMemory(0x201, 0x53);

    parenttest.EmulateCycle();

    REQUIRE(0x0553 == parenttest.GetIndexRegister());
}

TEST_CASE("jump to V0 plus constant") {
    Emulator::Chip8 parenttest("Unit Test, supress error");
    parenttest.WriteToMemory(0x200, 0xB5);
    parenttest.WriteToMemory(0x201, 0x53);

    parenttest.SetCpuRegister(0, 0x30);
    parenttest.EmulateCycle();

    REQUIRE(0x0583 == parenttest.GetProgramCounter());
}