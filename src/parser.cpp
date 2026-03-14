#include "../include/parser.hpp"

#include <stdexcept>

BinaryExprPayload get_bin_op_payload(Token& op) {
    switch (op.type) {
        case dTCode::plusT:  return BinaryExprPayload::_add;
        case dTCode::minusT: return BinaryExprPayload::_sub;
        case dTCode::starT:  return BinaryExprPayload::_mul;
        case dTCode::slashT: return BinaryExprPayload::_div;
        default: throw std::runtime_error("Tried to get the operator payload type of sth thats not an operator");
    }
}

LiteralExprPayload get_literal_payload(Token& literal) {
    switch (literal.type) {
        case dTCode::numT: return LiteralExprPayload::_litfloat;
        case dTCode::boolT: return LiteralExprPayload::_litbool;
        case dTCode::strT: return LiteralExprPayload::_litstr;
        default: throw std::runtime_error("Tried to get literal payload type of sth thats not a literal");
    }
}


#define BP_ADD_SUB ((std::pair<float,float>){1.0, 1.1})
#define BP_MUL_DIV ((std::pair<float,float>){2.0, 2.1})
#define BP_NEG (5)
// parse_expression should probably handle garbage tokens
std::pair<float,float> binding_power(Token& op) {
    switch(op.type) {
        case dTCode::plusT:
        case dTCode::minusT: return BP_ADD_SUB;
        case dTCode::starT:
        case dTCode::slashT: return BP_MUL_DIV;
        default: return {0.0,0.0};
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




// Expr* parse_line(LexOutput lex) {
    
// }

Expr* Parser::nud(Token op) {
    if ( is_literal(op.type) ) {
        LiteralExpr* ret = alloc<LiteralExpr>(); 
        ret->type = get_literal_payload(op);
        ret->value = op.pl;
        return ret;
    }

    switch (op.type) {
        case dTCode::minusT: {
            UnaryExpr* ret = alloc<UnaryExpr>();
            ret->op = UnaryExprPayload::_neg;
            auto [_, rbp] = binding_power(op);
            ret->operand = parse_expression(BP_NEG); // very high binding power since minus needs to bind very tightly

            return ret;
        }

        case dTCode::lparT: {
            Expr* right = parse_expression(0);
            Token expected_rparen = lex.consume();
            if (expected_rparen.type != dTCode::rparT) {
                throw  std::runtime_error("Expected ')' to close '('") ;
            }
            return right;
        }

        case dTCode::identT: { // stuff like `x` and `math`
            IdentifierExpr* ident = alloc<IdentifierExpr>();
            ident->name = std::get<std::string>(op.pl);
        }

        default:
            throw std::runtime_error("unexpected token seen");
                
    }
}

Expr* Parser::led(Token op, Expr* left) {
    switch (op.type) {
        case dTCode::plusT:
        case dTCode::minusT:
        case dTCode::starT:
        case dTCode::slashT: {
            Expr* right = parse_expression(binding_power(op).second);
            BinaryExpr* expr = alloc<BinaryExpr>();
            expr->left = left;
            expr->right = right;
            expr->op = get_bin_op_payload(op);

            return expr;
        }
        default:
            throw std::runtime_error("Unexpected token found during parsing");
    }
}

template <typename T>
T* Parser::alloc() {
    return (T*) arena.alloc(sizeof(T), alignof(T));
}

Expr* Parser::parse_expression(float min_bp) {
    Expr* left = nud(lex.consume());
    
    while (1) {
        Token cur = lex.peek();
        if (!cur.valid() || (binding_power(cur).first <= min_bp))
            break;
        lex.consume();
        Expr* left = led(cur, left);
    }

    return left;
}

Parser::Parser(LexOutput lex_output)
    :   lex(lex_output),
        arena(Arena::create(1024)) {}