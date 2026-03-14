#include "../include/dlex.hpp"
#include <string>


dTCode punc_match( char C0, char C1, const char*& cur ){
    using enum dTCode;

    #define monocase(e, t) case e: { ++cur; return t; }
    // feel free to quickly do the other ops, im tryna figure out why strtof is being goth
    switch (C0) {
        monocase('+', plusT);
        monocase('-', minusT);
        monocase('/', slashT);
        monocase('*', starT);

        default: return invT;
    }
}

LexOutput tokenize( std::string src ) {
    char* cur = src.data(); // .data() gives us a non-const ptr
    const char* end = cur + src.size();
    std::vector<Token> stream; // vector for now for simplicity

    #define next { c = * (cur++); goto start; }
    #define step { cur++; c = c1; }
    #define update { c = * cur; goto start; }

    char c = *cur;

    start:
        // Whitespace Skip
        if (isspace(c)) next;
        
        char c1 = *(cur + 1);
        if (c == '/') {
            // Comment until next line break
            if (c1 == '/') { 
                while (c != '\n' && c != '\0') { c = * (cur++); }
                goto start;
            }    
            // Comment until next */ 
            if (c1 == '*') {
                while (c != '\0') { char cn = * (cur++); if (c == '*' && cn == '/') { cur++; break; }}
                update
            }
        }

        if (isdigit(c) || (isdigit(c1) && c == '.')) {
            float val = strtof(cur, &cur);
            stream.emplace_back(Token{dTCode::numT, val});
            update
        }
        if (cur < end) { c = * (cur++); goto start; }
    
    
    LexOutput output = LexOutput(stream);
    return output;
} 

bool ::Token::valid() const {
    return  this->type != dTCode::unkT
        &&  this->type != dTCode::invT
        &&  this->type != dTCode::eofT;
}

LexOutput::LexOutput(std::vector<Token> stream)
    :   literals(Arena::create(1024)),
        stream(stream),
        cursor(0)
{}

