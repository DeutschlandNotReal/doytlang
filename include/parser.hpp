#pragma once

#define debug true

#include "dlex.hpp"


#include <vector>
#include <string>

using namespace std;

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
    _neg, _not, _exp, _ln, _abs, _flr, _sqrt,

    // Bitwise Operations (excluding not)
    _and, _or, _xor, _rsh, _lsh,

    // Comparative Operations
    _neq, _eq, _lt, _gt, _lteq, _gteq,
    _if, _else, _func,

    _error // something wrong has happened
};

struct ASTNode {
    vector<ASTNode*> children;
    ASTNode* parent;

    NodeGroup group;
    NodePayload payload_type;

    TokPl payload;

    ~ASTNode() {
        for (ASTNode* child : children) {
            delete child;
        }
    };
    
    ASTNode(ASTNode* parent, NodeGroup group, NodePayload payload_type, TokPl payload) {
        children = vector<ASTNode*>();
        parent = parent;
        group = group;
        payload_type = payload_type;
        payload = payload;
    };
};

bool is_atom(Token& token) {
    return 
        token.type == TK_LIT_FLOAT ||
        token.type == TK_LIT_DOUBLE ||
        token.type == TK_LIT_BOOL ||
        token.type == TK_LIT_STR ||
        token.type == TK_LIT_INT ||
        token.type == TK_IDENT;
}

bool get_op_payload(Token& op) {
    switch (op.type) {
        case TK_PLUS:  return NodePayload::_add;
        case TK_MINUS: return NodePayload::_sub;
        case TK_STAR:  return NodePayload::_mul;
        case TK_SLASH: return NodePayload::_div;
        case TK_PERC:  return NodePayload::_mod;
        case TK_STAR2: return NodePayload::_pow;
        default: return NodePayload::_error;
    }
}

// parse_expression should probably handle garbage tokens
pair<float,float> binding_power(Token& op) {
    switch(op.type) {
        case TK_PLUS:
        case TK_MINUS: return {1.0, 1.1};
        case TK_PERC:
        case TK_STAR:
        case TK_SLASH: return {2.0, 2};
    }
} 

ASTNode* parse_expression(LexContext lex, float min_bp = 0) {
   #define peek       lex._t;
   #define consume    lex.step();
   #define ahead      lex._th;
   #define tok_oob(t) t.type == TK_INVALID || t.type == TK_EOF

    Token lhs_token = peek;
    if (lhs_token.type == TK_INVALID) {
        throw runtime_error("Tried to parse past EOF");
    }
    
    if (!is_atom(lhs_token)) {
        throw runtime_error("Expected float literal when parsing expression");
    }

    ASTNode* lhs = new ASTNode(
        nullptr,
        NodeGroup::_atom,
        NodePayload::_litfloat,
        lhs_token.payload
    );

    while (1) {
        Token op = peek;
        if (op.type == TK_SEMI) {
            break;
        }
        if (tok_oob(op)) {
            throw runtime_error("Reached EOF while parsing expression");
        }
        if (get_op_payload(op) == NodePayload::_error) {
            throw runtime_error("Expected valid operator while parsing expression");
        }

        consume;

        auto [lbp, rbp] = binding_power(op);
        if (lbp < min_bp) {
            break;
        }
        ASTNode* rhs = parse_expression(lex, rbp); // scary recursion but should be fine cuz its only for expressions
        lhs = new ASTNode(
            nullptr,
            NodeGroup::_operation,
            get_op_payload(op),
            lhs_token.payload
            {lhs, rhs}
        );
    }
    return lhs;
}

ASTNode* parse_whole(LexContext lex) {

}