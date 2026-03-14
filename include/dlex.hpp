#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream> 
#include <sstream>
#include <cstdint>
#include "arena.hpp"


enum class dTCode : uint_fast8_t {
    // Control Tokens:
    invT = 0,  // Invalid token, placeholder
    eofT,      // End of File
    unkT,      // Unknown character, still lexed...

    // Keyword Tokens:
    retT,      // return
    fncT,      // f prefix for functions
    getT,      // get (analogue for #include or require())
    
    // Literals
    identT,    // Identity
    numT,      // Number, temporary but really just a long
    strT,      // String
    charT,     // Character
    boolT,     // Boolean (true/false)
    nullT,     // Null

    // Punctuation
    lparT, rparT, // ( and )
    lcurT, rcurT, // { and }
    lsqrT, rsqrT, // [ and ]

    plusT, minusT, starT, slashT, // + - * /
    dot, comma, // . ,

    gteqT, lteqT, // >= <=
    eq2T, eqT,    // == =
    gt2T, lt2T,   // >> <<
    neqT,         // !=

};

union dTPayload {
    float flt;
};

struct Token {
    dTCode type;
    dTPayload pl;
};

// Produced by lexer, used by parser
class LexOutput {
    private:
        Arena literals;
        std::vector<Token> stream;
        size_t cursor;
    public:
        Token peek(size_t offset = 0);
        Token consume();
};

LexOutput tokenize(std::string src);