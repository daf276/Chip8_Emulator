//
// Created by attila on 31.08.18.
//

#include <iostream>
#include <fstream>
#include "Chip8.h"
#include <random>
#include <algorithm>

namespace Emulator {

    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<double> dist(0.0, 255.0);

    Chip8::Chip8(std::string path, int *screen_buffer) : memory(4096), v(16), stack(16), key_pressed(16) {
        PopulateOpCodeTables();

        display_pixels = screen_buffer;
        program_counter = 0x200;

        std::ifstream inputfile;
        inputfile.open(path.c_str());

        if (inputfile.is_open()) {
            int counter = 0x200;
            while (!inputfile.eof()) {
                inputfile >> memory[counter];
                counter++;
            }

            inputfile.close();
        } else if (path == "Unit Test, supress error") {
            //Used to supress the cant find inputfile error while unit testing
        } else
            std::cerr << std::string("Can't find inputfile") << path << std::endl;
    }

    void Chip8::EmulateCycle() {
        opcode = memory[program_counter] << 8 | memory[program_counter + 1];
        std::cout << std::hex << std::uppercase << opcode << std::endl;

        (this->*opcode_table[(opcode & 0xF000) >> 12])();
    }

    void Chip8::PopulateOpCodeTables() { //Array with opcodes in the form of index of array == first 4 bits of opcode
        opcode_table = {&Chip8::OpCodeZero, &Chip8::Jump, &Chip8::Call, &Chip8::RegisterAndConstantSE,
                        &Chip8::RegisterAndConstantSNE, &Chip8::TwoRegistersSE, &Chip8::LoadConstantIntoRegister,
                        &Chip8::AddConstantToRegister, &Chip8::OpCodeEight, &Chip8::TwoRegistersSNE,
                        &Chip8::SetRegisterIToConstant, &Chip8::JumpToConstantPlusV0, &Chip8::SetCpuRegisterRandom,
                        &Chip8::DisplaySprite, &Chip8::OpCodeE, &Chip8::OpCodeF};

        opcode8_table = {&Chip8::StoreRegisterYInX, &Chip8::ORRegisterXAndY, &Chip8::ANDRegisterXAndY,
                         &Chip8::XORRegisterXAndY, &Chip8::ADDRegisterXAndY, &Chip8::SUBRegisterXAndY,
                         &Chip8::SHRRegisterX, &Chip8::SUBNRegisterXAndY, &Chip8::OpCodeInvalid, &Chip8::OpCodeInvalid,
                         &Chip8::OpCodeInvalid, &Chip8::OpCodeInvalid, &Chip8::OpCodeInvalid, &Chip8::OpCodeInvalid,
                         &Chip8::SHLRegisterX, &Chip8::OpCodeInvalid};
        opcodeF_table = {&Chip8::OpCodeFx0x, &Chip8::OpCodeInvalid, &Chip8::OpCodeInvalid};
    }

    void Chip8::OpCodeInvalid() {//Opcodes 0XXX
        std::cerr << "Invalid opcode" << std::endl;
    }

    void Chip8::OpCodeZero() {//Opcodes 0XXX
        if (opcode == 0x00E0) { //CLS
            //TODO implements
            program_counter += 2;
        } else if (opcode == 0x00EE) { //RET
            //TODO Unit Tests
            // Return from Subroutine
            stack_pointer--;
            program_counter = stack[stack_pointer];
        }
    }

    void Chip8::Jump() { //Opcode 1XXX -> JP addr
        program_counter = static_cast<unsigned short>(opcode & 0x0FFF);
    }

    void Chip8::Call() { //Opcode 2XXX -> CALL addr
        stack[stack_pointer] = program_counter;
        stack_pointer++;
        program_counter = static_cast<unsigned short>(opcode & 0x0FFF);
    }

