#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream> 
#include <sstream>

using namespace std;

enum TokCode {
    TK_INVALID, // Tokens past the _EOF token
    TK_EMPTY,   // Unproccessed Tokens
    TK_EOF,     // End of File Token
    
    TK_LIT_BOOL,
    TK_LIT_INT,
    TK_LIT_FLOAT,
    TK_LIT_DOUBLE,
    TK_LIT_STR,

    TK_IDENT,

    TK_IF, TK_ELSE, TK_RET, TK_FN, TK_GET,
    
    TK_LPAREN, TK_RPAREN, // ()
    TK_LBRACK, TK_RBRACK, // []
    TK_LBRACE, TK_RBRACE, // {}

    TK_DOT, TK_COMMA, TK_SEMI, TK_COLON, // . , ; :
    
    TK_PLUS, TK_MINUS, TK_STAR, TK_SLASH, TK_PERC, // + - * / %
    TK_HAT, TK_AND, TK_PIPE, TK_EXC, TK_QWE, // ^ & | ! ?

    TK_AND2, TK_STAR2, TK_PIPE2, // && ** ||
    TK_HASH, // #
    TK_GT, TK_LT, TK_EQ, // > < =
    TK_GTEQ, TK_LTEQ, TK_EQ2, TK_NEQ, TK_GT2, TK_LT2 // >= <= == != >> <<
};

union TokPl {
    long long t_lng;
    float     t_flt;
    int       t_int;
    double    t_dub;
    bool      t_bul;
    string*   t_str;
};

struct Token {
    TokCode type;
    int line;
    TokPl pl;

    ~Token(){
        if (type == TK_IDENT || type == TK_LIT_STR){
            delete pl.t_str;
        }
    };
};

inline string t_str(Token &t){
    switch(t.type){
        case TK_LPAREN: return"("; case TK_RPAREN: return")";
        case TK_LBRACK: return"["; case TK_RBRACK: return"]";
        case TK_LBRACE: return"{"; case TK_RBRACE: return"}";
        case TK_PLUS: return"+"; case TK_MINUS: return"-";
        case TK_STAR: return"*"; case TK_STAR2: return"**";
        case TK_SLASH: return"/";case TK_HAT: return"^";
        case TK_AND: return"&";  case TK_PIPE: return"|";
        case TK_EXC: return"!";  case TK_AND2: return"&&";
        case TK_HASH: return"#"; case TK_QWE: return"?";
        case TK_PIPE2: return"||";

        case TK_EQ: return"="; case TK_EQ2: return"=="; case TK_GT: return">"; case TK_LT: return"<";
        case TK_GT2: return">>"; case TK_LT2: return"<<"; case TK_GTEQ: return">=";case TK_LTEQ: return"<=";
        case TK_DOT: return"."; case TK_COMMA: return","; case TK_SEMI: return";"; case TK_COLON: return":";

        case TK_FN: return"FUNC"; case TK_RET: return"RETURN"; case TK_GET: return"GET"; case TK_IF: return"IF";

        case TK_EOF: return"EOF"; case TK_INVALID: return"INVALID";
        
        case TK_LIT_BOOL: return "BOOL:" + t.pl.t_bul ? "true" : "false"; 
        case TK_LIT_INT: return "INT:" + to_string(t.pl.t_lng);
        case TK_LIT_DOUBLE: return "DOUBLE:" + to_string(t.pl.t_dub); 
        case TK_LIT_FLOAT: return "FLOAT:" + to_string(t.pl.t_flt);
        case TK_LIT_STR: return "STR\"" + *(t.pl.t_str) + "\"";
        case TK_IDENT: return "IDENT\"" + *(t.pl.t_str) + "\"";
        default: return "TCODE " + to_string(t.type);
    };
};

struct LexContext{
    string src;
    size_t src_index;
    size_t src_size;
    int line;

    vector<Token*> tokstream;
    size_t tok_index;
    size_t tok_size;

    ~LexContext(){
        for (auto &t : tokstream){
            delete t;
        }
    }
    
    static LexContext from_source(string source){
        return {source, 0, source.size(), 1, {}, 0, 0};
    }

    static LexContext from_filepath(string filepath){
        ifstream file(filepath);
        if (!file.is_open()) {
            throw runtime_error("Could not open file " + filepath);
        }

        stringstream buf;
        buf << file.rdbuf();
        file.close();

        string bufstr = buf.str();
        return {bufstr, 0, bufstr.size(), 1, {}, 0, 0};
    }

    [[nodiscard]] inline Token& tpeek(int delta = 0){
        return *(tokstream[tok_index+delta]);
    }
    inline Token& tconsume(){return *(tokstream[tok_index++]);}
    inline Token& tnext(){tok_index++; return *(tokstream[tok_index]);}
    inline Token& tmove(int delta){tok_index+=delta; return *(tokstream[tok_index]);}

    [[nodiscard]] inline char cpeek(int delta = 0){
        int tpoint = src_index + delta;
        return (tpoint < src_size && (tpoint >= 0)) ? src[src_index+delta] : '\0';
    }

    inline char cconsume(){if(src[src_index] =='\n'){line++;} if(src_index>=src_size){return '\0';}; return src[src_index++];}
    inline char cnext()   {if(src[src_index] =='\n'){line++;} if(src_index>=src_size){return '\0';}; src_index++; return src[src_index];}
    
    inline void emit(Token* tok){
        tokstream.push_back(tok); tok_size++;
    };
    
    void throw_err(const string& message){
        throw runtime_error(
            "\n\033[1;38;2;255;0;0mFatal Error at line " + to_string(line) + ": " + message + "\033[0m"
        );
    }

    void throw_prob(const string& message){
        cerr << "\n\033[1;38;2;255;165;0mProblem at line " << line << ": " << message << "\033[0m";
    }
};


