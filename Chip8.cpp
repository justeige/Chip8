#include <iostream>
#include <array>
#include <stdint.h>
#include <assert.h>

// typedefs
using Byte   = uint8_t;  // big-endian
using Word   = uint16_t; // for this emulator, a word is defined as two bytes
using OpCode = uint16_t; // max 35
using Pixel  = uint8_t;  // chip8 has only black/white as color

template <std::size_t N> using Bytes = std::array<Byte, N>;
template <std::size_t N> using Words = std::array<Word, N>;
template <std::size_t N> using Pixels = std::array<Pixel, N>;

enum Register {
    V0, V1, V2, V3,
    V4, V5, V6, V7,
    V8, V9, VA, VB,
    VC, VD, VE, VF,
    VMAX
};

const Word StartAddress = 0x200;

// ref for Chip8 data taken from: https://en.wikipedia.org/wiki/Chip-8#Virtual_machine_description
struct Chip8 {

    // data
    Words<4096>  memory = {};
    Words<16>    stack  = {};
    Pixels<2048> screen = {};
    Bytes<16>    key    = {};

    // register
    Words<16> V = {}; // V-register, data register
    Word      I = 0; // index register

    // indices
    Word   SI = 0;            // stack-index of current stack level
    Word   PC = StartAddress; // program counter

    // timer
    Byte delayTimer = 0;
    Byte soundTimer = 0;

    // methods
    void emulate(OpCode op);
    OpCode currentOp() const
    {
        return memory[PC] << 8 | memory[PC + 1];
    }
};

// full debug output
std::ostream& operator << (std::ostream& os, Chip8 const& c)
{
    // show the data register:
    os << "------------------------\n";
    os << "V0: " << c.V[V0] << " V4: " << c.V[V4] << " V8: " << c.V[V8] << " VC: " << c.V[VC] << '\n';
    os << "V1: " << c.V[V1] << " V5: " << c.V[V5] << " V9: " << c.V[V9] << " VD: " << c.V[VD] << '\n';
    os << "V2: " << c.V[V2] << " V6: " << c.V[V6] << " VA: " << c.V[VA] << " VE: " << c.V[VE] << '\n';
    os << "V3: " << c.V[V3] << " V7: " << c.V[V7] << " VB: " << c.V[VB] << " VF: " << c.V[VF] << '\n';
    os << "------------------------\n";

    // indices
    os << "PC: " << c.PC << '\n';
    os << "SI: " << c.SI << '\n';

    return os;
}


// -------------------------------------------------
int main(int argc, char** argv)
{
    // test: jump to address 003
    {
        Chip8 chip;
        chip.emulate(0x1003);
        assert(chip.PC == 3);
        std::cout << chip << '\n';
    }

    // test: set register v1 = 7
    {
        Chip8 chip;
        chip.emulate(0x6107);
        assert(chip.V[V1] == 7);
        assert(chip.PC == StartAddress + 2); // a 2 byte instruction took place
        std::cout << chip << '\n';
    }

    return 0;
}
// -------------------------------------------------



// implementations
void Chip8::emulate(OpCode op)
{
    // decode operation (not every value is in use, but its easier to read this way!)
    const auto   X = (op & 0x0F00) >> 8;
    const auto   Y = (op & 0x00F0) >> 4;
    const auto  NN = (op & 0x00FF);
    const auto NNN = (op & 0x0FFF);

    switch (op & 0xF000) {
    case 0x0000:
        /// TODO
        break;

    case 0x1000: // 0x1NNN  goto NNN.
        PC = NNN;
        break;

    case 0x6000: // 0x6XNN  VX := NN.
        V[X] = NN;
        PC += 2;
        break;

    case 0x7000: // 0x7XNN  VX := VX + NN.
        V[X] = V[X] + NN;
        PC += 2;
        break;

    case 0x9000: // 0x9XY0  IF (VX != VY) -> Skip next instruction
        if (V[X] != V[Y]) {
            PC += 2; // skip next
        }
        PC += 2;
        break;

    case 0xA000: // 0xANNN  I := NNN.
        I = NNN;
        PC += 2;
        break;

    case 0xB000: // 0xBNNN  goto NNN + V0
        PC = NNN + V[V0];
        break;

    default:
        assert(false);
        break;
    }
}