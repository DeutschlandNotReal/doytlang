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

enum TokenGroup {
    _null,
    _strings,
    _numbers,     // floats, doubles, ints
    _identifiers, // Includes keywords
    _operations,   
    _punctuation
};

enum TokenCode {
    _eof,
    _none,
    _err,
    _ident,

    // Keywords
    _kw_exit,
    _kw_func,
    _kw_let,

    // Literals
    _litstr,
    _litfloat,
    _litdouble,
    _litint,
    _litbool,

    // Punctuation
    _brace1,   // {
    _brace2,   // }
    _bracket1, // [
    _bracket2, // ]
    _paran1,   // (
    _paran2,   // )

    _concat,   // {} or ..?
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
    _tilde,    // ~
    _hash,     // #
};


inline string tokencode_to_string(TokenCode code){
    switch(code){
        case TokenCode::_eof: return "<eof>";
        case TokenCode::_none: return "<none>";
        case TokenCode::_ident: return "<ident>";
        case TokenCode::_kw_exit: return "<exit>";
        case TokenCode::_kw_func: return "<func>";
        case TokenCode::_kw_let: return "<let>";
        case TokenCode::_litstr: return "<str>";
        case TokenCode::_litfloat: return "<float>";
        case TokenCode::_litdouble: return "<double>";
        case TokenCode::_litint: return "<int>";
        case TokenCode::_litbool: return "<bool>";
        case TokenCode::_brace1: return "<brace1>";
        case TokenCode::_brace2: return "<brace2>";
        case TokenCode::_bracket1: return "<bracket1>";
        case TokenCode::_bracket2: return "<bracket2>";
        case TokenCode::_paran1: return "<paran1>";
        case TokenCode::_paran2: return "<paran2>";
        case TokenCode::_concat: return "<concat>";
        case TokenCode::_plus: return "<plus>";
        case TokenCode::_minus: return "<minus>";
        case TokenCode::_fslash: return "<fslash>";
        case TokenCode::_perc: return "<mod>";
        case TokenCode::_star: return "<star>";
        case TokenCode::_dstar: return "<dstar>";
        case TokenCode::_and: return "<and>";
        case TokenCode::_hat: return "<hat>";
        case TokenCode::_pipe: return "<pipe>";
        case TokenCode::_exc: return "<esc>";
        case TokenCode::_qwe: return "<qwe>";
        case TokenCode::_dand: return "<dand>";
        case TokenCode::_dhat: return "<dhat>";
        case TokenCode::_dpipe: return "<dpipe>";
        case TokenCode::_desc: return "<desc>";
        case TokenCode::_dqwe: return "<dqwe>";
        case TokenCode::_eq: return "<eq>";
        case TokenCode::_gt: return "<gt>";
        case TokenCode::_lt: return "<lt>";
        case TokenCode::_deq: return "<deq>";
        case TokenCode::_dgt: return "<dgt>";
        case TokenCode::_dlt: return "<dlt>";
        case TokenCode::_gteq: return "<gteq>";
        case TokenCode::_lteq: return "<lteq>";
        case TokenCode::_neq: return "<neq>";
        case TokenCode::_comma: return "<comma>";
        case TokenCode::_dot: return "<dot>";
        case TokenCode::_colon: return "<colon>";
        case TokenCode::_semi: return "<semi>";
        case TokenCode::_tilde: return "<tilde>";
        case TokenCode::_hash: return "<hash>";
        default: return "<unknown>";
    };
};

typedef variant<string, float, double, int> PayloadVariant;

struct Token {
    TokenCode code;
    TokenGroup group;
    string lexeme;

    PayloadVariant payload;

    int line;
    int column;
    int index;

    inline void insert(char c){lexeme.push_back(c);};
    Token(TokenCode code) : code(code), lexeme(""), payload(0), line(0), column(0), index(-1) {};
};

struct SourceContext {
    int curline;
    int curcolumn;
    string filename;
};

struct Lexer {
    vector<Token> stream;
    int index;
    int tokencount;
    [[nodiscard]] inline optional<Token> peek(int ahead = 0) const {
        return (index + ahead >= tokencount) ? nullopt : make_optional(stream[index + ahead]);
    }

