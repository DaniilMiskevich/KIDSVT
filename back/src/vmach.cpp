// TODO remove all the `printf`s when not needed
//
#include "vmach.hpp"

#include <algorithm>
#include <cstdio>
#include <optional>

template <typename T>
static std::optional<T> sane_stoull(std::string const &str) {
    static_assert(std::is_unsigned<T>());

    if (!std::isdigit(str[0])) return std::nullopt;
    try {
        return std::make_optional<T>(std::stoull(str, NULL, 16));
    } catch (std::invalid_argument const &e) { return std::nullopt; }
}

template <typename T>
static T sane_pop(std::stack<T> &stack) {
    T const t = std::move(stack.top());
    stack.pop();
    return t;
}

/************
 ** public **
 ************/

std::string const Vmach::opcode_loop = "loop";
std::string const Vmach::opcode_endloop = "endloop";
std::string const Vmach::opcode_asc = "asc";
std::string const Vmach::opcode_desc = "desc";

std::string const Vmach::opcode_then = "then";
std::string const Vmach::opcode_endthen = "endthen";

std::string const Vmach::opcode_equal = "equal?";
std::string const Vmach::opcode_assert = "assert!";

std::string const Vmach::opcode_read = "read";
std::string const Vmach::opcode_write = "write";

std::string const Vmach::opcode_swap = "swap";
std::string const Vmach::opcode_cur = "cur";
std::string const Vmach::opcode_last = "last";

std::string const Vmach::opcode_comment = "/*";
std::string const Vmach::opcode_endcomment = "*/";

void Vmach::step() {
    auto const op = next_op();
    auto const constant = sane_stoull<Word>(op);

    if (constant.has_value()) {
        op_const(constant.value());
    } else {
        if (op.empty())
            ;  // TODO program ended. now what?

        try {
            _ops.at(op)();
        } catch (std::out_of_range const &e) {
            printf("warn: UNKNOWN OPERATION: `%s`\n", op.c_str());
        }
    }
}

/*************
 ** private **
 *************/

std::unordered_map<std::string, std::string> const Vmach::_opcode_opposites = {
    {Vmach::opcode_loop, Vmach::opcode_endloop},
    {Vmach::opcode_endloop, Vmach::opcode_loop},
    {Vmach::opcode_then, Vmach::opcode_endthen},
    {Vmach::opcode_endthen, Vmach::opcode_then},

    {Vmach::opcode_comment, Vmach::opcode_endcomment},
    {Vmach::opcode_endcomment, Vmach::opcode_comment},
};

std::string Vmach::next_op() {
    std::string op;
    *program >> op;
    // makes the machine case-insensitive
    std::transform(op.begin(), op.end(), op.begin(), tolower);
    return op;
}
std::string Vmach::prev_op() {
    std::string op;
    do { program->seekg(-2, std::ios::cur); } while (!isspace(program->get()));
    do { program->seekg(-2, std::ios::cur); } while (isspace(program->get()));

    size_t const pos = program->tellg();
    op = next_op();
    program->seekg(pos);

    return op;
}

void Vmach::goto_matching_op(std::string const &target_op, int const dir) {
    if (dir < 0) prev_op();  // skip the last op

    int nesting_lvl = 1;
    std::string op;
    do {
        op = dir > 0 ? next_op() : prev_op();

        if (op == target_op)
            nesting_lvl--;
        else if (_opcode_opposites.count(target_op) && op == _opcode_opposites.at(target_op))
            nesting_lvl++;

        printf("next op: '%s'; nesting: %i\n", op.c_str(), nesting_lvl);
    } while (!op.empty() && nesting_lvl > 0);

    dir > 0 ? prev_op() : next_op();  // after the backward-searched / at the forward-searched
}

void Vmach::op_const(Word const constant) { stack.push(constant); }
void Vmach::op_loop() {
    printf("executing loop: ");
    _hidden_stack.push(i);
    printf("↑%i; ", i);

    i = sane_pop(stack);
    printf("i=%i\n", i);
}
void Vmach::op_endloop() {
    printf("reached ENDLOOP; ");
    if (i == 0) {
        i = sane_pop(_hidden_stack);
        printf("loop ended: i=0; ↓%i\n", i);
    } else {
        printf("i=%i; next iter\n", i);
        goto_matching_op(opcode_loop, -1);
    }
}
void Vmach::op_asc() { i = (i + 1) % _ram.len; }
void Vmach::op_desc() { i = (i - 1) % _ram.len; }

void Vmach::op_then() {
    Word const arg = sane_pop(stack);  // TODO? or `i`
    printf("THEN condition ");
    if (!arg) {
        printf("is false; skipping\n");
        goto_matching_op(opcode_endthen, 1);
    } else
        printf("is true; not skipping\n");
}
void Vmach::op_endthen() {
    printf("reached ENDTHEN\n");
    // TODO? probably should do something
}

void Vmach::op_assert() {
    printf("HELLO WORLD!\n");
    Word const arg = sane_pop(stack);
    if (!arg) halt();
}

void Vmach::op_comment() { goto_matching_op(opcode_endcomment, 1); }
void Vmach::op_endcomment() { printf("COMMENT ENDED\n"); }
