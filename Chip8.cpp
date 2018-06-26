#include <iostream>
#include <array>
#include <cstdint>

// typedefs
using Byte = uint8_t;  // big-endian
using Word = uint16_t; // for this emulator, a word is defined as two bytes
using OpCode = uint16_t; // max 35
using Pixel = uint8_t;  // chip8 has only black/white as color

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
    Word   SI = 0; // stack-index of current stack level
    Word   PC = 0; // program counter

    // timer
    Byte delayTimer = 0;
    Byte soundTimer = 0;

    // methods
    /// void step(); // emulate one step?
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

    return os;
}


// -------------------------------------------------
int main(int argc, char** argv)
{
    Chip8 chip = {};
    std::cout << chip << '\n';
    return 0;
}
// -------------------------------------------------



