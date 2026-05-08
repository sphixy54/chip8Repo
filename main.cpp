#include <iostream>
#include <cstdint>
#include <fstream>
#include <chrono>
#include <cstring>
#include <random>
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADRESS = 0x50;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;

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

    void OP_00E0();

    void OP_00EE();

    void OP_1nnn();

    void OP_2nnn();

    void OP_3xkk();

    void OP_4xkk();

    void OP_5xy0();

    void OP_6xkk();

    void OP_7xkk();

    void OP_8xy0();

    void OP_8xy1();

    void OP_8xy2();

    void OP_8xy3();

    void OP_8xy4();

    void OP_8xy5();

    void OP_8xy6();

    void OP_8xy7();

    void OP_8xyE();

    void OP_9xy0();

    void OP_Annn();

    void OP_Bnnn();

    void OP_Cxkk();

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
    std::uniform_int_distribution<uint8_t> randByte;
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
            memory[START_ADDRESS+i] = buffer[i];
        }
        //free the buffer
        delete[] buffer;
    }

}

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
    pc = START_ADDRESS;
    // load font into memory
    for (unsigned int i =0; i< FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADRESS + i] = fontset[i];
    }
}
// cls: clear the display
void Chip8::OP_00E0() {
    memset(memory, 0, sizeof(video));
}
// ret: return from a subroutine
void Chip8::OP_00EE() {
    --sp;
    pc = stack[sp];
}
// JP addr: jump to location
void Chip8::OP_1nnn() {
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

// call addr: call subroutine at nnn.
void Chip8::OP_2nnn() {
    uint16_t address = opcode & 0x0FFFu;
    stack[sp] = pc;
    ++sp;
    pc = address;
}
// SE Vx, skip next instruction if Vx = kk
void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[Vx] == byte) {
        pc +=2;
    }
}

// SNE Vx, byte skip next instruction if Vx =! kk

void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[Vx] != byte) {
        pc +=2;
    }
}

// SE Vx, Vy skip next instruction if vx = vy
void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    if (registers[Vx] == registers[Vy]) {
        pc +=2;
    }
}

// LD Vx, byte

void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = byte;
}
// add Vx, byte set Vx = Vx + kk

void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] += byte;
}
// LD Vx,Vy set Vx =Vy
void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    registers[Vx] = registers[Vy];
}
// OR Vx,Vy set Vx = Vx OR Vy
void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    registers[Vx] |= registers[Vy];
}

// and Vx,Vy Set Vx = Vx AND Vy
void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    registers[Vx] &= registers[Vy];
}
// add Vx, Vy set Vx = VX XOR Vy
void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    registers[Vx] ^= registers[Vy];
}

// add Vx, Vy set Vx = Vx+Vy, set Vf = carry
void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    uint16_t sum = registers[Vx] + registers[Vy];
    if (sum > 255U) {
        registers[0xF] = 1;
    }
    else {
        registers[0xF] = 0;
    }
    registers[Vx] = sum & 0xFFu;
}

// 8xy5 - SUB Vx, Vy set Vx = Vx-Vy, Set VF = NOT borrow

void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1;
    }
    else {
        registers[0xF] = 0;
    }
    registers[Vx] -= registers[Vy];
}

// set Vx= Vx SHR 1

void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] >>=1;
}

// set Vx = Vy - Vx, set VF NOT BORROW

void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    if (registers[Vy] > registers[Vx])
    {
        registers[0xF] = 1;
    }
    else {
        registers[0xF] = 0;
    }
    registers[Vx] = registers[Vy] - registers[Vx];
}
// set Vx = Vx SHL 1
void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    registers[0xF] = (registers[Vx] & 0x80) >> 7u;
    registers[Vx] <<= 1;
}

// skip next instruciton if vx != vy
void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] != registers[Vy]) {
        pc +=2;
    }
}

// set I = nnn
void Chip8::OP_Annn() {
    uint16_t address = opcode&0x0FFFu;
    index = address;
}

// jump to location nnn +v0
void Chip8::OP_Bnnn() {
    uint16_t address = opcode&0x0FFFu;
    pc = registers[0]+ address;
}

// set vx = random byte and kkk

void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = randByte(randGen) & byte;
}

// display n-byte sprite starting at memory location I at (vx ,vy), set Vf == collision.

void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;
    registers[0xF] = 0;
    for (unsigned int row = 0; row < height; row++) {
        uint8_t spriteByte = memory[index + row];
        for (unsigned int col = 0; col < 8; col++) {
            uint8_t spritePixel= spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            //sprite pixel is on
            if (spritePixel) {
                if (*screenPixel = 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}