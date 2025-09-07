#pragma once

#define debug true

#include "lexer.hpp"

#include <vector>
#include <string>



// Operations, etc.
enum NodeGroup {
    _assignment,
    _paranthesis,
    _member,
    _expression,
    _operation,
    _atom
};

// addition, sub, etc.
enum NodePayload { 
    // literals
    _litfloat, _litbool, // dont bother with litbool for now (im not even lexxing it </3)
    _litstr,

    // Binary Operations
    _add, _sub, _div, _mul, _mod, _pow, _log, _concat,

    // Unary Operations
    _neg, _not, _exp, _ln, _abs, _flr,

    // Bitwise Operations (exluciding not)
    _and, _or, _xor, _rsh, _lsh,

    // Comparative Operations
    _neq, _eq, _lt, _gt, _lteq, _gteq,
    _if, _else, _func,

    _error // something wrong has happened
};

class ASTNode {
    std::vector<ASTNode*> children;
    ASTNode* parent;

    NodeGroup group;
    NodePayload payload_type;

    PayloadVariant payload;

    ~ASTNode() {
        for (ASTNode* child : children) {
            delete child;
        }
    };
    
    ASTNode(ASTNode* parent, NodeGroup group, NodePayload payload_type, PayloadVariant payload) {
        children = std::vector<ASTNode*>();
        parent = parent;
        group = group;
        payload_type = payload_type;
        payload = payload;
    };
};

bool is_atom(Token *token) {
    return token->code == TokenCode::_litfloat;
}

bool get_op_payload(Token op) {
    switch (op.code) {
        case TokenCode::_plus:  return NodePayload::_add;
        case TokenCode::_minus: return NodePayload::_sub;
        case TokenCode::_star:  return NodePayload::_mul;
        case TokenCode::_fslash:return NodePayload::_div;
        default: return NodePayload::_error;
    }
}

// parse_expression should probably handle garbage tokens
pair<float,float> binding_power(Token op) {
    switch(op.code) {
        case TokenCode::_plus:
        case TokenCode::_minus: return {1.0, 1.1};
        case TokenCode::_perc:
        case TokenCode::_star:
        case TokenCode::_fslash:return {2.0, 2};
    }
} 

// parse like (a * b / 5)
ASTNode* parse_expression(Lexer& lex, float min_bp = 0) {
    // minor skidding action..
    // praying that this works.. im off to sleep -fudu

    Token* lhs_token = lex.peek().value_or(nullptr);
    if (!lhs_token) {
        throw runtime_error("Tried to parse past EOF");
    }
    if (!is_atom(lhs_token)) {
        throw runtime_error("Expected float literal when parsing expression");
    }
    ASTNode* lhs = new ASTNode(
        nullptr,
        NodeGroup::_atom,
        NodePayload::_litfloat,
        get<float>(lhs_token->payload)
    );
    while (1) {
        Token* op = lex.peek().value_or(nullptr);
        if (op->code == TokenCode::_semi) {
            break;
        }
        if (!op) {
            throw runtime_error("Reached EOF while parsing expression");
        }
        if (!is_op(*op)) {
            throw runtime_error("Expected valid operator while parsing expression");
        }

        lex.consume();

        auto [lbp, rbp] = binding_power(op);
        if (lbp < min_bp) {
            break;
        }
        ASTNode* rhs = parse_expression(lex, rbp, lhs); // scary recursion but should be fine cuz its only for expressions
        lhs = new ASTNode(
            nullptr,
            NodeGroup::_operation,
            get_op_payload(*op),
            get<float>(lhs_token->payload),
            {lhs, rhs}
        );
    }
    return lhs;
}
// should we have semicolons?
// maybe, they are tokens rn _semi

// yeah maybe to end an expression
// semicolons to end expression

// 😨
ASTNode* parse_whole(Lexer &lex) {

}