/*
#pragma once

#include <unordered_map>
#define debug true

#include <stdexcept>
#include <string>

#include "dlex.hpp"

enum BinaryExprPayload {
    // Binary Operations
    _add, _sub, _div, _mul, _mod, _pow, _log, _concat,

    // Bitwise Operations (excluding not)
    _and, _or, _xor, _rsh, _lsh,

    // Comparative Operations
    _neq, _eq, _lt, _gt, _lteq, _gteq,
    _if, _else, _func,

    _idx, // a.b, a[b]

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

class parser_error : public std::runtime_error {
public:
    parser_error(const std::string& message) 
        : std::runtime_error(message) {}
    
    parser_error(const char* message) 
        : std::runtime_error(message) {}
};

enum class ExprType { Binary, Unary, Literal, Identifier, Call };

struct Expr {
    ExprType kind;
    virtual ~Expr() = default;
};

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

struct CallExpr : Expr {
    IdentifierExpr* fn;
    std::vector<Expr*> args;
};

std::string AST_to_str(Expr* root, int indent = 0);

enum class TypeKind {
    _float, _bool, _string,
    _struct, _array, _function,
    _pointer // ? 
};

struct StructField; // forward decl.

struct Type {
    TypeKind kind;
    union {
        struct {
            std::vector<StructField>* fields;          
        } struct_info;
        struct {
            Type* element_type;
            size_t size;
        } array_info;
        struct {
            Type* target_type;
        } ptr_info;
    };
};

struct StructField {
    std::string name;
    Type type;
};

template <typename T>
concept DerivedFromExpr = std::is_base_of_v<Expr, T>;

class Parser {
private:
    std::unordered_map<std::string, Type> type_registry; // holds all the types that have been seen so far
    // also holds all the primitives like float at the beginning

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

    template <DerivedFromExpr T>
    T* alloc_expr();

    Expr* parse_statement();
    Expr* parse_expression(float min_bp = 0);

    Expr* parse_whole();

    Parser(LexOutput lex_output);
};

*/