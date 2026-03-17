#include "../include/parser.hpp"
#include "dlex.hpp"

std::string binary_op_to_string(BinaryExprPayload op) {
    switch (op) {
        case BinaryExprPayload::_add:    return "add";
        case BinaryExprPayload::_sub:    return "subtract";
        case BinaryExprPayload::_div:    return "divide";
        case BinaryExprPayload::_mul:    return "multiply";
        case BinaryExprPayload::_mod:    return "modulo";
        case BinaryExprPayload::_pow:    return "power";
        case BinaryExprPayload::_log:    return "logarithm";
        case BinaryExprPayload::_concat: return "concatenate";
        case BinaryExprPayload::_and:    return "and";
        case BinaryExprPayload::_or:     return "or";
        case BinaryExprPayload::_xor:    return "xor";
        case BinaryExprPayload::_rsh:    return "right shift";
        case BinaryExprPayload::_lsh:    return "left shift";
        case BinaryExprPayload::_neq:    return "not equal";
        case BinaryExprPayload::_eq:     return "equal";
        case BinaryExprPayload::_lt:     return "less than";
        case BinaryExprPayload::_gt:     return "greater than";
        case BinaryExprPayload::_lteq:   return "less than or equal";
        case BinaryExprPayload::_gteq:   return "greater than or equal";
        case BinaryExprPayload::_if:     return "if";
        case BinaryExprPayload::_else:   return "else";
        case BinaryExprPayload::_func:   return "function";
        case BinaryExprPayload::_idx:    return "index";
        case BinaryExprPayload::_bin_error: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string literal_type_to_string(LiteralExprPayload type) {
    switch (type) {
        case LiteralExprPayload::_litfloat: return "float literal";
        case LiteralExprPayload::_litbool:  return "boolean literal";
        case LiteralExprPayload::_litstr:   return "string literal";
        case LiteralExprPayload::_error:    return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string unary_op_to_string(UnaryExprPayload op) {
    switch (op) {
        case UnaryExprPayload::_neg:  return "negate";
        case UnaryExprPayload::_not:  return "not";
        case UnaryExprPayload::_exp:  return "exponential";
        case UnaryExprPayload::_ln:   return "natural log";
        case UnaryExprPayload::_abs:  return "absolute value";
        case UnaryExprPayload::_flr:  return "floor";
        case UnaryExprPayload::_sqrt: return "square root";
        case UnaryExprPayload::_unary_error: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string variant_to_string(const dTPayload& payload) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, float>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        }
        return "";  // fallback
    }, payload);
}

std::string AST_to_str(Expr* root, int indent) {
    std::string ret = std::string(indent, '\t');
    
    switch (root->kind) {
        case ExprType::Literal: {
            LiteralExpr* root_casted = (LiteralExpr*)root;
            ret.append(literal_type_to_string(root_casted->type) + ": ");
            ret.append(variant_to_string( ((LiteralExpr*)root)->value ));
            break;
        }
        case ExprType::Unary: {
            UnaryExpr* root_casted = (UnaryExpr*)root;
            ret.append(unary_op_to_string(root_casted->op) + ": \n");
            ret.append(AST_to_str( root_casted->operand, indent + 1 ));
            break;;
        }
        case ExprType::Binary: {
            BinaryExpr* root_casted = (BinaryExpr*)root;
            ret.append(binary_op_to_string(root_casted->op) + ": \n");
            ret.append(AST_to_str( root_casted->left , indent + 1 ));
            ret.append(AST_to_str( root_casted->right, indent + 1 ));
            break;
        }
        case ExprType::Identifier: {
            IdentifierExpr* root_casted = (IdentifierExpr*)root;
            ret.append("ident: " + root_casted->name);
            break;
        }
        case ExprType::Call: {
            CallExpr* root_casted = (CallExpr*)root;
            ret.append("call:\n");
            ret.append(std::string(indent + 1, '\t') + "fn:\n");
            ret.append(AST_to_str(root_casted->fn, indent + 2));
            for (size_t i = 0; i < root_casted->args.size(); i++) {
                ret.append(std::string(indent + 1, '\t') + "arg" + std::to_string(i) + ":\n");
                ret.append(AST_to_str(root_casted->args[i], indent + 2));
            }
            break;
        }
    }
    return ret;
}


BinaryExprPayload get_bin_op_payload(Token& op) {
    switch (op.type) {
        case dTCode::plusT:  return BinaryExprPayload::_add;
        case dTCode::minusT: return BinaryExprPayload::_sub;
        case dTCode::starT:  return BinaryExprPayload::_mul;
        case dTCode::slashT: return BinaryExprPayload::_div;
        default: throw parser_error("Tried to get the operator payload type of sth thats not an operator");
    }
}

LiteralExprPayload get_literal_payload(Token& literal) {
    switch (literal.type) {
        case dTCode::numT: return LiteralExprPayload::_litfloat;
        case dTCode::boolT: return LiteralExprPayload::_litbool;
        case dTCode::strT: return LiteralExprPayload::_litstr;
        default: throw parser_error("Tried to get literal payload type of sth thats not a literal");
    }
}


#define BP_ADD_SUB ((std::pair<float,float>){1.0, 1.1})
#define BP_MUL_DIV ((std::pair<float,float>){2.0, 2.1})
#define BP_IDX ((std::pair<float,float>){10.0, 10.1})
#define BP_FN_CALL ((std::pair<float,float>) {6.0, 6.1})
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
        LiteralExpr* ret = alloc_expr<LiteralExpr>(); 
        ret->type = get_literal_payload(op);
        ret->value = op.pl;
        return ret;
    }

    switch (op.type) {
        case dTCode::minusT: {
            UnaryExpr* ret = alloc_expr<UnaryExpr>();
            ret->op = UnaryExprPayload::_neg;
            ret->operand = parse_expression(BP_NEG); // very high binding power since minus needs to bind very tightly

            return ret;
        }

        case dTCode::lparT: {
            Expr* right = parse_expression(0);
            Token expected_rparen = lex.consume();
            if (expected_rparen.type != dTCode::rparT) {
                throw  parser_error("Expected ')' to close '('") ;
            }
            return right;
        }

        case dTCode::identT: { // stuff like `x` and `math`
            IdentifierExpr* ident = alloc_expr<IdentifierExpr>();
            ident->name = std::get<std::string>(op.pl);
        }

        default:
            throw parser_error("unexpected token seen");
                
    }
}

