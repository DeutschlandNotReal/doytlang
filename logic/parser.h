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

enum class NodeType {
    _operation,
    _atom,
    _paranthesis,
    _assignment,
    _expression,
    _index,
    _call
};

enum class SubType {
    __CONTROL,
    _none,
    _miscpunc,
    __OPERATIONS,
    _mul,
    _div,
    _mod,
    _add,
    _sub,
    _and,
    _xor,
    _or,
    _unot,
    _lsh,
    _rsh,
    _eq,
    _call,
    __ATOMS,
    _ident,
    _strlit,
    _numlit,

};

struct Node {
    NodeType type;
    SubType  sub;
    Token* tok;
    Node* parent;
    std::vector<Node*> children;

    int right;  // right-most token index in the node
    int left;   // left-most token index in the node

    static Node* conjoin(NodeType type, SubType sub, Token* tok, std::vector<Node*> children){
        Node* parent = new Node{type, sub, tok, nullptr, children};
        return parent;
    }

    static Node* new_atom(NodeType type, SubType sub, Token* tok, Node* parent=nullptr){
        Node* atom = new Node{type, sub, tok, parent, {}};
        return atom;
    }

    void parent_to(Node* newpar){
        parent = newpar;
        newpar->children.push_back(this);
    }
};
