#ifndef VMACH_HPP
#define VMACH_HPP

#include <functional>
#include <map>
#include <sstream>
#include <stack>
#include <string>

#include "vram.hpp"

class Vmach {
   public:
    using Word = Vram::Word;
    enum State {
        OK,
        HALTED,
        PROGRAM_ENDED,

        ASSERTION_FAILED,
        STACK_UNDERFLOW,
    };

   public:
    Vmach(std::string const &program, Vram &ram)
    : program(new std::istringstream(program)), _ram(ram) {
        reset();
    }
    Vmach(Vmach const &other) = delete;
    Vmach(Vmach const &&other) = delete;
    ~Vmach() { delete program; }

    inline State state() const { return _state; }
    inline void reset() {
        i = 0;
        while (!stack.empty()) stack.pop();
        while (!_hidden_stack.empty()) _hidden_stack.pop();

        _state = OK;
        program->clear(), program->seekg(0, std::ios::beg);
    }
    inline void halt() { _state = HALTED; }

    void step();
    void dump_stack() const;

   private:
    std::string next_op(), prev_op();
    /// Goes to the matching `op` in the provided `direction`.
    /// When looking backward, searches for the position after the word.
    void goto_matching_op(std::string const &op, int const direction);

    Word stack_pop();
    void stack_push(Word const value);

    void op_const(Word const value);
    void op_loop(), op_endloop();
    void op_asc(), op_desc();
    void op_then(), op_endthen();
    void op_assert();
    void op_read(), op_write();
    void op_swap(), op_drop();
    void op_cur(), op_last();
    void op_equal(), op_greater(), op_less();
    void op_not(), op_xor(), op_or(), op_and(), op_lshift();
    void op_add(), op_neg();
    void op_push_i(), op_pop_i();

   public:
    static std::string const opcode_loop, opcode_endloop;
    static std::string const opcode_asc, opcode_desc;
    static std::string const opcode_then, opcode_endthen;
    static std::string const opcode_assert;
    static std::string const opcode_read, opcode_write;
    static std::string const opcode_swap, opcode_drop;
    static std::string const opcode_cur, opcode_last;
    static std::string const opcode_equal, opcode_greater, opcode_less;
    static std::string const opcode_not, opcode_xor, opcode_or, opcode_and, opcode_lshift;
    static std::string const opcode_add, opcode_neg;
    static std::string const opcode_push_i, opcode_pop_i;

    std::istream *const program;

    /// This is just a general register of a full size. The only ops that `mod RAM_SIZE`
    /// are `ASC` and `DESC`. Everything else treats this as a full binary number.
    Word i = 0;
    std::stack<Word> stack;

   private:
    static std::map<std::string, std::string> const _opcode_opposites;

    State _state = OK;

    Vram &_ram;
    std::stack<Word> _hidden_stack;

    std::map<std::string, std::function<void()>> const _ops = {
        {opcode_loop, [this]() { this->op_loop(); }},
        {opcode_endloop, [this]() { this->op_endloop(); }},
        {opcode_asc, [this]() { this->op_asc(); }},
        {opcode_desc, [this]() { this->op_desc(); }},

        {opcode_then, [this]() { this->op_then(); }},
        {opcode_endthen, [this]() { this->op_endthen(); }},

        {opcode_assert, [this]() { this->op_assert(); }},

        {opcode_read, [this]() { this->op_read(); }},
        {opcode_write, [this]() { this->op_write(); }},

        {opcode_swap, [this]() { this->op_swap(); }},
        {opcode_drop, [this]() { this->op_drop(); }},
        {opcode_cur, [this]() { this->op_cur(); }},
        {opcode_last, [this]() { this->op_last(); }},

        {opcode_equal, [this]() { this->op_equal(); }},

        {opcode_not, [this]() { this->op_not(); }},
        {opcode_xor, [this]() { this->op_xor(); }},
        {opcode_and, [this]() { this->op_and(); }},
        {opcode_lshift, [this]() { this->op_lshift(); }},

        {opcode_add, [this]() { this->op_add(); }},
        {opcode_neg, [this]() { this->op_neg(); }},

        {opcode_push_i, [this]() { this->op_push_i(); }},
        {opcode_pop_i, [this]() { this->op_pop_i(); }},

        {"@dump", [this]() { this->dump_stack(); }}
    };
};

#endif
