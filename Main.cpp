#include <iostream>
#include <iomanip> // for std::setfill, std::setw...
#include <assert.h>
#include <fstream>

#include "Chip8.h"

// sdl2 is used for graphics
#include "sdl2/SDL.h"
#undef main
#pragma comment(lib, "SDL2.lib")
#define SDL_FAILURE(msg) std::cout << "SDL: " << msg << " ; error ==" << SDL_GetError() << '\n';


int main(int argc, char** argv)
{
    // check if the file fits into the chip8 memory
    std::ifstream file("PONG.rom", std::ios::binary | std::ios::ate);
    auto fileSize = file.tellg();
    if (fileSize > 4096 - StartAddress) {
        assert(false);
        std::cout << "File too big! Expected " << 4096 - StartAddress << " bytes, got " << fileSize << " instead.\n";
        return EXIT_FAILURE;
    }

    // rewind to the beginning
    file.seekg(0, file.beg);

    // memory test
    {
        char buffer[4096];
        file.read(buffer, 4096);

        Chip8 chip;
        std::memcpy(chip.memory.data() + StartAddress, buffer, 4096);

        std::cout << std::hex;
        for (std::size_t n = 0; n < chip.memory.size(); ++n) {
            std::cout << "0x" << std::setfill('0') << std::setw(4);;
            std::cout << chip.memory[n] << '\n';
        }
        std::cout << std::dec;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_FAILURE("Init system");
        return EXIT_FAILURE;
    }

    auto screenWidth = Chip8::ScreenWidth * 100;
    auto screenHeight = Chip8::ScreenHeight * 100;

    /// TODO replace title with the game title!
    auto window = SDL_CreateWindow("Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_FAILURE("Init window");
        return EXIT_FAILURE;
    }

    auto renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        SDL_FAILURE("Init renderer");
        return EXIT_FAILURE;
    }

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

    return EXIT_SUCCESS;
}