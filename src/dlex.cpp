#include <cctype>
#include <cmath>
#include <lang/dlex.hpp>
#include <stdexcept>

int int_parse(const char*& ptr) {
    int value = 0, mul = 1;
    char c = *ptr;
    if (c == '-') { c = *(++ptr); mul = -1; }
    while (isdigit(c)) {
        value = value*10 + c - '0';
        c = *(++ptr);
    }
    return value * mul;
}

int uint_parse(const char*& ptr) {
    int value = 0;
    char c = *ptr;
    while (isdigit(c)) {
        value = value*10 + c - '0';
        c = *(++ptr);
    }
    return value;
}

float float_parse(const char*& ptr) {
    int i_val = 0;
    i_val = int_parse(ptr);

    if (*ptr != '.') return (float)i_val; 
    
    const char *d_ptr = ++ptr;
    int d_val = uint_parse(ptr);
    float val = (float)i_val + (float)d_val * std::pow(10, d_ptr - ptr);
    if ((*ptr | 0x20) != 'e') return val;
    
    if (*(++ptr) == '+') ++ptr;
    int e_val = int_parse(ptr);
    return val * std::pow(10, e_val);
}

TokenCode match_punctuation(char a, char& b, const char*& src) {
    using enum TokenCode;
    switch (a) {
        case '+': return PLUS;
        case '-': return MINUS;
        case '*': return STAR;
        case '/': return SLASH;
        case '.': return DOT;
        case ',': return COMMA;
        case ';': return SEMI;
        case '>': switch (b) {
            case '=': b = *(++src); return GTEQ;
            case '>': b = *(++src); return GT2;
            default: return GT;
        }
        case '<': switch (b) {
            case '=': b = *(++src); return LTEQ;
            case '<': b = *(++src); return LT2;
            default: return LT;
        }
        case '!': switch (b) {
            case '=': b = *(++src); return NEQ;
            default: return EXC;
        }
        case '=': switch(b) {
            case '=': b = *(++src); return EQ2;
            default: return EQ;
        }
        default: return _EOF;
    }
};

LexOutput tokenize(const char** src_set, int src_count, int flags) {
    LexOutput lexout = LexOutput();
    
    while (src_count--) {
        const char* src = *(src_set++);
        char b = *(src);
        while (b) {
            char a = b;
            b = *(++src);

            if (isspace(a)) continue;

            if (a == '/' && b == '/') {
                while (b && b != '\n') b = *(++src);
                continue;
            }

            // allows numbers begining with .
            if ((a == '.' && isdigit(b)) || isdigit(a)) {
                // temporarily just single NUMBER token
                lexout.emit(TokenCode::NUMBER, float_parse(--src));
                b = *src;
                continue;
            }

            {
                TokenCode punct = match_punctuation(a, b, src);
                if (punct != TokenCode::_EOF) {
                    lexout.emit(punct);
                    b = *(src); continue;
                } 
            }

            if (a == '\'' || a == '"') {
                const char* start = src;
                // using "", always parses as string
                // using '', parses as string if over 1 character
                char term = a;
                while (b != term && b) b = *(++src);
                int len = src - start;
                if (term == '\'' && len == 1) {
                    lexout.emit(TokenCode::CHAR, *(src-1));
                } else {
                    lexout.emit(TokenCode::STRING, 
                        TextView(start + 1, src)
                    );
                }
                b = *(++src); // skips closing term
                continue;
            }

            if (isalpha(a) || a == '_') {
                const char* ident_start = src;
                while (isalnum(b) || b == '_') b = *(++src);
                TextView ident_view(ident_start, src-1);
                if (ident_view == "return") { lexout.emit(TokenCode::RET); continue; }
                if (ident_view == "func") { lexout.emit(TokenCode::FUNC); continue; }
                if (ident_view == "get") { lexout.emit(TokenCode::GET); continue; }
                if (ident_view == "if") { lexout.emit(TokenCode::IF); continue; }
                if (ident_view == "else") { lexout.emit(TokenCode::ELSE); continue; }
                if (ident_view == "while") { lexout.emit(TokenCode::WHILE); continue; }
                if (ident_view == "break") { lexout.emit(TokenCode::BREAK); continue; }
                if (ident_view == "continue") { lexout.emit(TokenCode::CONTINUE); continue; }

                // temporary, identities will have an ID
                lexout.emit(TokenCode::IDENTITY, ident_view);
            }
        }
    }

    lexout.emit(TokenCode::_EOF);

    return lexout;
}