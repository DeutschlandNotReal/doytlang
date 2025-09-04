#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

enum class Code {
    // DataTypes
    _unknown,
    _eof,
    _litstr,
    _litnum,
    _ident,
    _nulltok, // Token doesn't actually exist

    // Punctuation
    _star,       // *   <- multiplication
    _obracket,   // [
    _cbracket,   // ]
    _obrace,     // {
    _cbrace,     // }
    _oparan,     // (
    _cparan,     // )
    _minus,      // -   <- subtruction
    _plus,       // +   <- addition
    _fslash,     // /   <- division
    _percent,    // %   <- modulo
    _and,        // &   <- and
    _pipe,       // |   <- or
    _under,      // _   <- typically unused, only counted as an _under if a single "_" string
    _hat,        // ^   <- xor
    _exc,        // !   <- not
    _qwe,        // ?   <- maybe ternary operations?
    _colon,      // :   <- end of statement
    _semi,       // ;   <- expression termination
    _comma,      // ,   <- expression seperation
    _dot,        // .   <- indexing
    _at,         // @   <- unplanned usage, many indexing??
    _hash,       // #   <- may be used for counting number of items in arrays/lists/dicts
    _tilde,      // ~   <- could cover for not, but ! does that

    // [<- This group is just for comparasons as expected
    _gt,         // >
    _lt,         // <
    _eq,         // =
    _gteq,       // >= 
    _lteq,       // <=
    _eqeq,       // ==
    _neq,        // !=


    _exc2,       // !!  <- no planned usage yet...
    _qwe2,       // ??  <- no planned usage yet...
    _star2,      // **  <- exponentiation
    _and2,       // &&  <- AND comparason (for &, I, ^, the single lettered versions are bitwise)
    _pipe2,      // ||  <- OR comparason
    _hat2,       // ^^  <- XOR comparason
    _gt2,        // >>  <- binary shift right
    _lt2,        // <<  <- binary shift left

    // Keywords
    _ret,       // the return keyword
    _set,       // assignment maybe
    _let,       // assignment maybe
    _get,       // to get libraries (maybe just from c++), usage: get 'library name as a literal string'
    _if,
    _else,
    _break,
    _continue,
    _for,
    _func
};

struct Token {
    Code code;
    std::variant<int, float, std::string> val;
    int index;

    int col;
    int line;

    std::string lexeme;

    void append(char c){
        lexeme.push_back(c);
    };

    static Token* create(int col, int line){
        auto t = new Token();
        t->col = col;
        t->line = line;
        return t;
    };

    static Token* eof(int col, int line){
        auto t = new Token();
        t->code = Code::_eof;
        t->line = line;
        t->col = col;
        return t;
    };

    static Token* null;
};

struct Source {
    std::string* src;
    int index;
    int size;
    
    int line;
    int col;

    char consume(){
        char c = (*src)[index];index++; 
        return c;
    }

    char advance(){
        char c = (*src)[index];
        index++;
        switch(c){case '\n': line++; col=1; break; default: col++;};
        return c;
    }

    void next(){index++;};
    void skip(){
        index++;return;
    }
    void move(int ahead){
        index+=ahead;
    }

    char peek(){
        return (*src)[index];
    }
    char lookahead(int ahead = 1){
        if (index+ahead>size){return '\0';};
        return (*src)[index + ahead];
    }
    bool islast(){
        return (index+1>=size);
    }
    bool isnextOOB(){
        return (index+2>size);
    }

    static Source* create(std::string* src){
        Source* s = new Source();
        s->src = src;
        s->index = 0;
        s->size = src->size();
        s->col = 0;
        s->line = 1;
    };
};

struct Lexer {
    std::vector<Token*> stream;
    int pointer;

    void emit(Token* tok){
        tok->index = stream.size();
        stream.push_back(tok);
    };

    Token* consume(){
        auto t = stream[pointer];
        pointer++;
        return t;
    };

    void retract(){
        pointer--;
    };

    Token* peek(){
        return stream[pointer];
    };

    Token* lookahead(int ahead = 1){
        if (pointer + ahead > stream.size()+1){
            return Token::null;
        };
        return stream[pointer+ahead];
    };

    bool islast(){
        return (pointer>stream.size());
    };



};


extern std::unordered_map<std::string, Code> operators;
extern std::unordered_map<std::string, Code> keywords;
Lexer *tokenize(std::string *src);