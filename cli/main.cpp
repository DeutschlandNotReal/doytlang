// g++ cli\main.cpp -o main
// ./main.exe

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <optional>
#include "../include/lexer.hpp"

const char *authors[] = {
    "DeutschlandNotReal",
    "sk337",
    nullptr,
};

using namespace std;

void display_tokens(std::vector<Token *> toks)
{
    std::cout << '\n';
    for (int i = 0; i < toks.size(); i++)
    {
        Token *tok = toks[i];
        auto lexeme = tok->lexeme;

        if (i % 2 == 0)
        {
            std::cout << "\033[41m" << lexeme << "\033[0m ";
        }
        else
        {
            std::cout << "\033[44m" << lexeme << "\033[0m ";
        }
    };
};
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "\nMust input a filepath!\n";
        return 1;
    };

    bool debug_msg = false;
    bool print_tok = false;
    if (argc > 2){
        for (int argi = 2; argi<argc; argi++){
            string sarg = string(argv[argi]);
            if(sarg=="disp"){print_tok = true; continue;};
            if(sarg=="step"){debug_msg = true; continue;};
            cerr << "\n invalid arguement " << argi << " (" << sarg << ")";
        };
    };

    const string filepath = argv[1];
    auto rawsrc = Source::construct(filepath);

    if (!rawsrc.has_value())
    {
        cout << "Cannot read file '" << filepath << "'!";
        return 1;
    };
    Source *src = rawsrc.value();

    std::cout << "Tokenizing " << src->src.size() << " characters from " << filepath << "\n";
    Lexer *lex;

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