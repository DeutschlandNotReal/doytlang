// g++ cli\main.cpp -o main;./main.exe test/main.doyt -p -d

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <optional>
#include <charconv>
#include <iomanip>
#include <fmt/core.h>
#include "../include/dlex.hpp"
#include "../src/dlex.cpp"

using namespace std;

inline void disp_help(){
    cerr << "\nFlags:\n -d : debug messages\n -p : print tokens\n -h : help\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "\nMust input a filepath!\n";
        disp_help();
        return 1;
    };

    int flags = 0b0000; // [show display messages][print tokens]
    string filepath;
    if (argc > 1){
        for (int argi = 1; argi<argc; argi++){
            switch (argv[argi][0]){
                case '-':
                    switch (*(argv[argi]+1)){
                        case 'd': flags |= 0b0001; break;
                        case 'p': flags |= 0b0010; break;
                        case 'h':
                            disp_help();
                            return 0;
                        default: cerr << "Unrecognised flag '" << argv[argi] << "'!\n"; return 1;
                    }; break;
                default: filepath = argv[argi]; break;
            };
        };
    };

    auto lexctx = LexContext::from_filepath(filepath);

    cout << "\nReading: " << filepath << ", Source Size: " << lexctx.src_size;

    bool success = true;
    auto bench_start = chrono::high_resolution_clock::now();
    try{
        token_lctx(lexctx, flags);
    }
    catch(runtime_error &e){
        success = false;
        cerr << e.what();
    }
    catch(exception &e){
        success = false;
        cerr << "\nTokenizer unexpected error: " << e.what();
    };
    auto bench_end = chrono::high_resolution_clock::now();

    double duration = chrono::duration_cast<chrono::microseconds>(bench_end - bench_start).count();

    if (!success){
        cout << "\n\033[1;38;2;255;165;0mFailed to fully tokenize!\033[0m";
    }
    cout << '\n' << lexctx.tok_size << " Tokens generated!";
    cout << fmt::format(
        " Took: {:.3f}ms ({:.3f}ns/token, {:.3f}ns/char) Characters / Token: {:.3f}", 
        duration / 1000.0, 
        duration / lexctx.tok_size * 1000, 
        duration / lexctx.src_size * 1000,
        static_cast<double>(lexctx.src_size) / lexctx.tok_size
    );


    if (flags & 0b0010){
        int i = 0;
        int lastline = 0;
        for (size_t t_id = 0; t_id < lexctx.tok_size; t_id++){
            i+=1;
            Token &tok = *lexctx.tokstream[t_id];
            if (tok.line != lastline){
                lastline = tok.line;
                cout << "\n\x1b[38;5;226m[" + to_string(lastline) + "]\033[0m ";
            }
            if (tok.type == TK_EOF){
                cout << "\x1b[38;5;203m" << "EOF" << "\033[0m ";
            } else if (tok.type == TK_EMPTY){
                cout << "\x1b[38;5;68m" << "EMPTY" << "\033[0m ";
            } else if (tok.type == TK_INVALID){
                cout << "\x1b[38;5;88m" << "INVALID" << "\033[0m ";
            } else if (i % 2 == 0){
                cout << "\x1b[38;5;8m" << t_str(tok) << "\033[0m ";
            } else {
                cout << "\x1b[38;5;7m" << t_str(tok) << "\033[0m ";
            }
        }
        cout << '\n';
    }


    return 0;
}