    inline Token consume() { return stream[index++]; }

    inline void emit(Token tok) { tok.index = tokencount++; tok.group = match_group(tok.code); stream.push_back(tok); }

    // Gets the token at a relative position from the given token.
    inline optional<Token> get_relative(Token from, int disp) const {
        int target_index = from.index + disp;
        return (target_index < 0 || target_index >= tokencount) ? nullopt : make_optional(stream[target_index]);
    };

    inline bool neof() const { return index < tokencount; }

    inline bool isahead(TokenCode code, int ahead = 0) const {
        return neof() ? peek(ahead).value().code == code : false;
    }
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
        if(c == '\n'){context.curcolumn=1; context.curline++;}else{context.curcolumn++;};
        return c;
    };

    char advancenext(){
        if(src[pointer] == '\n'){context.curcolumn=1; context.curline++;}else{context.curcolumn++;};
        pointer++;
        return src[pointer];
    };

    static optional<Source*> construct(const string &filepath){
        ifstream file(filepath);
        if (!file){return nullopt;};
        stringstream buf;
        buf << file.rdbuf();

        return new Source{buf.str(), 0, {0, 0, filepath}};
    };

    inline Token newtoken(TokenCode code, optional<string> lexeme){
        auto tok = new Token{code};
        if(lexeme.has_value()){tok->lexeme=lexeme.value();}
        tok->line = context.curline;
        tok->column = context.curcolumn;
        return *tok;
    };
};

inline TokenGroup match_group(TokenCode code){
    switch(code){
        case TokenCode::_eof:
        case TokenCode::_none:      return TokenGroup::_null;

        case TokenCode::_ident:
        case TokenCode::_kw_exit:
        case TokenCode::_kw_func:
        case TokenCode::_kw_let:    return TokenGroup::_identifiers;

        case TokenCode::_litstr:    return TokenGroup::_strings;
        case TokenCode::_litfloat:
        case TokenCode::_litdouble:
        case TokenCode::_litint:
        case TokenCode::_litbool:   return TokenGroup::_numbers;

        case TokenCode::_plus:
        case TokenCode::_minus:
        case TokenCode::_fslash:
        case TokenCode::_perc:
        case TokenCode::_star:
        case TokenCode::_dstar:

        case TokenCode::_and:
        case TokenCode::_hat:
        case TokenCode::_pipe:
        case TokenCode::_exc:
        case TokenCode::_qwe:

        case TokenCode::_dand:
        case TokenCode::_dhat:
        case TokenCode::_dpipe:
        case TokenCode::_desc:
        case TokenCode::_dqwe:

        case TokenCode::_eq:
        case TokenCode::_gt:
        case TokenCode::_lt:
        case TokenCode::_deq:
        case TokenCode::_dgt:
        case TokenCode::_dlt:
        case TokenCode::_gteq:
        case TokenCode::_lteq:
        case TokenCode::_neq:
        case TokenCode::_concat:

            return TokenGroup::_operations;

        case TokenCode::_brace1:
        case TokenCode::_brace2:
        case TokenCode::_bracket1:
        case TokenCode::_bracket2:
        case TokenCode::_paran1:
        case TokenCode::_paran2:
        case TokenCode::_comma:
        case TokenCode::_dot:
        case TokenCode::_colon:
        case TokenCode::_semi:
        case TokenCode::_tilde:
        case TokenCode::_hash:
            return TokenGroup::_punctuation;

        default: return TokenGroup::_null;
    };
};

