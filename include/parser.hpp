#pragma once

#define debug true

#include "dlex.hpp"
#include "arena.hpp"

// Operations, etc.
enum NodeGroup {
    _assignment,
    _paranthesis,
    _member,
    _expression,
    _operation,
    _atom,
    _uninitialised
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
    std::vector<ASTNode*, ArenaAllocator<ASTNode*>> children;
    ASTNode* parent;

    NodeGroup group;
    NodePayload payload_type;

    dTPayload payload;

    ~ASTNode();
    
    ASTNode(ASTNode* parent, NodeGroup group, NodePayload payload_type, dTPayload payload);
};

class Parser {
private:
    LexOutput lex;
    Arena arena;
    ArenaAllocator<ASTNode*> allocator;

    ASTNode* alloc_node();
    ASTNode* nud(Token op);
    ASTNode* led(Token op, ASTNode* left);

public:    
    ASTNode* parse_expression(float min_bp = 0);

    Parser(LexOutput lex_output);
};

