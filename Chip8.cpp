//
// Created by attila on 31.08.18.
//

#include <iostream>
#include <fstream>
#include "Chip8.h"

namespace Emulator {

    Chip8::Chip8(std::string path) : memory(4096), v(16), stack(16), opcode_table(17) {
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

    void Chip8::PopulateOpCodeTable() {
        opcode_table = {&Chip8::OpCodeZero, &Chip8::Jump, &Chip8::Call};
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
        program_counter = opcode & 0x0FFF;
    }

    void Chip8::Call() { //Opcode 2XXX -> CALL addr
        stack[stack_pointer] = program_counter;
        stack_pointer++;
        program_counter = opcode & 0x0FFF;
    }
/*
void Chip8::registerAndConstantSE() {
    if (getCpuRegisterValue((opCode & 0x0F00) >> 8) == (opCode & 0x00FF))
        setPCToSkipNextInstruction();
    else
        setPCToNextInstruction();
}

void Chip8::registerAndConstantSNE() {
    if (getCpuRegisterValue((opCode & 0x0F00) >> 8) != (opCode & 0x00FF))
        setPCToSkipNextInstruction();
    else
        setPCToNextInstruction();
}

void Chip8::twoRegistersSE() {
    if (getCpuRegisterValue((opCode & 0x0F00) >> 8) == getCpuRegisterValue((opCode & 0x00F0) >> 4))
        setPCToSkipNextInstruction();
    else
        setPCToNextInstruction();
}

void Chip8::addConstantToRegister() {
    int register = (opCode & 0x0F00) >> 8;
    setCpuRegister(register, getCpuRegisterValue(register) + (byte) opCode); //Casting gives us the 2 LSBs, which represent a constant
    setPCToNextInstruction();
}

void Chip8::opCodeEight() {
    int registerX = (opCode & 0x0F00) >> 8;
    int registerY = (opCode & 0x00F0) >> 4;
    int valueX = getCpuRegisterValue(registerX);
    int valueY = getCpuRegisterValue(registerY);

    switch (opCode & 0xF) {
        case 0: //LD
            setCpuRegister(registerX, valueY);
            break;
        case 1: //OR
            setCpuRegister(registerX, valueX | valueY);
            break;
        case 2: //AND
            setCpuRegister(registerX, valueX & valueY);
            break;
        case 3: //XOR
            setCpuRegister(registerX, valueX ^ valueY);
            break;
        case 4: //ADD
            setCpuRegister(registerX, valueX + valueY);
            setCpuRegister(15, (valueX + valueY > 255) ? 1 : 0);
            break;
        case 5: //SUB
            setCpuRegister(registerX, valueX - valueY);
            setCpuRegister(15, (valueX > valueY) ? 1 : 0);
            break;
        case 6: //SHR
            setCpuRegister(registerX, valueX / 2);
            setCpuRegister(15, valueX & 0b1);
            break;
        case 7: //SUBN
            setCpuRegister(registerX, valueY - valueX);
            setCpuRegister(15, (valueY > valueX) ? 1 : 0);
            break;
        case 0xE: //SHL
            setCpuRegister(registerX, valueX * 2);
            setCpuRegister(15, (valueX & 0b10000000) >> 7);
            break;
        default:
            try {
                throw new Exception("OpCode couldn't be handled cause it isn't in the specification, check your program");
            } catch (Exception e) {
                e.printStackTrace();
            }
            System.exit(-1);
    }
    setPCToNextInstruction();
}

void Chip8::twoRegistersSNE() {
    if (getCpuRegisterValue((opCode & 0x0F00) >> 8) != getCpuRegisterValue((opCode & 0x00F0) >> 4))
        setPCToSkipNextInstruction();
    else
        setPCToNextInstruction();
}

void Chip8::random() {
    setCpuRegister((opCode & 0x0F00) >> 8, (0x00FF & opCode) & new Random().nextInt(256));
    setPCToNextInstruction();
}

void Chip8::setPCToNextInstruction() {
    programCounter += 2;
}

void Chip8::setPCToSkipNextInstruction() {
    programCounter += 4;
}

void setCpuRegister(int index, int value) {
    cpuRegister[index] = (byte) value;
}

int getCpuRegisterValue(int i) {
    return Byte.toUnsignedInt(cpuRegister[i]);
}

int getStack(int i) {
    return stack[i];
}

int getStackPointer() {
    return stackPointer;
}

void writeToMemory(int index, byte value) {
    memory[index] = value;
}

int getProgramCounter() {
    return programCounter;
}

int getiRegister() {
    return iRegister;
}
*/
}