inline optional<Token> match_punctuation(Source &src){
    char a = src.peek(0);
    char b = src.peek(1);

    #define monotok(code){auto tok = src.newtoken(code, string(1, a)); src.advance(); return tok;};
    #define duotok(code){auto tok = src.newtoken(code, string({a, b})); src.advance(); src.advance(); return tok;};

    switch(a){
        case '=':switch(b){
            case '=': duotok(TokenCode::_deq);
            default: monotok(TokenCode::_eq);
        };
        case '>':switch(b){
            case '=': duotok(TokenCode::_gteq);
            case '>': duotok(TokenCode::_dgt);
            default: monotok(TokenCode::_gt);
        };
        case '<':switch(b){
            case '=': duotok(TokenCode::_lteq);
            case '<': duotok(TokenCode::_dlt);
            default: monotok(TokenCode::_lt);
        };
        case '!':switch(b){
            case '=': duotok(TokenCode::_neq);
            case '!': duotok(TokenCode::_desc);
            default: monotok(TokenCode::_exc);
        };
        case '?':switch(b){
            case '?': duotok(TokenCode::_dqwe);
            default: monotok(TokenCode::_qwe);
        };
        case '&':switch(b){
            case '&': duotok(TokenCode::_dand);
            default: monotok(TokenCode::_and);
        };
        case '^':switch(b){
            case '^': duotok(TokenCode::_dhat);
            default: monotok(TokenCode::_hat);
        };
        case '|':switch(b){
            case '|': duotok(TokenCode::_dpipe);
            default: monotok(TokenCode::_pipe);
        };
        case '*':switch(b){
            case '*': duotok(TokenCode::_dstar);
            default: monotok(TokenCode::_star);
        };
        case '+': monotok(TokenCode::_plus);
        case '-': monotok(TokenCode::_minus);
        case '/': monotok(TokenCode::_fslash);
        case '%': monotok(TokenCode::_perc);
        case '#': monotok(TokenCode::_hash);
        case '~': monotok(TokenCode::_tilde);

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
        default: return nullopt;
    };
};

inline char match_escchar(char a){
    switch(a){
        case '0':  return '\0';
        case 'n':  return '\n';
        case 't':  return '\t';
        case '\'': return '\'';
        case '\\': return '\\';
        case '\"': return '\"';
        case 'r':  return '\r';
        case '{': return '{';
        default:   return 'n';
    };
};

unordered_map<string, TokenCode> keywordmap {
    {"exit", TokenCode::_kw_exit},
    {"func", TokenCode::_kw_func},
    {"let",  TokenCode::_kw_let},
};

inline string vischar(char c){
    switch(c){
        case '\0': return "<null>";
        case '\n': return "<newline>";
        case '\t': return "<tab>";
        case '\r': return "<return>";
        case ' ':  return "<space>";
        default: return string(1, c);
    };
};

