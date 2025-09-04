#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <stack>
#include <format>
#include "../include/parser.h"
#include "../include/lexer.h"

std::pair<float, float> get_binding_power(NodeType type) {
    switch (type) {
        // --- Binary Operations ---
        case NodeType::_pow:   return {9.0f, 9.5f}; // exponentiation
        case NodeType::_log:   return {9.0f, 9.5f}; // logarithm (will be optimized with constant base don't you worry)

        case NodeType::_mul:   return {8.0f, 8.0f}; // multiplication
        case NodeType::_div:   return {8.0f, 8.0f}; // division
        case NodeType::_mod:   return {8.0f, 8.0f}; // modulo

        case NodeType::_add:   return {7.0f, 7.0f}; // addition
        case NodeType::_sub:   return {7.0f, 7.0f}; // subtraction

        case NodeType::_lsh:   return {6.0f, 6.0f}; // shift left
        case NodeType::_rsh:   return {6.0f, 6.0f}; // shift right

        case NodeType::_and:   return {5.0f, 5.0f}; // bitwise AND
        case NodeType::_xor:   return {4.0f, 4.0f}; // bitwise XOR
        case NodeType::_or:    return {3.0f, 3.0f}; // bitwise OR

        // --- Comparators ---
        case NodeType::_eq:    return {2.0f, 2.0f}; // ==
        case NodeType::_gt:    return {2.0f, 2.0f}; // >
        case NodeType::_lt:    return {2.0f, 2.0f}; // <
        case NodeType::_gteq:  return {2.0f, 2.0f}; // >=
        case NodeType::_lteq:  return {2.0f, 2.0f}; // <=
        case NodeType::_new:   return {2.0f, 2.0f}; // "new" operator (constructor-like)

        // --- Unary Operations ---
        case NodeType::_uneg:  return {10.0f, 10.0f}; // negation
        case NodeType::_inc:   return {10.0f, 10.0f}; // increment
        case NodeType::_dec:   return {10.0f, 10.0f}; // decrement
        case NodeType::_exp:   return {10.0f, 10.0f}; // exponential
        case NodeType::_ln:    return {10.0f, 10.0f}; // natural log
        case NodeType::_unot:  return {10.0f, 10.0f}; // unary NOT

        // --- Ternary ---
        case NodeType::_tern:  return {1.0f, 1.0f}; // ternary ? :

        default: return {0.0f, 0.0f}; // non-operator types
    }
}


void parse_expression(Lexer* lex, int limit){
    Token* tok = lex->peek();

    int parandebt = 0;

    return;
};