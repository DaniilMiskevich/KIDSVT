
#include <cstdio>
#include <string>

#include "conviniences.hpp"
#include "vmach.hpp"
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

char const *const demoprogram = R"END(
        0 1
        2 LOOP DESC
            SWAP
            0 LOOP 
                CUR WRITE
                CUR READ EQUAL?
                ASSERT!
            ASC ENDLOOP
        ENDLOOP
    )END";

int main() {
    Vram vram(32);

    char const str[] = "Hello world!";

    for (unsigned i = 0; i < lenof(str); i++) vram.write(i, str[i]);
    vram.set_error(3, 0, Vram::STUCK_AT_1);
    vram.set_error(4, 3, Vram::STUCK_AT_0);

    Vmach vmach("2 LOOP DESC    1 ASSERT!   ENDLOOP", vram);
    for (size_t i = 0; i < 20; i++) vmach.step();

    for (unsigned i = 0; i < vram.len / PRINT_COLS; i++) {
        for (unsigned j = 0; j < PRINT_COLS; j++) printf("%08lX ", vram.read(i * PRINT_COLS + j));
        printf("| ");
        for (unsigned j = 0; j < PRINT_COLS; j++) printword(vram.read(i * PRINT_COLS + j));
        printf("\n");
    }
    return 0;
}
