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
};

struct Lexer {
    std::vector<Token*> toks;

    int tok_size; // Token Count
    int src_size; // Source character Count
    int tok_pos; // Token location (index)
    int src_pos; // Src location (index)

    std::string *src;
    std::string lexeme;

    Token* curtok;

    static Lexer* create(std::string *src){
        Lexer* lex = new Lexer();
        lex->src = src;
        lex->src_size = src->size();
        lex->tok_pos = 0;
        lex->src_pos = 0;
        lex->tok_size = 0;
        lex->lexeme = nullptr;
        return lex;
    };

    // emit() returns a blank, _eof token
    Token* emit(Token* tok){src_pos++; toks.push_back(tok); return new Token{Code::_eof};};
    bool   islast(){return src_pos >= src_size-1;};

    void   skip(){src_pos++;}
    char   nextchar(){src_pos++; if(src_pos > src_size){return '\0';}char c = (*src)[src_pos]; return c;}
    char   charahead(int ahead = 1){if(src_pos+ahead>src_size){return'\0';}return(*src)[src_pos + ahead];};
    char   curchar(){return (*src)[src_pos];};
    void   skipahead(int ahead = 1){src_pos+=ahead;};

    Token* consume(){return toks[tok_pos++];}
    Token* retract(){auto last = toks.back(); tok_pos--; toks.pop_back(); return last;}

    Token* peek(){return toks[tok_pos];}
    Token* next(){return toks[tok_pos+1];}
    Token* lookahead(int ahead){return toks[tok_pos+ahead];}

};

extern std::unordered_map<std::string, Code> operators;
extern std::unordered_map<std::string, Code> keywords;
Lexer *tokenize(std::string *src);