//
// Created by attila on 31.08.18.
//

#ifndef CHIP8_EMULATOR_C_CHIP8_H
#define CHIP8_EMULATOR_C_CHIP8_H

#include <string>
#include <vector>
#include <random>

namespace Emulator {

    class Chip8 {

    private:
        unsigned short opcode;

        int * display_pixels;

        std::vector<void (Chip8::*)()> opcode_table;
        std::vector<void (Chip8::*)()> opcode8_table;
        std::vector<void (Chip8::*)()> opcodeF_table;

        std::vector<unsigned char> memory; //4096 bits of memory
        std::vector<unsigned char> v; //CPU registers named V0 to VE, last register is the carry flag
        std::vector<unsigned short> stack; //16 Stacklevels

        unsigned short index_register;
        unsigned short program_counter;
        unsigned char stack_pointer;

        unsigned char delay_timer;
        unsigned char sound_timer;

        void PopulateOpCodeTables();

        //Functions for the opcodes
        void OpCodeInvalid();
        void OpCodeZero();
        void Call();
        void Jump();
        void RegisterAndConstantSE();
        void RegisterAndConstantSNE();
        void TwoRegistersSE();
        void LoadConstantIntoRegister();
        void AddConstantToRegister();
        void OpCodeEight();
        void TwoRegistersSNE();
        void SetRegisterIToConstant();
        void SetPCToNextInstruction();
        void SetPCToSkipNextInstruction();
        void JumpToConstantPlusV0();
        void SetCpuRegisterRandom();
        void DisplaySprite();
        void OpCodeE();
        void OpCodeF();

        //Functions for opcodes with 8XXX
        void StoreRegisterYInX();
        void ORRegisterXAndY();
        void ANDRegisterXAndY();
        void XORRegisterXAndY();
        void ADDRegisterXAndY();
        void SUBRegisterXAndY();
        void SHRRegisterX();
        void SUBNRegisterXAndY();
        void SHLRegisterX();

        //Functions for opcodes with FXXX
        void OpCodeFx0x();
        void OpCodeFx1x();


    public:
        Chip8(std::string path, int* screen_buffer);

        std::vector<bool> key_pressed;

        void EmulateCycle();

        void SetDelayTimer(unsigned char timer);
        unsigned char GetDelayTimer();
        unsigned char GetSoundTimer();
        void SetCpuRegister(int index, unsigned char value);
        unsigned char GetCpuRegister(int i);
        unsigned short GetStack(int i);
        unsigned char GetStackPointer();
        unsigned short GetProgramCounter();
        void SetIndexRegister(unsigned short i);
        unsigned short GetIndexRegister();
        void WriteToMemory(int index, unsigned char value);
    };
}


#endif //CHIP8_EMULATOR_C_CHIP8_H
