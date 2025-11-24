#include <cstdio>
#include <string>

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

    0 -1
    2 loop desc
        swap
        0 loop 
            cur write
            cur read equal?
            assert!
        asc endloop
    endloop
    drop drop

)END";

int main() {
    Vram vram(8);
    Vmach vmach(
        /* R"END(

        1
        0 loop
            cur write
            lshift
        asc endloop
        drop

        1
        0 loop
            cur read
            equal? assert!
            lshift
        asc endloop
        drop

        )END", */
        demoprogram,
        vram
    );
    for (size_t i = 0; i < 3000; i++) { vmach.step(); }
    printf("=====ENDED======\nstate: %i\n", vmach.state());

    for (unsigned i = 0; i < vram.len / PRINT_COLS; i++) {
        for (unsigned j = 0; j < PRINT_COLS; j++) printf("%08X ", vram.read(i * PRINT_COLS + j));
        printf("| ");
        for (unsigned j = 0; j < PRINT_COLS; j++) printword(vram.read(i * PRINT_COLS + j));
        printf("\n");
    }
    return 0;
}
