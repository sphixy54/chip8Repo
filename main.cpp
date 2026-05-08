#include <iostream>
#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>
const unsigned int START_ADRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADRESS = 0x50;
uint8_t fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
class Chip8 {
    public:
    Chip8() ;
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index{};
    uint16_t pc{};
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video [64*32]{};
    uint32_t opcode{};
    std::default_random_engine randGen;
    void LoadROM(char const* filename);
};

void Chip8::LoadROM(char const* filename) {
    //Open the file as a binary stream and move the file  pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        //get size of file and allocate a buffer to hold the contents
        std::streampos size = file.tellg();
        char* buffer = new char[size];
        // Go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        // load the ROM contents into the Chip8's memory, starting at 0x200
        for (long i =0; i< size; ++i) {
            memory[START_ADRESS+i] = buffer[i];
        }
        //free the buffer
        delete[] buffer;
    }

}

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    pc = START_ADRESS;
    // load font into memory
    for (unsigned int i =0; i< FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADRESS + i] = fontset[i];
    }
}