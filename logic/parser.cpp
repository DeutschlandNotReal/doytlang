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
        case NodeType::_neq:   return {2.0f, 2.0f}; // !=
        case NodeType::_cand:  return {2.0f, 2.0f}; // &&
        case NodeType::_cnot:  return {2.0f, 2.0f}; // !!
        case NodeType::_cxor:  return {2.0f, 2.0f}; // ^^
        case NodeType::_cor:   return {2.0f, 2.0f}; // ||

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

NodeType match_nodetype(Code now, Code prev, Code next){
    switch(now){
        case Code::_and:     return NodeType::_and;
        case Code::_and2:    return NodeType::_cand;
        case Code::_pipe:    return NodeType::_or;
        case Code::_pipe2:   return NodeType::_cor;
        case Code::_hat:     return NodeType::_xor;
        case Code::_hat2:    return NodeType::_cxor;
        case Code::_exc:     return NodeType::_unot;
        case Code::_exc2:    return NodeType::_cnot;
        
        case Code::_plus:    return NodeType::_add;
        case Code::_minus:switch(next){
            case Code::_litnum:
            case Code::_ident:
                return NodeType::_uneg;
            default: return NodeType::_sub;
        };
        case Code::_star:    return NodeType::_mul;
        case Code::_star2:   return NodeType::_pow;
        case Code::_fslash:  return NodeType::_div;
        case Code::_percent: return NodeType::_mod;
        
        case Code::_eqeq:    return NodeType::_eq;
        case Code::_neq:     return NodeType::_neq;
        case Code::_gt:      return NodeType::_gt;
        case Code::_gteq:    return NodeType::_gteq;
        case Code::_lt:      return NodeType::_lt;
        case Code::_lteq:    return NodeType::_lteq;

        default: return NodeType::_none;
    };
};


void parse_expression(Lexer* lex, int limit){
    int startpoint = lex->pointer;


};