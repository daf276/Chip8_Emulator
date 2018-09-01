//
// Created by attila on 31.08.18.
//

#ifndef CHIP8_EMULATOR_C_CHIP8_H
#define CHIP8_EMULATOR_C_CHIP8_H

#include <string>
#include <vector>

namespace Emulator {

    class Chip8 {

    private:
        unsigned short opcode;

        std::vector<void (Chip8::*)()> opcode_table;

        std::vector<unsigned char> memory; //4096 bits of memory
        std::vector<unsigned char> v; //CPU registers named V0 to VE, last register is the carry flag
        std::vector<unsigned short> stack; //16 Stacklevels

        unsigned short index_register;
        unsigned short program_counter;
        unsigned char stack_pointer;

        unsigned char delay_timer;
        unsigned char sound_timer;

        void OpCodeZero();

        void Call();

        void Jump();

        void PopulateOpCodeTable();

    public:
        Chip8(std::string path);

        void EmulateCycle();
    };
}


#endif //CHIP8_EMULATOR_C_CHIP8_H
