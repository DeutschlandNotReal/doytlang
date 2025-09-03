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
    _star,       // *
    _obracket,   // [
    _cbracket,   // ]
    _obrace,     // {
    _cbrace,     // }
    _oparan,     // (
    _cparan,     // )
    _minus,      // -
    _plus,       // +
    _fslash,     // /
    _percent,    // %
    _and,        // &
    _pipe,       // |
    _under,      // _
    _hat,        // ^
    _exc,        // !
    _qwe,        // ?
    _colon,      // :
    _semi,       // ;
    _comma,      // ,
    _dot,        // .
    _at,         // @
    _hash,       // #

    _gt,         // >
    _lt,         // <
    _eq,         // =

    _gteq,       // >= 
    _lteq,       // <=
    _eqeq,       // ==
    _neq,        // !=
    _exc2,       // !!
    _qwe2,       // ??
    _star2,      // *
    _and2,       // &&
    _pipe2,      // ||
    _hat2,       // ^^
    _gt2,        // >>
    _lt2,        // <<

    // Keywords
    _ret,
    _set,
    _let,
    _get,
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

    private:
        int readuntil;
        void checkchar(char c){
            if (index < readuntil){return;};
            readuntil = index;
            switch(c){
                case '\n':{line++; col = 0; break;}; default:{col++; break;};
            };
        };
    public:

    char consume(){
        char c = (*src)[index];index++; 
        checkchar(c);
        return c;
    }

    void next(){index++;};
    void skip(){
        checkchar((*src)[index]);
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
        return (index>size);
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