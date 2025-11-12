#ifndef VMACH_HPP
#define VMACH_HPP

#include <cstdint>
#include <functional>
#include <sstream>
#include <stack>
#include <string>

#include "vram.hpp"

class Vmach {
   public:
    typedef uint16_t Word;

   public:
    Vmach(std::string const &program, Vram &ram)
    : program(new std::istringstream(program)), _ram(ram) {}
    Vmach(Vmach const &other) = delete;
    Vmach(Vmach const &&other) = delete;
    ~Vmach() { delete program; }

    inline bool is_running() { return _is_running; }
    inline void run() { _is_running = true; }
    inline void halt() { _is_running = false; }

    void step();

   private:
    std::string next_op(), prev_op();
    /// Goes to the matching `op` in the provided `direction`.
    /// When looking backward, searches for the position after the word.
    void goto_matching_op(std::string const &op, int const direction);

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
    static std::string const opcode_loop;
    static std::string const opcode_endloop;
    static std::string const opcode_asc;
    static std::string const opcode_desc;

    static std::string const opcode_then;
    static std::string const opcode_endthen;

    static std::string const opcode_equal;
    static std::string const opcode_assert;

    static std::string const opcode_read;
    static std::string const opcode_write;

    static std::string const opcode_swap;
    static std::string const opcode_cur;
    static std::string const opcode_last;

    std::istream *const program;

    /// This is just a general register of a full size. The only ops that `mod RAM_SIZE`
    /// are `ASC` and `DESC`. Everything else treats this as a full binary number.
    Word i = 0;
    std::stack<Word> stack;

   private:
    static std::unordered_map<std::string, std::string> const _opcode_opposites;

    bool _is_running = false;

    Vram &_ram;
    std::stack<Word> _hidden_stack;

    std::unordered_map<std::string, std::function<void()>> const _ops = {
        {opcode_loop, [this]() { this->op_loop(); }},
        {opcode_endloop, [this]() { this->op_endloop(); }},
        {opcode_asc, [this]() { this->op_asc(); }},
        {opcode_desc, [this]() { this->op_desc(); }},

        {opcode_then, [this]() { this->op_then(); }},
        {opcode_endthen, [this]() { this->op_endthen(); }},

        // {opcode_equal, [this]() { this->op_equal(); }},
        {opcode_assert, [this]() { this->op_assert(); }},

        // {opcode_write, [this]() { this->op_read(); }},
        // {opcode_read, [this]() { this->op_write(); }},

        // {opcode_swap, [this]() { this->op_swap(); }},
        // {opcode_cur, [this]() { this->op_cur(); }},
        // {opcode_last, [this]() { this->op_last(); }},
    };
};

#endif
