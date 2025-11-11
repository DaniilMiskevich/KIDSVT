#ifndef VMACH_HPP
#define VMACH_HPP

#include <cstdint>
#include <functional>
#include <sstream>
#include <stack>
#include <string>

#include "vram.hpp"

#define VMACH_CMD_SEP ('\n')

// TODO? probably unnecessary?
static std::string cleanup_program(std::string const &program) {
    // char replacement[2] = {VMACH_CMD_SEP};
    // maybe not a best way to do this, but it works for now
    // return std::regex_replace(program, std::regex("\\s+"), replacement);

    return program;
}

class Vmach {
   public:
    typedef uint16_t Word;

   public:
    static_assert(
        VMACH_CMD_SEP != '\0',
        "As program is a string, VMACH_CMD_SEP must not be '\\0'."
    );
    Vmach(std::string const &program, Vram &ram)
    : program(new std::istringstream(cleanup_program(program))), _ram(ram) {}
    Vmach(Vmach const &other) = delete;
    Vmach(Vmach const &&other) = delete;
    ~Vmach() { delete program; }

    inline bool is_running() { return _is_running; }
    inline void run() { _is_running = true; }
    inline void halt() { _is_running = false; }

    void step();

   private:
    std::string next_op();
    /// Goes to the matching `op` in the provided `direction`.
    /// When looking backward, searches for the position after the word.
    void goto_matching_op(char const *const op, int const direction);

    void op_const(Word const value);
    void op_loop(), op_endloop();
    void op_asc(), op_desc();
    void op_then(), op_endthen();
    void op_equal();
    void op_assert();
    void op_read(), op_write();
    void op_swap();
    void op_cur(), op_last();
    // TODO more ops : logical and arithmetic

   public:
    std::istream *const program;

    /// This is just a general register of a full size. The only ops that `mod RAM_SIZE`
    /// are `ASC` and `DESC`. Everything else treats this as a full binary number.
    Word i = 0;
    std::stack<Word> stack;

   private:
    bool _is_running = false;

    Vram &_ram;
    std::stack<Word> _hidden_stack;
    std::unordered_map<std::string, std::function<void()>> const _ops = {
        {"loop", [this]() { this->op_loop(); }},
        {"endloop", [this]() { this->op_endloop(); }},
        {"asc", [this]() { this->op_asc(); }},
        {"desc", [this]() { this->op_desc(); }},

        {"then", [this]() { this->op_then(); }},
        {"endthen", [this]() { this->op_endthen(); }},

        // {"equal?", [this]() { this->op_equal(); }},
        {"assert!", [this]() { this->op_assert(); }},

        // {"read", [this]() { this->op_read(); }},
        // {"write", [this]() { this->op_write(); }},

        // {"swap", [this]() { this->op_swap(); }},
        // {"cur", [this]() { this->op_cur(); }},
        // {"last", [this]() { this->op_last(); }},

        // TODO rest
    };
};

#endif
