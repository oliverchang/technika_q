#include <cstdio>
#include <cstdint>

#include "TechnikaSong.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s [file]\n", argv[0]);
        return -1;
    }

    TechnikaSong song(argv[1]);
    song.play();

    return 0;
}