    void Chip8::RegisterAndConstantSE() { //Opcode 3XXX -> SE Vx, byte
        if (v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            SetPCToSkipNextInstruction();
        else
            SetPCToNextInstruction();
    }

    void Chip8::RegisterAndConstantSNE() { //Opcode 4XXX -> SNE Vx, byte
        if (v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            SetPCToSkipNextInstruction();
        else
            SetPCToNextInstruction();
    }

    void Chip8::TwoRegistersSE() { //Opcode 5XXX -> SE Vx, Vy
        if (v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4])
            SetPCToSkipNextInstruction();
        else
            SetPCToNextInstruction();
    }

    void Chip8::LoadConstantIntoRegister() { //Opcode 6XXX -> LD Vx, byte
        v[(opcode & 0x0F00) >> 8] = static_cast<unsigned char>(opcode); //Casting gives the 2 LSBs, aka the constant
        SetPCToNextInstruction();
    }

    void Chip8::AddConstantToRegister() { //Opcode 7XXX -> ADD Vx, byte
        v[(opcode & 0x0F00) >> 8] += static_cast<unsigned char>(opcode); //Casting gives the 2 LSBs, aka the constant
        SetPCToNextInstruction();
    }

    void Chip8::OpCodeEight() { //All Opcodes in the form of 8XXX
        (this->*opcode8_table[opcode & 0x000F])();
        SetPCToNextInstruction();
    }

    void Chip8::StoreRegisterYInX() { //opcode 8XX0 -> LD Vx, Vy
        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
    }

    void Chip8::ORRegisterXAndY() { //opcode 8XX1 -> OR Vx, Vy
        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] | v[(opcode & 0x00F0) >> 4];
    }

