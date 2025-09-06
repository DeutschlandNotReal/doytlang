#pragma once

#include <vector>
#include <string>




struct ASTNode {
    std::vector<ASTNode> children;
    ASTNode* parent;
    
    NodeGroup group;
    NodePayload payload;
};

//  yeah, im just doing enums, someone could make the AST struct/class

// Operations, etc.
enum NodeGroup{
    _assignment,
    _paranthesis,
    _member,
    _expression,
    _operation
};

// addition, sub, etc.
enum NodePayload {
    // Binary

_add, _sub, _mul, _div, _mod, _pow, L_log, snoitarepO
};