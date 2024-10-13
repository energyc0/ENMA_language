#include <iostream>
#include "enma_types.h"
#include "parser.h"

parsing_error::parsing_error(const char* msg, int line, int token) noexcept :
     _msg(msg), _line(line), _token(token) {}

std::string parsing_error::what(){
    return "line " + std::to_string(_line) + ", token " + std::to_string(_token) + "\n" + _msg;
}

token_storage::token_storage(std::list<token_t>& tokens) : _tokens(tokens), _iter(tokens.begin()) {
    skip_new_lines();
}

token_t token_storage::get_current() noexcept{
    if(_iter == _tokens.end()){
        return token_t(token_type_e::END, 0);
    }
    return *_iter;
}
token_t token_storage::get_next() noexcept {
    next();
    if(_iter == _tokens.end()){
        return token_t(token_type_e::END, 0);
    }
    return *_iter;
}
void token_storage::next() noexcept{
    if(_iter == _tokens.end()){
        return;
    }
    ++_iter;
    ++_token_number;
    skip_new_lines();
}

void token_storage::skip_new_lines(){
    while(_iter != _tokens.end() && _iter->get_type() == token_type_e::NEW_LINE){
        _token_number = 1;
        _line_number++;
         _iter++;
    }
}

ast_node_t::ast_node_t(): val(0), left(nullptr), right(nullptr), type(ast_node_type_e::END) {}

ast_node_t::ast_node_t(int _val, ast_node_type_e _t, const std::shared_ptr<ast_node_t>& _left, const std::shared_ptr<ast_node_t>& _right) :
 val(_val), left(_left), right(_right), type(_t) {}

ast_node_t::~ast_node_t() {
    if(left)
        left.reset();
    if(right)
        right.reset();
}

int ast_node_t::interpret_node(){
    switch(type){
        case ast_node_type_e::NUM:  return val;
        case ast_node_type_e::ADD:  return left->interpret_node() + right->interpret_node();
        case ast_node_type_e::SUB:  return left->interpret_node() - right->interpret_node();
        case ast_node_type_e::DIV:  return left->interpret_node() / right->interpret_node();
        case ast_node_type_e::MUL:  return left->interpret_node() * right->interpret_node();
        default:
            std::cerr<<"undefined ast_node type\n";
            break;
    }
    return -1;
}

ast_node_type_e parser::reinterpret_arith_op(const token_t& t){
    if(t.get_type() == token_type_e::END){
        return ast_node_type_e::END;
    }else if(t.get_type() != token_type_e::OPERATOR){
        throw parsing_error("syntax error\noperator expected\n",
        _tokens->get_line_number(),
        _tokens->get_token_number());
    }
    switch (static_cast<operator_type_e>(t.get_value())){
        case operator_type_e::ADD: return ast_node_type_e::ADD;
        case operator_type_e::SUB: return ast_node_type_e::SUB;
        case operator_type_e::DIV: return ast_node_type_e::DIV;
        case operator_type_e::MUL: return ast_node_type_e::MUL;
            break;
        default:
            throw parsing_error("syntax error\nundefined operator\n",
             _tokens->get_line_number(),
            _tokens->get_token_number());
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
            throw parsing_error("syntax error\nexpected arithmetic operation\n",
            _tokens->get_line_number(),
            _tokens->get_token_number());
            break;
    }
    return -1;
}

std::shared_ptr<ast_node_t> parser::get_primary_expr(){
    auto t = _tokens->get_current();
    switch(t.get_type()){
        case token_type_e::CONSTANT:
            return std::shared_ptr<ast_node_t>(new ast_node_t(t.get_value(), ast_node_type_e::NUM, nullptr, nullptr));
        case token_type_e::END:
            return std::shared_ptr<ast_node_t>(new ast_node_t(t.get_value(), ast_node_type_e::END, nullptr, nullptr));
        default:
            throw parsing_error("syntax error\nprimary expression expected\n",
            _tokens->get_line_number(),
            _tokens->get_token_number());
        break;
    }
    return nullptr;
}

std::shared_ptr<ast_node_t> parser::bin_expr_parse(int prev_op_precedence){
    auto left = get_primary_expr();
    if(left->type == ast_node_type_e::END)
        return left;

    std::shared_ptr<ast_node_t> right = nullptr;
    auto token = _tokens->get_next();
    while(get_arith_op_precedence(reinterpret_arith_op(token)) > prev_op_precedence){
        _tokens->next();
        right = bin_expr_parse(get_arith_op_precedence(reinterpret_arith_op(token)));

        left = std::shared_ptr<ast_node_t>(new ast_node_t(0, reinterpret_arith_op(token), left, right));
        token = _tokens->get_current();
        if(token.get_type() == token_type_e::END){
            break;
        }
    }
    return left;
}

std::shared_ptr<ast_node_t> parser::binary_expr(token_storage& tokens, bool& result){
    try{
        _tokens = &tokens;
        auto root = bin_expr_parse(parser::get_arith_op_precedence(ast_node_type_e::END));
        result = true;
        return root;
    }catch(parsing_error& err){
        std::cerr << err.what();
    }

    result = false;
    return nullptr;
}