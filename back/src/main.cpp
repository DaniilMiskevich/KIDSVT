#include <cstdio>
#include <fstream>

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

/*
 * */

int main() {
    std::istream *program =

        // new std::istringstream(
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
        // R"END(

        //     0DEAD 0BEEF

        //     last @dump drop

        //     cur @dump drop

        //     )END"
        // );
        new std::ifstream("./res/march_x.whatever");

    Vram vram(8);
    Vmach vmach(program, vram);

    while (vmach.state() == Vmach::OK || vmach.state() == Vmach::HALTED) {
        while (vmach.state() == Vmach::OK) vmach.step();

        if (vmach.state() == Vmach::HALTED) {
            printf("\n===== PROGRAM HALTED =====\n");

            char choise;
            printf("Continue? "), scanf(" %c", &choise);
            if (choise != 'y') break;
            vmach.unhalt();
        }
    }
    char const *const states[] = {
        "early",
        "halted",
        "successfully",
        "program error!",
        "stack underflow!",
    };
    printf("\n===== PROGRAM ENDED: %s =====\n\n", states[vmach.state()]);

    for (unsigned i = 0; i < vram.len / PRINT_COLS; i++) {
        for (unsigned j = 0; j < PRINT_COLS; j++) printf("%08X ", vram.read(i * PRINT_COLS + j));
        printf("| ");
        for (unsigned j = 0; j < PRINT_COLS; j++) printword(vram.read(i * PRINT_COLS + j));
        printf("\n");
    }
    return 0;
}
