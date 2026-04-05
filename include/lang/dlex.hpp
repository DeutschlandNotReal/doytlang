#pragma once

#include <lang/pool.hpp>

enum class TokenCode : char {
    // Control Tokens:
    _EOF = 0,   // End of File

    // Keyword Tokens:
    RET,       // return
    FUNC,      // function declaration
    GET,      // get (analogue for #include or require())
    IF,
    ELSE,
        
    // Literals
    IDENTITY,    // Identity
    NUMBER,      // Number, temporary but really just a long long (64b)
    STRING,      // String
    CHAR,        // Character
    BOOL,        // Boolean (true/false)
    NIL,         // Null

    // Punctuation
    PARAN_L, PARAN_R, // ( and )
    CURLY_L, CURLY_R, // { and }
    BRACK_L, BRACK_R, // [ and ]

    PLUS, MINUS, STAR, SLASH, // + - * /
    DOT, COMMA, // . ,

    GTEQ, LTEQ, // >= <=
    EQ2, EQ,    // == =
    GT2, LT2,   // >> <<
    NEQ,         // !=
};

class Token {
    TokenCode tcode;
    void* data;
    public:
        TokenCode code() const noexcept { return tcode; }

        template<typename T> T value() = delete;
};

class LexOutput {

};