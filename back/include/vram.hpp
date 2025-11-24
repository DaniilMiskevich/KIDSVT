#ifndef VRAM_HPP
#define VRAM_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <unordered_map>

class Vram {
   public:
    using Word = uint32_t;

    enum ErrType {
        NO = 0,
        STUCK_AT_0,
        STUCK_AT_1,
        // TODO! expand on this
    };
    using WordErrs = std::array<ErrType, sizeof(Word) * 8>;

   public:
    explicit Vram(size_t const len) : len(len), _data(new Word[len]) {
        for (size_t i = 0; i < len; i++) _data[i] = Word{};
    };
    // TODO implement copy and move constructor

    /// Gets a word at `i`ndex of the ram with set errors applied.
    Word read(size_t const i) const;
    /// Writes a word at `i`ndex to the ram with set errors applied.
    void write(size_t const i,
               Word const word);  // don't try converting into an operator

    inline ErrType get_error(size_t const i, unsigned const bit_i) { return _errors[i][bit_i]; }
    inline void set_error(size_t const i, unsigned const bit_i, ErrType const err) {
        _errors[i][bit_i] = err;
    }

    inline Word operator[](size_t const i) const { return read(i); }

   public:
    size_t const len;

   private:
    Word *const _data;
    std::unordered_map<size_t, WordErrs> _errors = {};
};

#endif
