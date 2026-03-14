#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "../include/dlex.hpp"
#include "../include/parser.hpp"

std::string read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::in);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // read entire contents into stringstream
    return buffer.str();     // convert to std::string
}

int main() {
    std::string filename;
    std::cout << "enter filename";
    std::cin >> filename;

    std::string contents;
    try {
        contents = read_file(filename);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    LexOutput lexed = tokenize(contents);
    // Expr* ast_root = parse_file(lexed);
    // interpet idk
}