#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <stack>
#include <format>
#include "lexer.h"
#include "parser.h"

using namespace std;

std::pair<float, float> get_binding(NodeType type) {
    switch (type) {
        case NodeType::_pow:   return {9.0, 9.6}; // exponentiation

        case NodeType::_mul:     return {8.0, 8.0}; // multiplication
        case NodeType::_div:     return {8.0, 8.0}; // division
        case NodeType::_mod:     return {8.0, 8.0}; // modulo

        case NodeType::_add:     return {7.0, 7.0}; // addition
        case NodeType::_sub:     return {7.0, 7.0}; // subtraction

        case NodeType::_lsh:     return {6.0, 6.0}; // shift left
        case NodeType::_rsh:     return {6.0, 6.0}; // shift right

        case NodeType::_lt:      return {5.0, 5.0}; // less than
        case NodeType::_gt:      return {5.0, 5.0}; // greater than
        case NodeType::_lteq:    return {5.0, 5.0}; // less equal
        case NodeType::_gteq:    return {5.0, 5.0}; // greater equal
        case NodeType::_eq:      return {5.0, 5.0}; // equal
        case NodeType::_neq:     return {5.0, 5.0}; // not equal

        case NodeType::_and:     return {4.0, 4.0}; // bitwise and
        case NodeType::_xor:     return {3.5, 3.5}; // bitwise xor
        case NodeType::_or:      return {3.0, 3.0}; // bitwise or

        case NodeType::_and2:    return {2.0, 2.0}; // logical and
        case NodeType::_pipe2:   return {1.5, 1.5}; // logical or
        case NodeType::_tern:    return {1.0, 1.1}; // ternary

        case NodeType::_assign:  return {0.5, 0.4}; // assignment

        case NodeType::_unot:    return {10.0, 10.0}; // logical not
        case NodeType::_bnot:    return {10.0, 10.0}; // bitwise not
        case NodeType::_uneg:    return {10.0, 10.0}; // unary negation
        case NodeType::_inc:     return {10.0, 10.0}; // increment
        case NodeType::_dec:     return {10.0, 10.0}; // decrement

        default:                 return {0.0, 0.0};
    }
}

void parse_expression(Lexer* lex, int limit){
    Token* tok = lex->peek();

    int parandebt = 0;

    return;
};