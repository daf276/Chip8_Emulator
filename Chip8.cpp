//
// Created by attila on 31.08.18.
//

#include <iostream>
#include <fstream>
#include "Chip8.h"
#include <random>

namespace Emulator {

    static std::random_device rd;
    static std::mt19937 mt(rd());
    static std::uniform_real_distribution<double> dist(0.0, 255.0);

    Chip8::Chip8(std::string path) : memory(4096), v(16), stack(16), opcode_table(17) {
        PopulateOpCodeTable();

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
        } else {
            std::cerr << std::string("Can't find inputfile") << path << std::endl;
        }
    }

    void Chip8::EmulateCycle() {
        opcode = memory[program_counter] << 8 | memory[program_counter + 1];
        std::cout << std::hex << std::uppercase << opcode << std::endl;

        (this->*opcode_table[(opcode & 0xF000) >> 12])();
    }

    void Chip8::PopulateOpCodeTable() { //Array with opcodes in the form of index of array == first 4 bits of opcode
        opcode_table = {&Chip8::OpCodeZero, &Chip8::Jump, &Chip8::Call, &Chip8::RegisterAndConstantSE,
                        &Chip8::RegisterAndConstantSNE, &Chip8::TwoRegistersSE, &Chip8::LoadConstantIntoRegister,
                        &Chip8::AddConstantToRegister, &Chip8::OpCodeEight, &Chip8::TwoRegistersSNE,
                        &Chip8::SetRegisterIToConstant, &Chip8::JumpToConstantPlusV0, &Chip8::SetCpuRegisterRandom, &Chip8::DisplaySprite, &Chip8::OpCodeE, & Chip8::OpCodeF};
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
        v[(opcode & 0x0F00)
                >> 8] = static_cast<unsigned char>(opcode); //Casting gives us the 2 LSBs, which represent the constant
        SetPCToNextInstruction();
    }

    void Chip8::AddConstantToRegister() { //Opcode 7XXX -> ADD Vx, byte
        int current_register = (opcode & 0x0F00) >> 8;
        v[current_register] += static_cast<unsigned char>(opcode); //Casting gives us the 2 LSBs, which represent the constant
        SetPCToNextInstruction();
    }

    void Chip8::OpCodeEight() {
        int register_x = (opcode & 0x0F00) >> 8;
        int register_y = (opcode & 0x00F0) >> 4;
        unsigned char value_x = v[register_x];
        unsigned char value_y = v[register_y];

        switch (opcode & 0xF) {
            case 0: //LD
                v[register_x] = value_y;
                break;
            case 1: //OR
                v[register_x] = value_x | value_y;
                break;
            case 2: //AND
                v[register_x] = value_x & value_y;
                break;
            case 3: //XOR
                v[register_x] = value_x ^ value_y;
                break;
            case 4: //ADD
                v[register_x] = value_x + value_y;
                v[15] = static_cast<unsigned char>((value_x | value_y > 255) ? 1 : 0);
                break;
            case 5: //SUB
                v[register_x] = value_x - value_y;
                v[15] = static_cast<unsigned char>((value_x > value_y) ? 1 : 0);
                break;
            case 6: //SHR
                v[register_x] /= 2;
                v[15] = static_cast<unsigned char>(value_x & 0b1);
                break;
            case 7: //SUBN
                v[register_x] = value_y - value_x;
                v[15] = static_cast<unsigned char>((value_y > value_x) ? 1 : 0);
                break;
            case 0xE: //SHL
                v[register_x] *= 2;
                v[15] = static_cast<unsigned char>((value_x & 0b10000000) >> 7);
                break;
            default:
                std::cerr << "opcode not in specification" << std::endl;
        }
        SetPCToNextInstruction();
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
        v[(opcode & 0x0F00) >> 8] = static_cast<unsigned char>((0x00FF & opcode)) & static_cast<unsigned char>(dist(mt));
        SetPCToNextInstruction();
    }

    void Chip8::DisplaySprite() { //Opcode DXXX -> DRW Vx, Vy, nibble
        //TODO
        SetPCToNextInstruction();
    }

    void Chip8::OpCodeE() {
        //TODO
        SetPCToNextInstruction();
    }

    void Chip8::OpCodeF() {
        //TODO
        SetPCToNextInstruction();
    }

    void Chip8::SetPCToNextInstruction() {
        program_counter += 2;
    }

    void Chip8::SetPCToSkipNextInstruction() {
        program_counter += 4;
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