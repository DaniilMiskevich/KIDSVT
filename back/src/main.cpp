
#include <cstdio>

#include "conviniences.hpp"
#include "vram.hpp"

#define PRINT_COLS (4)

static int printword(Vram::Word const word) {
    return printf(
        "%c%c%c%c ",
        (uint8_t)(word >> 24),
        (uint8_t)(word >> 16),
        (uint8_t)(word >> 8),
        (uint8_t)(word >> 0)
    );
}

int main() {
    Vram vram(32);

    char const str[] = "Hello world!";

    for (unsigned i = 0; i < lenof(str); i++) vram.write(i, str[i]);
    vram.set_error(3, 0, Vram::STUCK_AT_1);
    vram.set_error(4, 3, Vram::STUCK_AT_0);

    for (unsigned i = 0; i < vram.word_count / PRINT_COLS; i++) {
        for (unsigned j = 0; j < PRINT_COLS; j++)
            printf("%08lX ", vram.read(i * PRINT_COLS + j));
        printf("| ");
        for (unsigned j = 0; j < PRINT_COLS; j++)
            printword(vram.read(i * PRINT_COLS + j));
        printf("\n");
    }
    return 0;
}
