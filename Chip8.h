#pragma once

#include <array>

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

const Pixel Black        = 0;
const Pixel White        = 1;
const Byte  Pressed      = 1;

const Byte Font[] = {
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

// ref for Chip8 data taken from: https://en.wikipedia.org/wiki/Chip-8#Virtual_machine_description
struct Chip8 {

    const static Word StartAddress = 0x200;
    const static int  WorkingMemory = 4096 - StartAddress;

    // data
    Words<4096>  memory = {};
    Words<16>    stack = {};
    Pixels<2048> screen = {};
    Bytes<16>    key = {};

    // constants
    static constexpr Pixel ScreenWidth = 64;
    static constexpr Pixel ScreenHeight = 32;

    // register
    Words<16> V = {}; // V-register, data register
    Word      I = 0; // index register

    // indices
    Word   SI = 0;            // stack-index of current stack level
    Word   PC = StartAddress; // program counter

    // timer
    Word delayTimer = 0;
    Word soundTimer = 0;

    // methods
    void emulate(OpCode op);
    OpCode currentOp() const;

};

// full debug output
std::ostream& operator << (std::ostream& os, Chip8 const& c);