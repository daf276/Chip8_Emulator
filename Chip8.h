#ifndef CHIP8_EMULATOR_C_CHIP8_H
#define CHIP8_EMULATOR_C_CHIP8_H

#include <string>
#include <vector>
#include <random>
#include <memory>

namespace Emulator {

    class Chip8 {

    private:
        std::unique_ptr<std::mt19937> mt;
        std::unique_ptr<std::uniform_real_distribution<double>> dist;

        std::vector<void (Chip8::*)()> opcode_table;
        std::vector<void (Chip8::*)()> opcode8_table;
        std::vector<void (Chip8::*)()> opcodeF_table;

        std::vector<std::vector<bool>> gfx; //64*32 Pixel screen
        std::vector<unsigned char> memory; //4096 bits of memory
        std::vector<unsigned char> v; //CPU registers named V0 to VE, last register is the carry flag
        std::vector<unsigned short> stack; //16 Stacklevels

        unsigned short opcode;

        unsigned short index_register;
        unsigned short program_counter;
        unsigned char stack_pointer;

        unsigned char delay_timer;
        unsigned char sound_timer;

        void PopulateOpCodeTables();
        void LoadHexDigitSpriteIntoMemory();

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
        void LoadFontLocationIntoIndexRegister();
        void StoreBCDInMemory();
        void LoadRegistersIntoMemory();
        void LoadMemoryIntoRegisters();

    public:
        std::vector<bool> key_pressed;

        Chip8();
        explicit Chip8(std::string path);

        void EmulateCycle();

        const std::vector<std::vector<bool>> &GetGfx() const;
        unsigned short GetIndexRegister() const;
        void SetIndexRegister(unsigned short index_register);
        unsigned short GetProgramCounter() const;
        void SetProgramCounter(unsigned short program_counter);
        unsigned char GetStackPointer() const;
        void SetStackPointer(unsigned char stack_pointer);
        unsigned char GetDelayTimer() const;
        void SetDelayTimer(unsigned char delay_timer);
        unsigned char GetSoundTimer() const;
        void SetSoundTimer(unsigned char sound_timer);
        void SetCpuRegister(int index, unsigned char value);
        unsigned char GetCpuRegister(int i);
        unsigned short GetStack(int i);
        void WriteToMemory(int index, unsigned char value);
        unsigned char GetMemory(int index);
    };
}


#endif //CHIP8_EMULATOR_C_CHIP8_H
