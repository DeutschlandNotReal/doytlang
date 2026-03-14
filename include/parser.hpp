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

enum BinaryExprPayload {
    // Binary Operations
    _add, _sub, _div, _mul, _mod, _pow, _log, _concat,

    // Bitwise Operations (excluding not)
    _and, _or, _xor, _rsh, _lsh,

    // Comparative Operations
    _neq, _eq, _lt, _gt, _lteq, _gteq,
    _if, _else, _func,

    _bin_error // something wrong has happened
};

enum LiteralExprPayload {
    // literals
    _litfloat, _litbool, // dont bother with litbool for now (im not even lexxing it </3)
    _litstr,

    _error // something wrong has happened

};

enum UnaryExprPayload {
    _neg, _not, _exp, _ln, _abs, _flr, _sqrt,

    _unary_error // something wrong has happened
};

struct Expr { virtual ~Expr() = default; };

struct BinaryExpr : Expr {
    Expr* left;
    Expr* right;
    BinaryExprPayload op;
};

struct UnaryExpr : Expr {
    UnaryExprPayload op;
    Expr* operand;
};

struct LiteralExpr : Expr {
    LiteralExprPayload type;
    dTPayload value;
};

struct IdentifierExpr : Expr {
    std::string name;
};

// struct ASTNode {
//     std::vector<ASTNode*, ArenaAllocator<ASTNode*>> children;
//     ASTNode* parent;

//     NodeGroup group;
//     NodePayload payload_type;

//     dTPayload payload;

//     ~ASTNode();
    
//     ASTNode(ASTNode* parent, NodeGroup group, NodePayload payload_type, dTPayload payload, ArenaAllocator<ASTNode*> allocator);
// };

class Parser {
private:
    LexOutput lex;
    Arena arena;

    
    Expr* nud(Token op);
    Expr* led(Token op, Expr* left);
    
public:    
    // // ASTNode* alloc_node();
    // BinaryExpr* alloc_bin_expr();
    // UnaryExpr* alloc_unary_expr();
    // LiteralExpr* alloc_literal_expr();
    // IdentifierExpr* alloc_identifier_expr();
    template <typename T>
    T* alloc();

    Expr* parse_expression(float min_bp = 0);

    Parser(LexOutput lex_output);
};

