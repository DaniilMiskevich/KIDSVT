#include "vram.hpp"

#include <cstddef>
#include <stdexcept>

Vram::Word Vram::read(size_t const i) const {
    Word word = _data[i];

    try {
        auto const errs = _errors.at(i);
        for (unsigned i = 0; i < errs.size(); i++) switch (errs[i]) {
            case NO: break;
            case STUCK_AT_0: word &= ~(1 << i); break;
            case STUCK_AT_1: word |= (1 << i); break;
            }
    } catch (std::out_of_range const e) {}

    return word;
}

void Vram::write(size_t const i, Word const word) { _data[i] = word; }
