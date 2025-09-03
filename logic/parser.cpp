#include "lexer.h"
#include "parser.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <stack>
#include <format>

using namespace std;

std::pair<int, int> get_binding(Code code){
    switch(code){
        case Code::_exc:       return {6,7}; // unary not
        case Code::_star2:     return {5,6}; // exponent
        case Code::_star:      return {4,4}; // multiplication
        case Code::_fslash:    return {4,4}; // division
        case Code::_percent:   return {4,4}; // modulo
        case Code::_plus:      return {3,3}; // addition
        case Code::_minus:     return {3,3}; // subtraction
        case Code::_gt2:       return {2,2}; // shift right
        case Code::_lt2:       return {2,2}; // shift left
        case Code::_and:       return {1,1}; // bitwise AND
        case Code::_hat:       return {1,1}; // bitwise XOR
        case Code::_pipe:      return {1,1}; // bitwise OR
    };
    return {0,0};
};

enum CodeGroup {
    _atom,
    _operation,
    _unmatched
};

CodeGroup get_group(Code code){
    switch(code){
        case Code::_exc:
        case Code::_star2:
        case Code::_star:
        case Code::_fslash:
        case Code::_percent:
        case Code::_minus:
        case Code::_gt2:
        case Code::_lt2:
        case Code::_and:
        case Code::_hat:
        case Code::_pipe:
            return CodeGroup::_operation;
        case Code::_ident:
        case Code::_litnum:
            return CodeGroup::_atom;
    };
    return CodeGroup::_unmatched;
};

SubType get_operation(Code code){
    switch(code){
        case Code::_exc:     return SubType::_unot;
        case Code::_and:     return SubType::_and;
        case Code::_pipe:    return SubType::_or;
        case Code::_hat:     return SubType::_xor;
        case Code::_star:    return SubType::_mul;
        case Code::_fslash:  return SubType::_div;
        case Code::_percent: return SubType::_mod;
        case Code::_gt2:     return SubType::_rsh;
        case Code::_lt2:     return SubType::_lsh;
        case Code::_plus:    return SubType::_add;
        case Code::_minus:   return SubType::_sub;
    };
    return SubType::_none;
}

void parse_expression(Lexer* lex, int limit){
    Token* tok = lex->peek();

    int parandebt = 0;

    return;
};