#include <variant>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include "lexer.h"

std::pair<Code, std::string> match_punctuation(char a, char b, Source* src){
    std::string lexeme(1, a);

    #define inc { src->skip(); lexeme.push_back(b); }
    #define returnl(c) return std::make_pair(c, lexeme)

    switch(a){
        case '(': returnl(Code::_oparan);
        case ')': returnl(Code::_cparan);
        case '[': returnl(Code::_obracket);
        case ']': returnl(Code::_cbracket);
        case '{': returnl(Code::_obrace);
        case '}': returnl(Code::_cbrace);

        case '?': switch(b){
            case '?': inc; returnl(Code::_qwe2);
            default: returnl(Code::_qwe);
        };
        case '|': switch(b){
            case '|': inc; returnl(Code::_pipe2);
            default: returnl(Code::_pipe);
        };
        case '&': switch(b){
            case '&': inc; returnl(Code::_and2);
            default: returnl(Code::_and);  
        };
        case '^': switch(b){
            case '^': inc; returnl(Code::_hat2);
            default: returnl(Code::_hat);
        };
        case '=': switch(b){
            case '=': inc; returnl(Code::_eqeq);
            default: returnl(Code::_eq);
        };
        case '>': switch(b){
            case '>': inc; returnl(Code::_gt2);
            case '=': inc; returnl(Code::_gteq);
            default: returnl(Code::_gt);
        };
        case '<': switch(b){
            case '<': inc; returnl(Code::_lt2);
            case '=': inc; returnl(Code::_lteq);
            default: returnl(Code::_lt);
        };
        case '!': switch(b){
            case '=': inc; returnl(Code::_neq);
            case '!': inc; returnl(Code::_exc2);
            default: returnl(Code::_exc);
        };

        case ':': returnl(Code::_colon);
        case ';': returnl(Code::_semi);
        case '.': returnl(Code::_dot);
        case ',': returnl(Code::_comma);
        case '#': returnl(Code::_hash);
        case '@': returnl(Code::_at);

        case '+': returnl(Code::_plus);
        case '-': returnl(Code::_minus);
        case '/': returnl(Code::_fslash);
        case '%': returnl(Code::_percent);

        case '*': switch(b){
            case '*': inc; returnl(Code::_star2);
            default: returnl(Code::_star);
        };
    };
    return std::make_pair(Code::_unknown, lexeme);
}

char match_escapechar(char esccode){
    switch(esccode){
        case '\\': return esccode;
        case '\'': return esccode;
        case 't':  return '\t';
        case 'n':  return '\n';
        case 'v':  return '\v';
    };
    return esccode;
};

std::unordered_map<std::string, Code> keyword_map {
    {"return", Code::_ret},
    {"func", Code::_func},
    {"if", Code::_if},
    {"else", Code::_else},
    {"continue", Code::_continue},
    {"break", Code::_break},
    {"for", Code::_for},
    {"func", Code::_func}
};

Lexer* tokenize(std::string *strsrc){
    Source *src = Source::create(strsrc);
    Lexer *lex = new Lexer();

    while (!src->islast()){
        char c = src->consume();

        // Comment checking (comment sequence: /*())
        if (c == '/' && src->lookahead(1) == '*'){
            src->consume(); // consumes *
            char termination = '/n';
            if (src->lookahead(1) == '('){termination = ')';};

            do{
                c = src->consume();
            } while(c != termination || src->index >= src->size);
        };
        
        // Spaces always skipped
        if (isspace(c)){continue;};

        // Punctuation match check
        int line = src->line; int col = src->col;
        auto [puncmatch, punclexeme] = match_punctuation(c, src->lookahead(), src);
        if (puncmatch != Code::_unknown){
            lex->emit(new Token{puncmatch, 0, 0, col, line, punclexeme});
            continue;
        };

        // Number literals
        if (isdigit(c)){
            auto t = new Token{Code::_litnum, 0, 0, col, line, ""};
            int hasdot = false;
            int rollback = 0;
            do{
                c = src->peek();
                if (isdigit(c)){
                    t->lexeme.push_back(c);
                }
                else if (c == '.'){
                    if (hasdot){
                        rollback = 1;
                    } else {
                        hasdot = true;
                        t->lexeme.push_back(c);
                    };
                }
                else {
                    rollback = 1;
                };

                if (rollback>0){break;};
                src->next();
                if (src->islast()){break;};
            } while (!isspace(c));
            t->val = std::stof(t->lexeme);
            lex->emit(t);
            src->move(-rollback);
        };

    };
    
    return lex;
};