    void Chip8::ANDRegisterXAndY() { //opcode 8XX2 -> AND Vx, Vy
        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] & v[(opcode & 0x00F0) >> 4];
    }

    void Chip8::XORRegisterXAndY() { //opcode 8XX3 -> XOR Vx, Vy
        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] ^ v[(opcode & 0x00F0) >> 4];
    }

    void Chip8::ADDRegisterXAndY() { //opcode 8XX4 -> AND Vx, Vy
        v[15] = static_cast<unsigned char>((v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4] > 255) ? 1 : 0);
        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] + v[(opcode & 0x00F0) >> 4];
    }

    void Chip8::SUBRegisterXAndY() { //opcode 8XX5 -> SUB Vx, Vy
        v[15] = static_cast<unsigned char>((v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4]) ? 1 : 0);
        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x0F00) >> 8] - v[(opcode & 0x00F0) >> 4];
    }

    void Chip8::SHRRegisterX() { //opcode 8XX6 -> SHR Vx {, Vy}
        v[15] = static_cast<unsigned char>(v[(opcode & 0x0F00) >> 8] & 0b1);
        v[(opcode & 0x0F00) >> 8] /= 2;
    }

    void Chip8::SUBNRegisterXAndY() { //opcode 8XX7 -> SUBN Vx, Vy
        v[15] = static_cast<unsigned char>(v[(opcode & 0x00F0) >> 4] > (v[(opcode & 0x0F00) >> 8]) ? 1 : 0);
        v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
    }

    void Chip8::SHLRegisterX() { //opcode 8XXE -> SHL Vx {, Vy}
        v[15] = static_cast<unsigned char>((v[(opcode & 0x0F00) >> 8] & 0b10000000) >> 7);
        v[(opcode & 0x0F00) >> 8] *= 2;
    }

    void Chip8::TwoRegistersSNE() { //Opcode 9XXX -> SNE Vx, Vy
        if (v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4])
            SetPCToSkipNextInstruction();
        else
            SetPCToNextInstruction();
    }

    void Chip8::SetRegisterIToConstant() { //Opcode AXXX -> LD I, addr
        index_register = static_cast<unsigned short>(opcode & 0x0FFF);
        SetPCToNextInstruction();
    }

    void Chip8::JumpToConstantPlusV0() { //Opcode BXXX -> JP V0, addr
        program_counter = static_cast<unsigned short>((opcode & 0x0FFF) + v[0]);
    }

    void Chip8::SetCpuRegisterRandom() { //Opcode CXXX -> RND Vx, byte
        v[(opcode & 0x0F00) >> 8] =
                static_cast<unsigned char>((0x00FF & opcode)) & static_cast<unsigned char>(dist(mt));
        SetPCToNextInstruction();
    }

    void Chip8::DisplaySprite() { //Opcode DXXX -> DRW Vx, Vy, nibble
        unsigned char number_of_bytes = static_cast<unsigned char>(opcode & 0x000F);
        unsigned char x = v[(opcode & 0x0F00) >> 8];
        unsigned char y = v[(opcode & 0x00F0) >> 4];

        for (int i = 0; i < number_of_bytes; ++i) {
            unsigned char row_of_pixels = memory[index_register + i];

            display_pixels[64 * (y + i) + x] ^= (row_of_pixels & 128) > 0 ? 255 : 0;
            display_pixels[64 * (y + i) + x + 1] ^= (row_of_pixels & 64) > 0 ? 255 : 0;
            display_pixels[64 * (y + i) + x + 2] ^= (row_of_pixels & 32) > 0 ? 255 : 0;
            display_pixels[64 * (y + i) + x + 3] ^= (row_of_pixels & 16) > 0 ? 255 : 0;
            display_pixels[64 * (y + i) + x + 4] ^= (row_of_pixels & 8) > 0 ? 255 : 0;
            display_pixels[64 * (y + i) + x + 5] ^= (row_of_pixels & 4) > 0 ? 255 : 0;
            display_pixels[64 * (y + i) + x + 6] ^= (row_of_pixels & 2) > 0 ? 255 : 0;
            display_pixels[64 * (y + i) + x + 7] ^= (row_of_pixels & 1) > 0 ? 255 : 0;
        }

        SetPCToNextInstruction();
    }

    void Chip8::OpCodeE() { //Skip next instruction depending on key state
        if ((opcode & 0x00FF) == 0x009E) { //SKP Vx
            if(key_pressed[(opcode & 0x0F00) >> 8]) SetPCToSkipNextInstruction();
            else SetPCToNextInstruction();
        } else if ((opcode & 0x00FF) == 0x00A1) { //SKNP Vx
            if(!key_pressed[(opcode & 0x0F00) >> 8]) SetPCToSkipNextInstruction();
            else SetPCToNextInstruction();
        }
    }

    void Chip8::OpCodeF() {
        (this->*opcodeF_table[(opcode & 0x00F0) >> 4])();
    }

    void Chip8::OpCodeFx0x() {
        if ((opcode & 0x000F) == 0x7) {
            v[(opcode & 0x0F00) >> 8] = delay_timer;
        } else if ((opcode & 0x000F) == 0xA) {
            if (std::any_of(key_pressed.begin(), key_pressed.end(), [](bool i) { return i; })) {
                SetPCToNextInstruction();
            }
        }
    }

    void Chip8::SetPCToNextInstruction() {
        program_counter += 2;
    }

    void Chip8::SetPCToSkipNextInstruction() {
        program_counter += 4;
    }

    void Chip8::SetDelayTimer(unsigned char timer){
        delay_timer = timer;
    }

    void Chip8::SetCpuRegister(int index, unsigned char value) {
        v[index] = value;
    }

    unsigned char Chip8::GetCpuRegister(int i) {
        return v[i];
    }

    unsigned short Chip8::GetStack(int i) {
        return stack[i];
    }

    unsigned char Chip8::GetStackPointer() {
        return stack_pointer;
    }

    void Chip8::WriteToMemory(int index, unsigned char value) {
        memory[index] = value;
    }

    unsigned short Chip8::GetProgramCounter() {
        return program_counter;
    }

    unsigned short Chip8::GetIndexRegister() {
        return index_register;
    }
}