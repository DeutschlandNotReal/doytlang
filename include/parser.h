#pragma once
#include <vector>
#include <string>
#include "lexer.h"
#include <optional>

enum class ParserErrors {
    _malformed_num,
    _malformed_str,
    _unclosed_para,
    _unclosed_brace,
    _unclosed_bracket
};

enum class NodeCategory {
    _operation,
    _assignment,
    _call,
    _atom,
    _paranthesis,
    _expression
};

enum class NodeType {
    _none,
    _miscpunc,

    _assign,

    // Paranthesis
    _paranopen,
    _paranclose,

    //Binary Operations
    _mul, _div, _mod, _add, _sub, _pow, _log,
    
    // Unary Operations
    _uneg, _inc, _dec, _exp, _ln,

    // Bitwise Operations
    _and, _xor, _or, _unot, _lsh, _rsh,

    // Comparators
    _eq, _gt, _lt, _gteq, _lteq, _new,

    // Ternary
    _tern,

    // Atoms
    _ident,
    _strlit,
    _numlit,
    _boolit
};

struct Node {
    NodeCategory cat;
    NodeType  type;
    Token* tok;
    Node* parent;
    std::vector<Node*> children;

    int right;  // right-most token index in the node
    int left;   // left-most token index in the node

    static Node* conjoin(NodeCategory cat, NodeType type, Token* tok, std::vector<Node*> children){
        Node* parent = new Node{cat, type, tok, nullptr, children};
        return parent;
    }

    static Node* new_atom(NodeCategory cat, NodeType type, Token* tok, Node* parent=nullptr){
        Node* atom = new Node{cat, type, tok, parent, {}};
        return atom;
    }

    void parent_to(Node* newpar){
        parent = newpar;
        newpar->children.push_back(this);
    }
};