Expr* Parser::led(Token op, Expr* left) {
    switch (op.type) {
        case dTCode::plusT:
        case dTCode::minusT:
        case dTCode::starT:
        case dTCode::slashT: {
            Expr* right = parse_expression(binding_power(op).second);
            BinaryExpr* expr = alloc_expr<BinaryExpr>();
            expr->left = left;
            expr->right = right;
            expr->op = get_bin_op_payload(op);

            return expr;
        }
        case dTCode::dot: {
            if (left->kind != ExprType::Identifier) {
                throw parser_error("Expected identifier before '.'");
            }
            Expr* right = parse_expression(BP_IDX.second);
            BinaryExpr* expr = alloc_expr<BinaryExpr>();
            expr->left = left;
            expr->right = right;
            expr->op = BinaryExprPayload::_idx;
            return expr;
        }
        case dTCode::lparT: {
            std::vector<Expr*> args;

            if (lex.peek().type != dTCode::rparT) { // not empty call
                while (true) {
                    args.push_back(parse_expression(0));

                    Token next = lex.peek();
                    if (next.type == dTCode::comma) {
                        lex.consume();
                        continue;
                    }
                    break;
                }
            }

            Token expected_rparen = lex.consume();
            if (expected_rparen.type != dTCode::rparT) {
                throw parser_error("expected ')' to close func call");
            }

            CallExpr* expr = alloc_expr<CallExpr>();
            expr->fn = (IdentifierExpr*) left;
            expr->args = args;
            return expr;
        }

        default:
            throw parser_error("Unexpected token found during parsing");
    }
}

template <typename T> struct ExprTypeTraits;
template <> struct ExprTypeTraits<BinaryExpr>     { static constexpr ExprType value = ExprType::Binary; };
template <> struct ExprTypeTraits<UnaryExpr>      { static constexpr ExprType value = ExprType::Unary; };
template <> struct ExprTypeTraits<LiteralExpr>    { static constexpr ExprType value = ExprType::Literal; };
template <> struct ExprTypeTraits<IdentifierExpr> { static constexpr ExprType value = ExprType::Identifier; };
template <> struct ExprTypeTraits<CallExpr>       { static constexpr ExprType value = ExprType::Call; };


template <DerivedFromExpr T>
T* Parser::alloc_expr() {
    T* expr = (T*) arena.alloc(sizeof(T), alignof(T));
    expr->kind = ExprTypeTraits<T>::value;
    return expr;
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

Expr* Parser::parse_statement() {
    Token first_tok = lex.consume();
    if (first_tok.type == dTCode::identT) { // variable assignemnt? (starts with type possibly)
        std::string name = std::get<std::string>(first_tok.pl);
        if (type_registry.contains(name)) {
            Type t = type_registry.at(name);

        }
    }
}

Parser::Parser(LexOutput lex_output)
    :   lex(lex_output),
        arena(Arena::create(1024)),
        type_registry({
            { "float" , Type{TypeKind::_float } },
            { "string", Type{TypeKind::_string} },
            { "bool"  , Type{TypeKind::_bool  } },
        })
        {}