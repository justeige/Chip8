#include <iostream>
#include <array>
#include <stdint.h>
#include <assert.h>

// typedefs
using Byte   = uint8_t;  // big-endian
using Word   = uint16_t; // for this emulator, a word is defined as two bytes
using OpCode = uint16_t; // max 35
using Pixel  = uint8_t;  // chip8 has only black/white as color

template <std::size_t N> using Bytes  = std::array<Byte, N>;
template <std::size_t N> using Words  = std::array<Word, N>;
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
    const auto   x = (op & 0x0F00) >> 8;
    const auto   y = (op & 0x00F0) >> 4;
    const auto  nn = (op & 0x00FF);
    const auto nnn = (op & 0x0FFF);

    auto& Vx = V[x];
    auto& Vy = V[y];
    auto& Vf = V[VF]; // last register == carry flag

    // move one instruction forward == 2 bytes
    PC += 2;

    switch (op & 0xF000) {

    // 0nnn
    case 0x0000: {

        switch (op & 0x00FF) {

        // 00E0  clear screen
        case 0x00E0:
            screen.fill(Black);
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

    // 1nnn  goto nnn.
    case 0x1000:
        PC = nnn;
        break;

    // 2nnn  call subroutine
    case 0x2000:
        /// TODO implement
        assert(false);
        break;

    // 3xnn  skip next instruction if VX == NN
    case 0x3000:
        if (Vx == nn) {
            PC += 2;
        }
        PC += 2;
        break;

    // 4xnn  skip next instruction if VX != NN
    case 0x4000:
        if (Vx != nn) {
            PC += 2;
        }
        break;

    // 5xy0  skip next instruction if VX == VY
    case 0x5000:
        if (Vx == Vy) {
            PC += 2;
        }
        break;

    // 6xnn  Vx := nn
    case 0x6000:
        Vx = nn;
        break;

    // 7XNN  Vx := Vx + nn (no carry)
    case 0x7000:
        Vx = Vx + nn;
        break;

    // 8nnn
    case 0x8000: {

        switch (op & 0x000F) {

        // 8xy0  Vx := Vy
        case 0x0000:
            Vx = Vy;
            break;

        // 8xy1  Vx := Vx bit_or Vy
        case 0x0001:
            Vx |= Vy;
            break;

        // 8xy2  Vx := Vx bit_and Vy
        case 0x0002:
            Vx &= Vy;
            break;

        // 8xy3  Vx := Vx xor Vy
        case 0x0003:
            Vx ^= Vy;
            break;

        // 8xy4  Vx := Vx + Vy, Vf == carry
        case 0x0004:
            Vf = (Vx + Vy > 255) ? 1 : 0;
            Vx += Vy;
            break;

        // 8xy5  Vx := Vx - Vy, Vf == borrow
        case 0x0005:
            /// TODO implement
            assert(false);
            break;

        // 8xy6  Vx := right_shift Vy, Vf := least significant bit of Vy BEFORE the shift
        case 0x0006:
            /// TODO implement
            assert(false);
            break;

        // 8xy7  Vx := Vy - Vx, Vf == borrow (reversal of 8xy5)
        case 0x0007:
            /// TODO implement
            assert(false);
            break;

        // 8xyE  Vx := left_shift Vy, Vf := least significant bit of Vy BEFORE the shift
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

    // 0x9xy0  skip next instruction if Vx != Vy
    case 0x9000:
        if (Vx != Vy) {
            PC += 2; // skip next
        }
        break;

    // 0xAnnn  I := nnn
    case 0xA000:
        I = nnn;
        break;

    // 0xBnnn  goto nnn + V0
    case 0xB000:
        PC = nnn + V[V0];
        break;

    // 0xCxnn  Vx := random_number bit_and nn
    case 0xC000:
        /// TODO implement
        assert(false);
        break;

    // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
    case 0xD000:
        /// TODO implement
        assert(false);
        break;

    // Ennn
    case 0xE000: {

        switch (op & 0x00FF) {

        // Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
        case 0x009E:
            /// TODO implement
            assert(false);
            break;

        // Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
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

    // Fnnn
    case 0xF000: {

        switch (op & 0x00FF) {

        // Fx07  Vx := delayTimer
        case 0x0007:
            Vx = delayTimer;
            break;

        // A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
        case 0x000A:
            /// TODO implement
            assert(false);
            break;

        // Fx15  delayTimer := Vx
        case 0x0015:
            delayTimer = Vx;
            break;

        // Fx18  soundTimer := Vx
        case 0x0018:
            soundTimer = Vx;
            break;

        // Fx1E  I := I + Vx
        case 0x001E:
            I += Vx;
            break;

        // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
        case 0x0029:
            /// TODO implement
            assert(false);
            break;

        // Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
        case 0x0033:
            /// TODO implement
            assert(false);
            break;

        // Fx55  stores [V0, ..., VX] in memory starting at address I; I is increased by 1 for each value
        //       => register dump
        case 0x0055:
            for (int reg = V0; reg <= x; ++reg) {
                memory[I + reg] = V[reg];
            }
            I += x + 1;
            break;

        // Fx65  fills [V0, ...,  VX] with values from memory starting at address I; I is increased by 1 for each value
        //       => register load
        case 0x0065:
            for (int reg = V0; reg <= x; ++reg) {
                V[reg] = memory[I + reg];
            }
            I += x + 1;
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