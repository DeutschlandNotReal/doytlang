// g++ cli\main.cpp -o main
// ./main.exe

#include "lexer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <optional>
#include "parser.h"

std::optional<std::string> ReadFile(const std::string &filepath)
{
    std::ifstream file(filepath);
    if (!file)
    {
        std::cerr << "The file " << filepath << " is NOT real.\n";
        return std::nullopt;
    };

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printtoks(std::vector<Token *> toks)
{
    for (int i = 0; i < toks.size(); i++)
    {
        Token *tok = toks[i];
        auto lexeme = tok->lexeme;

        if (i % 2 == 0)
        {
            std::cout << "\033[41m" << lexeme << "\033[0m";
        }
        else
        {
            std::cout << "\033[44m" << lexeme << "\033[0m";
        }
    };
};

int main()
{
    auto opt = ReadFile("test/main.doyt");
    if (!opt.has_value())
    {
        std::cout << "No source file found, exiting...\n";
        return 1;
    };
    auto src = opt.value();
    std::cout << "Reading " << src << '\n';
    Lexer *lex;

    try
    {
        lex = tokenize(&src);
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

    std::cout << lex->stream.size() << "tokens!";

    printtoks(lex->stream);

    return 0;
}