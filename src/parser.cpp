#include "parser.hpp"

#include <vector>
#include <string>
// weird error in dlex.cpp, using enum doesnt work?!
// amazing name
// typedef  ASTNodeChildrenVec; k this chudded thing keeps erroring

struct ASTNode {
    std::vector<ASTNode*, ArenaAllocator<ASTNode*>> children;
    ASTNode* parent;

    NodeGroup group;
    NodePayload payload_type;

    dTPayload payload;

    ~ASTNode() {
        for (ASTNode* child : children) {
            delete child;
        }
    };
    
    ASTNode(ASTNode* parent, NodeGroup group, NodePayload payload_type, dTPayload payload, ArenaAllocator<ASTNode*> allocator)
        :   children(allocator),  // initialize member vector with your allocator
            parent(parent),
            group(group),
            payload_type(payload_type),
            payload(payload)
    {}; 
};

// bool is_atom(Token token) {
//     return 
//         token.type == TK_LIT_FLOAT ||
//         token.type == TK_LIT_DOUBLE ||
//         token.type == TK_LIT_BOOL ||
//         token.type == TK_LIT_STR ||
//         token.type == TK_LIT_INT ||
//         token.type == TK_IDENT;
// }

NodePayload get_op_payload(Token& op) {
    switch (op.type) {
        case dTCode::plusT:  return NodePayload::_add;
        case dTCode::minusT: return NodePayload::_sub;
        case dTCode::starT:  return NodePayload::_mul;
        case dTCode::slashT: return NodePayload::_div;
        default: return NodePayload::_error;
    }
}

// parse_expression should probably handle garbage tokens
std::pair<float,float> binding_power(Token& op) {
    switch(op.type) {
        case dTCode::plusT:
        case dTCode::minusT: return {1.0, 1.1};
        case dTCode::starT:
        case dTCode::slashT: return {2.0, 2};
        return {0.0,0.0};
    }
} 

bool is_literal(dTCode code) {
    return  code == dTCode::identT ||
            code == dTCode::numT ||
            code == dTCode::strT ||
            code == dTCode::charT ||
            code == dTCode::boolT ||
            code == dTCode::nullT;
}




ASTNode* parse_whole(LexOutput lex) {
    
}

// value of out is the return
ASTNode* Parser::nud(Token op) {
    ASTNode* ret = alloc_node();
    
    if ( is_literal(op.type) ) {
        ret->payload_type = get_op_payload(op);
        ret->payload = op.pl;
        return ret;
    }

    switch (op.type) {
        case dTCode::minusT:
            ret->payload_type = NodePayload::_neg;
            ret->children.push_back(parse_expression(30));
            return ret; // 0 errors in projectt!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        case dTCode::lparT: // i took a small break after the strtof ragebait
            ASTNode* right = parse_expression(0);
            Token expected_rparen = lex.consume();
            if (expected_rparen.type != dTCode::rparT) {
                // TODO: ERROR
                return;
            }
            return right;
                
    }
}

ASTNode* Parser::led(Token op, ASTNode* left) {

}

ASTNode* Parser::alloc_node() {
    ASTNode node = ASTNode(nullptr, NodeGroup::_uninitialised, NodePayload::_error, (dTPayload)0, allocator);
    return (ASTNode*) arena.alloc(sizeof(node), alignof(node));
}

ASTNode* Parser::parse_expression(float min_bp = 0) {
    #define peek       lex.tpeek();
    #define consume    lex.tconsume();
    #define ahead      lex._th;
    #define tok_oob(t) t.type == dTCode::invalidT || t.type == dTCode::eofT

    ASTNode* left = (ASTNode*)0; // please the compiler for now
    
    return left;
}

Parser::Parser(LexOutput lex_output)
    :   lex(lex_output),
        arena(Arena::create(1024)),
        allocator(&arena) {}