inline Lexer* tokenize(Source &src, bool debug_msg){
    Lexer* lex = new Lexer{};
    char c = src.peek();

    // could you update parser.hpp w/ your version
    // or a terminal command I forgot which one

    #define adv      c = src.advance()
    #define advnext  c = src.advancenext()
    #define emit(v) lex->emit(v);if(debug_msg){cout<<"\nEmitted token w/ lexeme: "<<v.lexeme;}

    bool awaitconcat = 0;
    bool awaitstr    = '\'';

    int lastindex = -1;
    do{
        c=src.peek();
        if(debug_msg){cout<<"\nProccessing: "<<vischar(c);};
        if (isspace(c)){advnext; continue;};

        // Comment match /* until \n or eof, or /*()
        if (c == '/' && src.peek(1) == '*'){
            if(debug_msg){cout<<"\nDetected comment start.";}
            src.advance(); // consumes /
            src.advance(); // consumes *

            char breakat = '\n';
            if(src.peek() == '('){breakat = ')'; src.advance();};

            while (1){
                advnext;
                if(c==breakat || c=='\0'){break;};
            };
            if(c==')'){src.advance();}; // consumes final ) if needed
            if(debug_msg){cout<<"\nDetected comment end.";}
            continue;
        };

        // Punctuation match (all 1-2 character symbols)
        auto ptok = match_punctuation(src);
        if(ptok.has_value() && !(c == '}' && awaitconcat)){
            if(debug_msg){cout<<"\nMatched punctuation: "<< ptok.value().lexeme;};
            emit(ptok.value()); continue;
        } else if (debug_msg){cout<<"\nFailed to match to punc";};

        // String match
        if(c=='\'' || c == '\"' || (c == '}' && awaitconcat)){
            char terminator = awaitconcat ? awaitstr : c;
            if(debug_msg){{cout<<"\nDetected string literal start with "<<terminator;}};
            if (c == '}' && awaitconcat) {
                awaitconcat = 0; 
                emit(src.newtoken(TokenCode::_concat, ""));
                src.advance(); // just consume the '}'
                if (src.peek() == terminator) {
                    src.advance(); // consume the closing quote
                    
                    continue;       // resume lexing after string
                }
            } else {
                advnext; // consumes initial ' or "
            };

            auto strtok = src.newtoken(TokenCode::_litstr, nullopt);
            while (1){
                if(c=='\0'){
                    // reached eof w/o string termination
                    throw runtime_error("\nUnterminated string!");
                } else if (c == terminator){src.advance(); break;};

                if (c == '\\'){
                    char cnext = src.peek(1);
                    char match = match_escchar(cnext); // n if no match
                    
                    if (match != 'n'){
                        if(debug_msg){cout << "\nEscape char match " << match;};

                        strtok.lexeme.push_back(match);
                        advnext; // consumes escape character
                    };
                } else if (c == '{') {
                    // Begins concatination chain ({}) (breaks at \{, handled by escape char handler below)
                    if(debug_msg){cout << "\nConcat check!";}
                    
                    emit(strtok);
                    emit(src.newtoken(TokenCode::_concat, ""));
                    awaitstr = terminator;
                    src.advance(); // consumes {
                    awaitconcat = 1;
                    break;
                } else { strtok.lexeme.push_back(c);};
                advnext;
            };
            if(awaitconcat){continue;};
            
            emit(strtok); continue;
            awaitconcat = 0;
        };

        // Number Match
        if (isdigit(c)){
            if(debug_msg){cout << "\nDetected number literal start.";};
            auto numtok = src.newtoken(TokenCode::_litint, nullopt);
            while (1){
                if(c=='\0'){break;};
                if(!isalnum(c) && c != '.'){break;};

                numtok.lexeme.push_back(c);
                advnext;
            };

            char final = numtok.lexeme.back();

            if (final == 'f' || final == 'F'){
                numtok.lexeme.pop_back(); // remove f
                numtok.code = TokenCode::_litfloat;
                try{numtok.payload = stof(numtok.lexeme);}
                catch(const invalid_argument &e){throw runtime_error("\nMalformed float!");}
                catch(const out_of_range &e){throw runtime_error("\nFloat out of range!");};
                emit(numtok); continue;
            } else if (final == 'd' || final == 'D'){
                numtok.lexeme.pop_back(); // remove d
                numtok.code = TokenCode::_litdouble;
                try{numtok.payload = stod(numtok.lexeme);}
                catch(const invalid_argument &e){throw runtime_error("\nMalformed double!");}
                catch(const out_of_range &e){throw runtime_error("\nDouble out of range!");};
                emit(numtok); continue;
            };
            try{numtok.payload = stoi(numtok.lexeme);}
            catch(...){ // Is an invalid int
                try{numtok.code = TokenCode::_litfloat; numtok.payload = stof(numtok.lexeme);}
                catch(const invalid_argument &e){throw runtime_error("\nMalformed number!");}
                catch(const out_of_range &e){throw runtime_error("\nNumber out of range!");};
            };
            emit(numtok); continue;
        };
        
        // Identifier Match
        if (isalpha(c) || c == '_'){
            if(debug_msg){cout << "\nDetected identifier start.";};
            auto identok = src.newtoken(TokenCode::_ident, nullopt);
            while(1){
                if(c == '\0' || (!isalnum(c) && c != '_')){break;};
                identok.insert(c);
                advnext;
            };
            identok.payload = identok.lexeme;

            if (keywordmap.count(identok.lexeme)>0){
                identok.code = keywordmap[identok.lexeme];
            };

            emit(identok); continue;
        };
        
        if (c == '\0'){break;};
        if(debug_msg){cout << "\nUncaught symbol '" << c << "'";};

        // to stop forever-loops
        if(lastindex==src.pointer){throw runtime_error("\nExited hazardous loop...");}
        lastindex=src.pointer;
        advnext;
    } while (c != '\0');

    return lex;
};