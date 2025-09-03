#include <variant>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include "lexer.h"

Code match_punctuation(char a, char b, Lexer* lex){

    #define inc lex->skip();

    switch(a){
        case '(': return Code::_oparan;
        case ')': return Code::_cparan;
        case '[': return Code::_obracket;
        case ']': return Code::_cbracket;
        case '{': return Code::_obrace;
        case '}': return Code::_cbrace;

        case '?': switch(b){
            case '?': inc; return Code::_qwe2;
            default:  return Code::_qwe;
        };
        case '|': switch(b){
            case '|': inc; return Code::_pipe2;
            default:  return Code::_pipe;
        };
        case '&': switch(b){
          case '&': inc; return Code::_and2;
          default:  return Code::_and;  
        };
        case '^': switch(b){
            case '^': inc; return Code::_hat2;
            default:  return Code::_hat;
        };
        case '=': switch(b){
            case '=': inc; return Code::_eqeq;
            default:  return Code::_eq;
        };
        case '>': switch(b){
            case '>': inc; return Code::_gt2;
            case '=': inc; return Code::_gteq;
            default:  return Code::_gt;
        };
        case '<': switch(b){
            case '<': inc; return Code::_lt2;
            case '=': inc; return Code::_lteq;
            default:  return Code::_lt;
        };
        case '!': switch(b){
            case '=': inc; return Code::_neq;
            case '!': inc; return Code::_exc2;
            default:  return Code::_exc;
        };

        case ':': return Code::_colon;
        case ';': return Code::_semi;
        case '.': return Code::_dot;
        case ',': return Code::_comma;
        case '#': return Code::_hash;
        case '@': return Code::_at;

        case '+': return Code::_plus;
        case '-': return Code::_minus;
        case '/': return Code::_fslash;
        case '%': return Code::_percent;

        case '*': switch(b){
            case '*': inc; return Code::_star2;
            default:  return Code::_star;
        };
    };
    return Code::_unknown;
};

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

Lexer* tokenize(std::string *src){
    Lexer *lex = Lexer::create(src);
    Token *tok = new Token{Code::_eof};

    int line = 1; int col = 0;

    char c = lex->nextchar();
    do{
        switch(c){
            case '\n':line++;col=1;break;
            default:col++;
        };
        
        int i = lex->src_pos;
        Code punct = match_punctuation(c, lex->charahead(1), lex);
        if (punct != Code::_eof){ // Punctuation Match
            tok->code = punct;
            tok->lexeme = std::string(1, c);

            if (i != lex->src_pos){
                tok->lexeme.push_back(lex->curchar());
            };

            tok = lex->emit(tok);
            continue;
        };
        
        // String literals
        if (c == '\''){
            tok->code = Code::_litstr;
            char c = lex->nextchar();
            col++;
            do{
                if (c == '\''){
                    break;
                } else if(c == '\\'){
                    tok->lexeme.push_back(match_escapechar(lex->charahead()));
                    if (lex->charahead() == 'n'){
                        line++;
                    };
                    lex->skip();col++;
                } else {
                    tok->lexeme.push_back(c);
                }
                c = lex->nextchar();col++;
            } while (!lex->islast() && (c != '\''));
            if (lex->islast() && (c!='\'')){
                throw std::runtime_error("Unterminated string literal.");
            };

            tok->val = tok->lexeme;
            tok = lex->emit(tok); continue;
        };

        // Number literals
        if (std::isdigit(c)){
            tok->code = Code::_litnum;
            bool dot = false;
            do{
                dot = dot || c == '.';
                tok->lexeme.push_back(c);
                c = lex->nextchar();col++;
            } while (!lex->islast() && !isspace(c) && !(dot && c == '.'));
            tok->val = std::stof(tok->lexeme);
            tok = lex->emit(tok); continue;
        };

        // Identifiers
        if (std::isalpha(c) || c == '_'){
            tok->code = Code::_ident;
            do{
                tok->lexeme.push_back(c);
                c = lex->nextchar();col++;
            } while (!lex->islast() && !isspace(c) && (isalnum(c) || c == '_'));

            if(keyword_map.count(tok->lexeme) != 0){
                // Is a keyword
                tok->code = keyword_map[tok->lexeme];
            } else {
                // Regular identifier
                tok->val = tok->lexeme;
            };

            tok = lex->emit(tok); continue;
        };

        lex->nextchar();
    } while (!lex->islast());
    
    return lex;
};