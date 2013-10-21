#include <cstdio>
#include <cstdint>

#include "SDL.h"
#include "TechnikaSong.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [file]\n", argv[0]);
        return -1;
    }

    SDL_Init(SDL_INIT_TIMER);

    TechnikaSong song(argv[1]);
    song.play();

    SDL_Quit();

    return 0;
}
