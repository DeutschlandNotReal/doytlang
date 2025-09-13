#include "../include/dlex.hpp"
#include <optional>
#include <iostream>
#include <string>

using namespace std;


inline TokCode match_punct(char a, char b, LexContext& context){
    #define retnext(t) context.src_index++; return t;

    switch(a){
        case '{': return TK_LBRACE; case '}': return TK_RBRACE;
        case '[': return TK_LBRACK; case ']': return TK_RBRACK;
        case '(': return TK_LPAREN; case ')': return TK_RPAREN;

        case '+': return TK_PLUS; case '-': return TK_MINUS; case '/': return TK_SLASH;
        case '%': return TK_PERC; case '#': return TK_HASH; case '?': return TK_QWE;
        case '.': return TK_DOT; case ',': return TK_COMMA; case ';': return TK_SEMI; case ':': return TK_COLON;
        case '^': return TK_HAT;

        case '&':switch(b){
            case '&': retnext (TK_AND2); default: return TK_AND;
        };
        case '|':switch(b){
            case '|': retnext (TK_PIPE2); default: return TK_PIPE;
        };
        case '*':switch(b){
            case '*': retnext (TK_STAR2); default: return TK_STAR;
        };
        case '>':switch(b){
            case '>': retnext (TK_GT2); case '=': retnext (TK_GTEQ); default: return TK_GT;
        };
        case '<':switch(b){
            case '<': retnext (TK_LT2); case '=': retnext (TK_LTEQ); default: return TK_LT;
        };
        case '=':switch(b){
            case '=': retnext (TK_EQ2); default: return TK_EQ;
        };
        case '!':switch(b){
            case '=': retnext (TK_NEQ); default: return TK_EXC;
        };
        default: return TK_EMPTY;
    };
};

static inline optional<float> interp_flt(string lexm){
    char* end; const char* cstr = lexm.c_str();

    float val = strtof(cstr, &end);
    
    if (end == cstr || *end != '\0'){
        return nullopt;
    }; return val;
};

static inline optional<double> interp_dub(string lexm){
    char* end; const char* cstr = lexm.c_str();

    double val = strtod(cstr, &end);
    
    if (end == cstr || *end != '\0'){
        return nullopt;
    }; return val;
};

static inline optional<long long> interp_llong(string lexm){
    char* end; const char* cstr = lexm.c_str();

    long long val = strtoll(cstr, &end, 0);
    
    if (end == cstr || *end != '\0'){
        return nullopt;
    }; return val;
};

static string vischar(char _c){
    switch(_c){
        case ' ': return "<space>";
        case '\n': return "<newline>";
        case '\t': return "<tab>";
        case '\r': return "<\\r>";
        default: return string(1, _c);
    };
};

