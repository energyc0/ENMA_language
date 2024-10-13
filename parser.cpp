#include <exception>
#include <iostream>
#include "enma_types.h"
#include "parser.h"

ast_node_type_e parser::reinterpret_arith_op(const token_t& t){
    if(t.get_type() != token_type_e::OPERATOR && t.get_type() != token_type_e::END){
        throw std::runtime_error("syntax error\noperator expected\n");
    }
    switch (static_cast<operator_type_e>(t.get_value())){
        case operator_type_e::ADD: return ast_node_type_e::ADD;
        case operator_type_e::SUB: return ast_node_type_e::SUB;
        case operator_type_e::DIV: return ast_node_type_e::DIV;
        case operator_type_e::MUL: return ast_node_type_e::MUL;
        case operator_type_e::END: return ast_node_type_e::END;
            break;
        default:
            throw std::runtime_error("syntax error\nundefined operator\n");
            break;
    }
    return ast_node_type_e::END;
}

int parser::get_arith_op_precedence(ast_node_type_e op){
    switch (op){
        case ast_node_type_e::DIV:
        case ast_node_type_e::MUL:
            return 100;
        case ast_node_type_e::ADD:
        case ast_node_type_e::SUB:
            return 10;
        case ast_node_type_e::END:
            return 0;
        default:
            throw std::runtime_error("syntax error\nunexpected arithmetic operation\n");
            break;
    }
    return -1;
}

ast_node_t* parser::get_primary_expr(token_storage& tokens){
    auto t = tokens.get_current();
    switch(t.get_type()){
        case token_type_e::CONSTANT:
            return new ast_node_t(t.get_value(), ast_node_type_e::NUM, nullptr, nullptr);
        case token_type_e::END:
            return new ast_node_t(t.get_value(), ast_node_type_e::END, nullptr, nullptr);
        default:
            throw std::runtime_error("syntax error\nprimary expression expected\n");
        break;
    }
    return nullptr;
}

ast_node_t* parser::bin_expr_parse(token_storage& tokens, int prev_op_precedence){
    ast_node_t* left = get_primary_expr(tokens);
    if(left->type == ast_node_type_e::END)
        return left;

    ast_node_t* right = nullptr;
    auto token = tokens.get_next();
    while(get_arith_op_precedence(reinterpret_arith_op(token)) > prev_op_precedence){
        tokens.next();
        right = bin_expr_parse(tokens, get_arith_op_precedence(reinterpret_arith_op(token)));

        left = new ast_node_t(0, reinterpret_arith_op(token), left, right);
        token = tokens.get_current();
        if(token.get_type() == token_type_e::END){
            break;
        }
    }
    return left;
}

std::shared_ptr<ast_node_t> parser::binary_expr(token_storage& tokens, bool& result){
    try{
        std::shared_ptr<ast_node_t> root =
         std::shared_ptr<ast_node_t>(bin_expr_parse(tokens, parser::get_arith_op_precedence(ast_node_type_e::END)));
        result = true;
        return root;
    }catch(std::runtime_error& err){
        std::cerr << err.what();
    }

    result = false;
    return nullptr;
}