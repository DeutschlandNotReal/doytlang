#pragma once
#include <vector>
#include <cstdint>
#include <math.h>

enum class Opcode: uint8_t {
    // Arithmetic
    ADD, SUB, MUL, DIV, MOD,
    NEG, EXP, LN, SQRT,
    sADD, sSUB, sMUL, sDIV, sMOD, // variable op= expression, such as a /= 3

    // Load
    LOAD, LOADk,

    PRINT,

    CALL,

    // Control
    SLEEP, HALT
};

struct Instruction {
    Opcode code;
    uint8_t a;
    uint8_t b;
    uint8_t c;
};

struct doytVM {
    size_t i_inx;
    std::vector<double> reg;
    std::vector<Instruction> ins;

    inline Instruction current() const{
        return ins[i_inx];
    }

    inline void advance() {i_inx++;}
};