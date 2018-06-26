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

const Pixel Black = 0;
const Pixel White = 1;
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

    // 0NNN
    case 0x0000: {

        switch (op & 0x00FF) {

        // 00E0  clear screen
        case 0x00E0:
            for (auto& pixel : screen) {
                pixel = Black;
            }
            break;

        // 00EE  return from subroutine
        case 0x00EE:
            /// TODO implement
            assert(false);
            break;

        default:
            assert(false);
            break;
        }
    }
    break;

    // 1NNN  goto NNN.
    case 0x1000:
        PC = NNN;
        break;

    // 2NNN  call subroutine
    case 0x2000:
        /// TODO implement
        assert(false);
        break;

    // 3XNN  if VX == NN -> skip next instruction
    case 0x3000:
        if (V[X] == NN) {
            PC += 2;
        }
        PC += 2;
        break;

    // 4XNN  if VX != NN -> skip next instruction
    case 0x4000:
        if (V[X] != NN) {
            PC += 2;
        }
        PC += 2;
        break;

    // 5XY0  if VX == VY -> skip next instruction
    case 0x5000:
        if (V[X] == V[Y]) {
            PC += 2;
        }
        PC += 2;
        break;

    // 6XNN  VX := NN.
    case 0x6000:
        V[X] = NN;
        PC += 2;
        break;

    // 7XNN  VX := VX + NN.
    case 0x7000:
        V[X] = V[X] + NN;
        PC += 2;
        break;

    // 8NNN
    case 0x8000: {

        switch (op & 0x000F) {
        case 0x0000:
            /// TODO implement
            assert(false);
            break;

        case 0x0001:
            /// TODO implement
            assert(false);
            break;

        case 0x0002:
            /// TODO implement
            assert(false);
            break;

        case 0x0003:
            /// TODO implement
            assert(false);
            break;

        case 0x0004:
            /// TODO implement
            assert(false);
            break;

        case 0x0005:
            /// TODO implement
            assert(false);
            break;

        case 0x0006:
            /// TODO implement
            assert(false);
            break;

        case 0x0007:
            /// TODO implement
            assert(false);
            break;

        case 0x000E:
            /// TODO implement
            assert(false);
            break;

        default:
            assert(false);
            break;
        }
        break;
    }

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

    case 0xC000:
        /// TODO implement
        assert(false);
        break;

    case 0xD000:
        /// TODO implement
        assert(false);
        break;

    // ENNN
    case 0xE000: {

        switch (op & 0x00FF) {

        case 0x009E:
            /// TODO implement
            assert(false);
            break;

        case 0x00A1:
            /// TODO implement
            assert(false);
            break;

        default:
            assert(false);
            break;
        }
    }
    break;

    // FNNN
    case 0xF000: {

        switch (op & 0x00FF) {
        case 0x0007:
            /// TODO implement
            assert(false);
            break;

        case 0x000A:
            /// TODO implement
            assert(false);
            break;

        case 0x0015:
            /// TODO implement
            assert(false);
            break;

        case 0x0018:
            /// TODO implement
            assert(false);
            break;

        case 0x001E:
            /// TODO implement
            assert(false);
            break;

        case 0x0029:
            /// TODO implement
            assert(false);
            break;

        case 0x0033:
            /// TODO implement
            assert(false);
            break;

        case 0x0055:
            /// TODO implement
            assert(false);
            break;

        case 0x0065:
            /// TODO implement
            assert(false);
            break;

        default:
            assert(false);
            break;
        }
        break;
    }

    default:
        assert(false);
        break;
    }
}