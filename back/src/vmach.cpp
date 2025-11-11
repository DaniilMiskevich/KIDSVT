#include "vmach.hpp"

#include <algorithm>
#include <cstdio>
#include <optional>
#include <thread>

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

std::string Vmach::next_op() {
    std::string op;
    *program >> op;
    // makes the machine case-insensitive
    std::transform(op.begin(), op.end(), op.begin(), tolower);
    return op;
}

void Vmach::goto_matching_op(char const *const op, int const direction) {
    // TODO! should account for the right number of commands for nesting
    // (e.g LOOP LOOP ... ENDLOOP ENDLOOP)

    std::string cmd;
    do {
        do {
            program->seekg(2 * direction /*TODO wrong if |direction|>1*/, std::ios::cur);
        } while (!isspace(program->get()));
        size_t const pos = program->tellg();
        cmd = next_op();
        program->seekg(pos, std::ios::beg);
    } while (cmd != std::string(op));
    next_op();  // TODO remove the hack
}

void Vmach::op_const(Word const constant) { stack.push(constant); }
void Vmach::op_loop() {
    _hidden_stack.push(i);

    i = sane_pop(stack);
}
void Vmach::op_endloop() {
    if (i == 0) {
        i = sane_pop(_hidden_stack);
    } else {
        goto_matching_op("loop", -1);
    }
}
void Vmach::op_asc() { i = (i + 1) % _ram.len; }
void Vmach::op_desc() { i = (i - 1) % _ram.len; }

void Vmach::op_then() {
    Word const arg = sane_pop(stack);  // TODO? or `i`
    if (!arg) goto_matching_op("endthen", 1);
}
void Vmach::op_endthen() {
    // TODO? probably should do something
}

void Vmach::op_assert() {
    printf("HELLO WORLD!\n");  // TODO for testing; remove later
    Word const arg = sane_pop(stack);
    if (!arg) halt();
}
