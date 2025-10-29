#ifndef VRAM_HPP
#define VRAM_HPP

#include <cstddef>
#include <cstdlib>

template <typename Word>
class Vram {
   public:
    Vram(size_t const word_count)
    : word_count(word_count), _data(new Word[word_count]) {};

    Word read(size_t const addr);
    void write(size_t const addr, Word const word);

    inline Word const &operator[](size_t const addr) { return read(addr); }

   public:
    size_t const word_count;

   private:
    Word *const _data;
};

#endif
