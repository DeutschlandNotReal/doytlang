#pragma once

#include <ostream>
#include <cstring>
#include <lang/pool.hpp>
#include <lang/view.hpp>

enum class TokenCode : char {
    // Control Tokens:
    _EOF = 0,   // End of File

    // Keyword Tokens:
    RET,       // return
    FUNC,      // function declaration
    GET,      // get (analogue for #include or require())
    IF,
    ELSE,
    WHILE,
    BREAK,
    CONTINUE,
        
    // Literals
    IDENTITY,    // Identity
    NUMBER,      // Number, temporary but really just a float
    STRING,      // String
    CHAR,        // Character
    BOOL,        // Boolean (true/false)
    NIL,         // Null

    // Punctuation
    PARAN_L, PARAN_R, // ( and )
    CURLY_L, CURLY_R, // { and }
    BRACK_L, BRACK_R, // [ and ]

    PLUS, MINUS, STAR, SLASH, // + - * /
    DOT, COMMA, SEMI, // . , ;
    EXC, // !

    GTEQ, LTEQ, // >= <=
    EQ2, EQ,    // == =
    GT2, LT2,   // >> <<
    GT, LT,     // > <
    NEQ,         // !=
};

class LexOutput;

// pointer to char pointer so that many sources can be compiled as one
LexOutput tokenize(const char** src, int src_count, int flags = 0);

class Token {
    TokenCode tcode;
    void* data = nullptr;

    public:
        TokenCode code() const noexcept { return tcode; }
        Token(): tcode(TokenCode::_EOF) {}
        Token(TokenCode code): tcode(code) {}
        template <typename T> Token(TokenCode code, T val): tcode(code) {
            if (sizeof(T) > 8) data = new char[sizeof(T)];
            
            std::memcpy(data, &val, sizeof(T));
        }
        
        template <typename T> T value() const {
            T val;
            std::memcpy(&val, (sizeof(T) > 8) ? data : &data, sizeof(T));

            return val;
        }

        std::ostream& print(std::ostream& stream) const;
};

class LexOutput {
    friend LexOutput tokenize(const char**, int, int);
    friend int main(int, const char**);
    RawPool pool;
    Pool<Token> token_pool;
    int tok_count = 0;
    void emit(TokenCode code) {
        ++ tok_count;
        token_pool.emplace(code);
    }

    template <typename T> void emit(TokenCode code, T value) {
        ++ tok_count;
        token_pool.emplace(code, value);
    }

    public:
        int count() const { return tok_count; }
        const Token& peek() const;
        const Token& consume();
};