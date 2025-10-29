#include "vram.hpp"

#include <cstddef>

template <typename Word>
Word Vram<Word>::read(size_t const addr);
void Vram<Word>::write(size_t const addr, Word const word);
