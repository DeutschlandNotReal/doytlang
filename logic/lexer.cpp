#include <variant>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include <sstream>
#include <stdexcept>
#include <format>

#include "lexer.h"


std::pair<Code, std::string> match_punctuation(char a, char b, Source* src){
    std::string lexeme(1, a);

    #define inc { src->advance(); lexeme.push_back(b); }
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
        case '~': returnl(Code::_tilde);

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
        char c = src->advance();

        // Comment checking (comment sequence: /*())
        if (c == '/' && src->lookahead(1) == '*'){
            src->advance(); // consumes *
            char termination = '\n';
            if (src->lookahead(1) == '('){termination = ')';};

            do{
                c = src->advance();
            } while(c != termination && !src->isnextOOB());
        };

        // Punctuation check
        auto [punctcode, punctlexeme] = match_punctuation(c,src->lookahead(1), src);
        if (punctcode != Code::_unknown){
            lex->emit(new Token{punctcode, 0, 0, src->col, src->line, punctlexeme});
            continue;
        };

        // Spaces always skipped
        if (isspace(c)){
            continue;
        };

        // Strings
        if (c == '\''){
            auto stok = new Token{Code::_litstr, 0, 0, src->col, src->line};
            c = src->advance(); // consumes initial '
            do{

            } while(!src->isnextOOB() || c == '\'');
        };

        // Numbers
        if (isdigit(c)){
            auto dtok = new Token{Code::_litnum, 0, 0, src->col, src->line};
            src->col++;
            bool hasdot = false;

            do{
                if (c == '.'){
                    if (hasdot){
                        throw std::runtime_error("Malformed number");
                    }
                    else{hasdot=true;};
                };
                if (!isdigit(c)){
                    throw std::runtime_error("Malformed number");
                };

                dtok->lexeme.push_back(c);
                if (isspace(src->lookahead(1))){break;};

                c = src->advance();
            } while (!src->isnextOOB() && !isspace(c));
            dtok->val = std::stof(dtok->lexeme);
            lex->stream.push_back(dtok);
            continue;
        };

        // Identifiers
        if (isalpha(c) || c == '_'){
            auto itok = new Token{Code::_ident, 0, 0,src->col, src->line};
            do{
                itok->lexeme.push_back(c);

                if (isspace(src->lookahead(1))){break;};
                c = src->advance();
            } while (!src->isnextOOB() && !isspace(c));
            itok->val = itok->lexeme;
            lex->stream.push_back(itok);
            continue;
        };



    };
    
    return lex;
};