void token_lctx(LexContext& lctx, int debug_flags){
    // debug flags: 0b[][][][show_msg]

    size_t char_maxext = 0;
    lctx.src_index = 0;

    char _c = lctx.cpeek();

    bool dmsg = (debug_flags & 0b0001) != 0;

    #define next _c = lctx.cnext(); if(dmsg){cout<<"\nOn Character " + vischar(_c);}

    // Impossible under normal circumstances to get more tokens than there are characters in the source + 1 (EOF t)

    #define lencheck if (lctx.tok_size + 1 > lctx.src_size){lctx.throw_err("Faulty Tokenstream size.");}

    while (1){
        _c = lctx.cpeek();
        if (dmsg){cerr << "\nConstructing from " << vischar(_c) << " at " << to_string(lctx.src_index);}
        if (_c == '\0'){lctx.emit(new Token{TK_EOF, lctx.line}); break;};

        if (lctx.src_index + 1 <= char_maxext){
            // Tokenizer not designed to roll back on characters.
            lctx.throw_err("Reached source index " + to_string(lctx.src_index) + " multiple times!");
        }
        
        lencheck;

        char_maxext = lctx.src_index + 1;
        
        if (isspace(_c)){
            int skiplen = 0;
            while (isspace(_c)){skiplen++; _c = lctx.cnext();}
            if (dmsg){cout << "\nSkipped " + to_string(skiplen) + " whitespaces.";}
            continue;
        }


        // Comment handling // or //()
        if (_c == '/' && lctx.cpeek(1) == '/'){
            char term = lctx.cpeek(2) == '(' ? ')' : '\n';
            lctx.src_index+=2; // consumes / and /

            while (_c != term && _c != '\0'){_c = lctx.cnext();}
            if (_c == term){next;}
            continue;
        }

        // Number Literals
        if (isdigit(_c) || (_c == '.' && isdigit(lctx.cpeek(1)))){
            string lexm;
            while (isalnum(_c) || _c == '.' || _c == '_' || (_c == '-' && (lexm.back() == 'e' || lexm.back() == 'E'))){
                if (_c != '_'){lexm.push_back(_c);}
                next;
            }
            if (lexm.size() < 1){
                lctx.throw_err("Empty literal number lexeme!");
            }

            int dtype = 0; // 0: undesignated, 1: int, 2: float, 3: double
            switch(lexm.back()){
                case 'i': lexm.pop_back(); dtype = 1; break; 
                case 'f': lexm.pop_back(); dtype = 2; break;
                case 'd': lexm.pop_back(); dtype = 3; break;
            }

            switch(dtype){
                case 0:{
                    auto val_ll = interp_llong(lexm);
                    if (val_ll.has_value()){
                        auto lltok = new Token{TK_LIT_INT, lctx.line}; lltok->pl.t_lng = val_ll.value();
                        lctx.emit(lltok);
                        continue;
                    }
                    auto val_f = interp_flt(lexm);
                    if (val_f.has_value()){
                        auto ftok = new Token{TK_LIT_FLOAT, lctx.line}; ftok->pl.t_flt = val_f.value();
                        lctx.emit(ftok);
                        continue;
                    } lctx.throw_err("Malformed number.");
                }
                case 1:{
                    auto val_ll = interp_llong(lexm);
                    if (val_ll.has_value()){
                        auto lltok = new Token{TK_LIT_INT, lctx.line}; lltok->pl.t_lng = val_ll.value();
                        lctx.emit(lltok);
                        continue;
                    } lctx.throw_err("Could not cast to int.");
                }
                case 2:{
                    auto val_f = interp_flt(lexm);
                    if (val_f.has_value()){
                        auto ftok = new Token{TK_LIT_FLOAT, lctx.line}; ftok->pl.t_flt = val_f.value();
                        lctx.emit(ftok);
                        continue;
                    } lctx.throw_err("Could not cast to float.");
                }
                case 3:{
                    auto val_d = interp_dub(lexm);
                    if(val_d.has_value()){
                        auto dtok = new Token{TK_LIT_DOUBLE, lctx.line}; dtok->pl.t_dub = val_d.value();
                        lctx.emit(dtok);
                        continue;
                    } lctx.throw_err("Could not cast to double.");
                }
            }


        }

        // Punctuation handling
        TokCode punctcode = match_punct(_c, lctx.cpeek(1), lctx);
        if (punctcode != TK_EMPTY){
            if (dmsg){cout << "\nMatched Punctuation!";}
            lctx.cnext();
            lctx.emit(new Token{punctcode, lctx.line});
            continue;
        }

        // String Literals
        if (_c == '"' || _c == '\''){
            char term = _c;

            next;
            string lexm;
            while (_c != '\0' && _c != term){lexm.push_back(_c); next;}
            if (_c == '\0'){throw runtime_error("Unterminated string literal at line " + to_string(lctx.line));}

            lctx.cnext(); // consumes final quote
            auto strtok = new Token{TK_LIT_STR, lctx.line};
            strtok->pl.t_str = new string(lexm);
            lctx.emit(strtok);
            continue;
        }

        // Identifiers
        if (isalpha(_c) || _c == '_'){
            string lexm;

            while (isalnum(_c) || _c == '_'){
                lexm.push_back(_c);
                _c = lctx.cnext();
            };

            if (lexm == "false" || lexm == "true"){
                auto booltok = new Token{TK_LIT_BOOL, lctx.line};
                booltok->pl.t_bul = lexm == "true";
                lctx.emit(booltok); continue;
            };
            if(lexm == "if"){lctx.emit(new Token{TK_IF, lctx.line}); continue;}
            if(lexm == "else"){lctx.emit(new Token{TK_ELSE, lctx.line}); continue;}
            if(lexm == "func"){lctx.emit(new Token{TK_FN, lctx.line}); continue;}
            if(lexm == "get"){lctx.emit(new Token{TK_GET, lctx.line}); continue;}
            if(lexm == "return"){lctx.emit(new Token{TK_RET, lctx.line}); continue;}

            auto idtok = new Token{TK_IDENT, lctx.line};
            idtok->pl.t_str = new string(lexm);
            lctx.emit(idtok); continue;
        }
        lctx.throw_err("Unmatched character " + vischar(_c) + " at " + to_string(lctx.src_index));  
    };
}