/*
void token_lctx(LessxContext& lctx, int debug_flags){
    // debug flags: 0b[][][][show_msg]

    size_t char_maxext = 0;
    lctx.src_index = 0;

    bool dmsg = (debug_flags & 0b0001) != 0;

    #define next c0 = lctx.cnext(); if(dmsg) {cout<<"\nOn Character " + vischar(c0);}

    unordered_map<string_view, string*> lexeme_map;
    lexeme_map.reserve(512);

    while (1){
        auto [c0, c1] = lctx.cpeek2();

        if (dmsg){cerr << "\nConstructing from " << vischar(c0) << " at " << to_string(lctx.src_index);}
        if (c0 == '\0'){lctx.emitfc(TK_EOF); break;};

        if (lctx.src_index + 1 <= char_maxext){
            // Tokenizer not designed to roll back on characters.
            lctx.throw_err("Reached source index " + to_string(lctx.src_index) + " multiple times!");
        }

        char_maxext = lctx.src_index + 1;
        
        if (isspace(c0)){
            int skiplen = 0;
            while (isspace(c0)){skiplen++; c0 = lctx.cnext();}
            if (dmsg){cout << "\nSkipped " + to_string(skiplen) + " whitespaces.";}
            continue;
        }


        // Comment handling // or //()
        if (c0 == '/' && c1 == '/'){
            char term = lctx.cpeek(2) == '(' ? ')' : '\n';
            lctx.src_index+=2; // consumes / and /

            while (c0 != term && c0 != '\0'){c0 = lctx.cnext();}
            if (c0 == term){next;}
            continue;
        }

        // Number Literals
        if (isdigit(c0) || (c0 == '.' && isdigit(c1))){
            string lexm;
            char prev = '\0';
            while (isalnum(c0) || c0 == '.' || c0 == '_' || (c0 == '-' && (prev == 'e' || prev == 'E'))){
                if (c0 != '_'){lexm.push_back(c0); prev = c0;}
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
                        auto lltok = Token{TK_LIT_INT, lctx.line}; lltok.pl.t_lng = val_ll.value();
                        lctx.emit(lltok);
                        continue;
                    }
                    auto val_f = interp_flt(lexm);
                    if (val_f.has_value()){
                        auto ftok = Token{TK_LIT_FLOAT, lctx.line}; ftok.pl.t_flt = val_f.value();
                        lctx.emit(ftok);
                        continue;
                    } lctx.throw_err("Malformed number.");
                }
                case 1:{
                    auto val_ll = interp_llong(lexm);
                    if (val_ll.has_value()){
                        auto lltok = Token{TK_LIT_INT, lctx.line}; lltok.pl.t_lng = val_ll.value();
                        lctx.emit(lltok);
                        continue;
                    } lctx.throw_err("Malformed int.");
                }
                case 2:{
                    auto val_f = interp_flt(lexm);
                    if (val_f.has_value()){
                        auto ftok = Token{TK_LIT_FLOAT, lctx.line}; ftok.pl.t_flt = val_f.value();
                        lctx.emit(ftok);
                        continue;
                    } lctx.throw_err("Malformed float.");
                }
                case 3:{
                    auto val_d = interp_dub(lexm);
                    if(val_d.has_value()){
                        auto dtok = Token{TK_LIT_DOUBLE, lctx.line}; dtok.pl.t_dub = val_d.value();
                        lctx.emit(dtok);
                        continue;
                    } lctx.throw_err("Malformed Double.");
                }
            }


        }

        // Punctuation handling
        TokCode punctcode = match_punct(c0, c1, lctx);
        if (punctcode != TK_EMPTY){
            if (dmsg){cout << "\nMatched Punctuation!";}
            lctx.cnext();
            lctx.emitfc(punctcode);
            continue;
        }

        // String Literals
        if (c0 == '"' || c0 == '\''){
            char term = c0;

            next;
            string lexm;
            while (c0 != '\0' && c0 != term){lexm.push_back(c0); next;}
            if (c0 == '\0'){throw runtime_error("Unterminated string literal at line " + to_string(lctx.line));}

            lctx.cnext(); // consumes final quote
            auto strtok = Token{TK_LIT_STR, lctx.line};
            
            string* cached = lexeme_map[lexm];
            if (cached){strtok.pl.t_str = cached;}
            else {auto ptr = lctx.alloc_string(lexm); lexeme_map[lexm] = ptr, strtok.pl.t_str = ptr;};

            lctx.emit(strtok);
            continue;
        }

        // Identifiers
        if (isalpha(c0) || c0 == '_'){
            string lexm;

            while (isalnum(c0) || c0 == '_'){
                lexm.push_back(c0);
                c0 = lctx.cnext();
            };

            if (lexm == "false" || lexm == "true"){
                auto booltok = Token{TK_LIT_BOOL, lctx.line};
                booltok.pl.t_bul = lexm == "true";
                lctx.emit(booltok); continue;
            };
            if(lexm == "if"){lctx.emitfc(TK_IF); continue;}
            if(lexm == "else"){lctx.emitfc(TK_ELSE); continue;}
            if(lexm == "func"){lctx.emitfc(TK_FN); continue;}
            if(lexm == "get"){lctx.emitfc(TK_GET); continue;}
            if(lexm == "return"){lctx.emitfc(TK_RET); continue;}

            auto idtok = Token{TK_IDENT, lctx.line};

            string* cached = lexeme_map[lexm];
            if (cached){idtok.pl.t_str = cached;}
            else {auto ptr = lctx.alloc_string(lexm); lexeme_map[lexm] = ptr, idtok.pl.t_str = ptr;};

            lctx.emit(idtok); continue;
        }

        lctx.throw_err("Unmatched character " + vischar(c0) + " at " + to_string(lctx.src_index));  
    };
}

*/