#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <variant>

using namespace std;

enum TokenCode {
    _eof,
    _none,
    _ident,

    // Keywords
    _kw_exit,

    // Literals
    _litstr,
    _litfloat,
    _litint,
    _litbool,

    // Punctuation
    _brace1,   // {
    _brace2,   // }
    _bracket1, // [
    _bracket2, // ]
    _paran1,   // (
    _paran2,   // )

    _plus,     // +
    _minus,    // -
    _fslash,   // /
    _perc,     // %
    _star,     // *
    _dstar,    // **

    _and,      // &
    _hat,      // ^
    _pipe,     // |
    _exc,      // !
    _qwe,      // ?

    _dand,     // &&
    _dhat,     // ^^
    _dpipe,    // ||
    _desc,     // !!
    _dqwe,     // ??

    _eq,       // =
    _gt,       // >
    _lt,       // <

    _deq,      // ==
    _dgt,      // >>
    _dlt,      // <<
    _gteq,     // >=
    _lteq,     // <=
    _neq,      // !=

    _comma,    // ,
    _dot,      // .
    _colon,    // :
    _semi,     // ;
    _hash,     // #
};

struct Token {
    TokenCode code;
    string lexeme;
    variant<string, float, int> payload;

    int line;
    int column;
    int index;
};

struct SourceContext {
    int curline;
    int curcolumn;
    string filename;
};

struct Lexer {
    vector<Token*> stream;
    int index;
    int tokencount;
    [[nodiscard]] inline optional<Token*> peek(int ahead = 0) const{
        return (index+ahead>tokencount) ? nullptr : stream[index+ahead];
    };

    inline Token* consume(){return stream[index++];};

    inline void emit(Token* tok){tok->index = tokencount++; stream.push_back(tok);};
};

struct Source {
    string src;
    int pointer;
    SourceContext context;

    [[nodiscard]] inline char peek(int ahead=0) const{
        return (pointer+ahead>=src.size()) ? '\0' : src[pointer+ahead];
    };

    inline char consume(){
        return src[pointer++];
    };

    // Serves the same purpose as consume, but also influences source context
    char advance(){
        char c = src[pointer++];
        if (c == '\n'){context.curcolumn=1; context.curline++;}else{context.curcolumn++;};
        return c;
    };

    static optional<Source*> construct(const string &filepath){
        ifstream file(filepath);
        if (!file){return nullopt;};
        stringstream buf;
        buf << file.rdbuf();

        return new Source{buf.str(), 0, {0, 0, filepath}};
    };

    inline Token* newtoken(TokenCode code, optional<string> lexeme){
        auto tok = new Token{code};
        if(lexeme.has_value()){tok->lexeme=lexeme.value();}
        tok->line = context.curline;
        tok->column = context.curcolumn;
        return tok;
    };
};

inline optional<Token*> match_punctuation(Source* src){
    char a = src->peek();
    char b = src->peek(1);
    
    #define monotok(code){auto tok = src->newtoken(code, string(1, a)); src->advance(); return tok; break;};
    #define duotok(code){auto tok = src->newtoken(code, string({a, b})); src->advance(); src->advance(); return tok; break;};

    switch(a){
        case '=':switch(b){
            case '=': duotok(TokenCode::_deq);
            default: monotok(TokenCode::_eq);
        } return nullopt;
        case '>':switch(b){
            case '=': duotok(TokenCode::_gteq);
            case '>': duotok(TokenCode::_dgt);
            default: monotok(TokenCode::_gt);
        } return nullopt;
        case '<':switch(b){
            case '=': duotok(TokenCode::_lteq);
            case '<': duotok(TokenCode::_dlt);
            default: monotok(TokenCode::_lt);
        } return nullopt;
        case '!':switch(b){
            case '=': duotok(TokenCode::_neq);
            case '!': duotok(TokenCode::_desc);
            default: monotok(TokenCode::_exc);
        } return nullopt;
        case '?':switch(b){
            case '?': duotok(TokenCode::_dqwe);
            default: monotok(TokenCode::_qwe);
        } return nullopt;
        case '&':switch(b){
            case '&': duotok(TokenCode::_dand);
            default: monotok(TokenCode::_and);
        } return nullopt;
        case '^':switch(b){
            case '^': duotok(TokenCode::_dhat);
            default: monotok(TokenCode::_hat);
        } return nullopt;
        case '|':switch(b){
            case '|': duotok(TokenCode::_dpipe);
            default: monotok(TokenCode::_pipe);
        } return nullopt;
        case '*':switch(b){
            case '*': duotok(TokenCode::_dstar);
            default: monotok(TokenCode::_star);
        } return nullopt;
        case '+': monotok(TokenCode::_plus);
        case '-': monotok(TokenCode::_minus);
        case '/': monotok(TokenCode::_fslash);
        case '%': monotok(TokenCode::_perc);
        case '#': monotok(TokenCode::_hash);

        case '.': monotok(TokenCode::_dot);
        case ',': monotok(TokenCode::_comma);
        case ':': monotok(TokenCode::_colon);
        case ';': monotok(TokenCode::_semi);
    
        case '{': monotok(TokenCode::_brace1);
        case '}': monotok(TokenCode::_brace2);
        case '(': monotok(TokenCode::_paran1);
        case ')': monotok(TokenCode::_paran2);
        case '[': monotok(TokenCode::_bracket1);
        case ']': monotok(TokenCode::_bracket2);
    };
    return nullopt;
};

unordered_map<string, TokenCode> keywordmap {
    {"exit", TokenCode::_kw_exit}
};


inline Lexer *tokenize(Source* src){
    Lexer *lex = new Lexer{};
    char c = src->peek();

    #define consume c=src->advance()

    do{
        if (isspace(c)){consume; continue;};
        // Punctuation match (all 1-2 character symbols)
        auto ptok = match_punctuation(src);
        if(ptok.has_value()){lex->emit(ptok.value()); continue;};

        // String literal match
        if(c=='\''){
            consume; // consumes initial '
            auto strtok = src->newtoken(TokenCode::_litstr, nullopt);
            while (1){
                if(c=='\0'){
                    // reached eof w/o string termination
                    throw runtime_error("Unterminated string!");
                } else if (c == '\''){consume; break;};

                strtok->lexeme.push_back(c);
                consume;
            };
            lex->emit(strtok); continue;
        };

        if (isdigit(c)){
            auto numtok = src->newtoken(TokenCode::_litint, nullopt);
            while (1){
                if(c=='\0'){break;};
                if(!isalnum(c) && c != '.'){break;};

                numtok->lexeme.push_back(c);
                consume;
            };
            try{numtok->payload = stoi(numtok->lexeme);}
            catch(...){ // Is an invalid int
                try{numtok->code = TokenCode::_litfloat; numtok->payload = stof(numtok->lexeme);}
                catch(const invalid_argument &e){throw runtime_error("Malformed number!");}
                catch(const out_of_range &e){throw runtime_error("Number out of range!");};
            };
            lex->emit(numtok); continue;
        };
        
        if (isalpha(c)){
            auto identok = src->newtoken(TokenCode::_ident, nullopt);
            while (1){
                if(c == '\0' || !isalnum(c)){break;};
                identok->lexeme.push_back(c);
                consume;
            };
            identok->payload = identok->lexeme;

            if (keywordmap.count(identok->lexeme)>0){
                identok->code = keywordmap[identok->lexeme];
            };

            lex->emit(identok); continue;
        };
        
        if (c == '\0'){break;};
        cout << "\nUncaught symbol (" << c << ")!";
        consume;
    } while (c != '\0');

    return lex;
};