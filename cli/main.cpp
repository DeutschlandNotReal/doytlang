// g++ cli\main.cpp -o main
// ./main.exe

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <optional>
#include <charconv>
#include "../include/lexer.hpp"

const char *authors[] = {
    "DeutschlandNotReal (doytdeuter)",
    "sk337 <me@pk3.zip>",
    "fhudufin",
    nullptr,
};

using namespace std;

void display_tokens(vector<Token> toks)
{
    cout << '\n';      
    for (int i = 0; i < toks.size(); i++)
    {
        Token tok = toks[i];
        auto lexeme = tok.lexeme;
        auto code = tok.code;
        auto lexcode = tokencode_to_string(code);
        if(code != TokenCode::_litstr && code != TokenCode::_ident && code != TokenCode::_litint && code != TokenCode::_litfloat && code != TokenCode::_litdouble && code != TokenCode::_litbool){
            lexeme = "";
        };
        #define colout(col, content) "\033[" << col << "m" << content << "\033[0m ";

        if (i % 2 == 0)
        {
            cout << colout(41, lexcode<<lexeme);
        }
        else
        {
            cout << colout(44, lexcode<<lexeme);
        }
    };
};

void printhelp()
{
    cerr << "DoytLang CLI\nAuthors: ";
    for (int i = 0; authors[i] != nullptr; i++)
    {
        cerr << authors[i];
        if (authors[i + 1] != nullptr)
        {
            cerr << ", ";
        };
    };
    cerr << "\nFlags:\n -d : debug messages\n -p : print tokens\n -h : help\n";
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "\nMust input a filepath!\n";
        printhelp();
        return 1;
    };

    bool debug_msg = false;
    bool print_tok = false;
    string filepath;
    if (argc > 1){
        for (int argi = 1; argi<argc; argi++){
            switch (argv[argi][0]){
                case '-':
                    switch (*(argv[argi]+1)){
                        case 'd': debug_msg = true; break;
                        case 'p': print_tok = true; break;
                        case 'h':
                            printhelp();
                            return 0;
                        default: cerr << "Unknown flag '" << argv[argi] << "'!\n"; return 1;
                    }; break;
                default: filepath = argv[argi]; break;
            };
        };
    };

    auto rawsrc = Source::construct(filepath);

    if (!rawsrc.has_value())
    {
        cout << "Cannot read file '" << filepath << "'!";
        return 1;
    };
    Source src = *rawsrc.value();

    std::cout << "Tokenizing " << src.src.size() << " characters from " << filepath << "\n";
    Lexer* lex;

    auto bench_begin = chrono::high_resolution_clock::now();
    try
    {
        lex = tokenize(src, debug_msg);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "tokenizer error: " << e.what() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cout << "error with tokenizer... " << e.what() << '\n';
    }
    catch (...)
    {
        std::cout << "Unknown lexer error\n";
    };
    auto bench_final = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = bench_final - bench_begin;

    std::cout << "\nProduced " << lex->stream.size() << " token(s)! (duration: " << elapsed.count() << "ms)";
    if (print_tok)
    {
        display_tokens(lex->stream);
    };

    return 0;
}