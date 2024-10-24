#include <iostream>
#include "parser.h"
#include "token_types.h"
#include "ast.h"

parsing_error::parsing_error(const char* msg, const token_storage& storage) noexcept :
     _msg(msg), _line(storage.get_line_number()), _token(storage.get_token_number()) {}

std::string parsing_error::what(){
    return "line " + std::to_string(_line) + ", token " + std::to_string(_token) + "\n" + _msg;
}

token_storage::token_storage(std::list<std::shared_ptr<token>>& tokens) : _tokens(tokens), _iter(tokens.begin()) {
    skip_new_lines();
}

std::shared_ptr<token> token_storage::get_current() noexcept{
    if(_iter == _tokens.end()){
        return std::make_shared<token_end>();
    }
    return *_iter;
}
std::shared_ptr<token> token_storage::get_next() noexcept {
    next();
    if(_iter == _tokens.end()){
        return std::make_shared<token_end>();
    }
    return *_iter;
}
void token_storage::next() noexcept{
    if(_iter == _tokens.end()){
        return;
    }
    ++_iter;
    _prev_token = _token_number++;
    skip_new_lines();
}

void token_storage::skip_new_lines(){
    _prev_line = _line_number;
    while(_iter != _tokens.end() && _iter->get()->get_type() == token_type::NEW_LINE){
        _token_number = 1;
        _line_number++;
         _iter++;
    }
}

arithmetical_operation parser::reinterpret_arith_op(const token& t){
    if(t.get_type() == token_type::PUNCTUATION &&
     static_cast<const token_punctuation&>(t).get_punctuation() == punctuation_type::SEMICOLON){
        return arithmetical_operation::END_EXPR;
    }else if(t.get_type() != token_type::OPERATOR){
        throw parsing_error("syntax error\nexpected arithmetical operation\n", *_tokens);
    }
    switch (static_cast<const token_operator&>(t).get_operator()){
        case operator_type::ADD: return arithmetical_operation::ADD;
        case operator_type::SUB: return arithmetical_operation::SUB;
        case operator_type::MUL: return arithmetical_operation::MUL;
        case operator_type::DIV: return arithmetical_operation::DIV;
        default:
            throw parsing_error("syntax error\nexpected arithmetical operation\n", *_tokens);
    }
}

int parser::get_arith_op_precedence(arithmetical_operation op){
    switch (op){
        case arithmetical_operation::DIV:
        case arithmetical_operation::MUL:
            return 100;
        case arithmetical_operation::ADD:
        case arithmetical_operation::SUB:
            return 10;
        case arithmetical_operation::END_EXPR:
            return 0;
        default:
            throw parsing_error("syntax error\nexpected arithmetical operation\n", *_tokens);
            break;
    }
    return -1;
}

std::shared_ptr<expression> parser::get_primary_expr(){
    auto t = _tokens->get_current();
    switch(t->get_type()){
        case token_type::CONSTANT:
            return std::make_shared<number_expression>(static_cast<token_constant&>(*t).get_value());
        case token_type::IDENTIFIER:{
            auto t_id = static_cast<token_identifier&>(*t);
            if(_declared_identifiers.find(t_id.get_identifier_code()) == _declared_identifiers.end()){
                throw parsing_error("syntax error\nundeclared identifier\n", *_tokens);
            }
            return std::make_shared<identifier_expression>(static_cast<token_identifier&>(*t).get_identifier_code());
        }
        case token_type::END:
        case token_type::PUNCTUATION:
            return nullptr;
        default:
            throw parsing_error("syntax error\nprimary expression expected\n", *_tokens);
        break;
    }
    return nullptr;
}

std::shared_ptr<expression> parser::bin_expr_parse(int prev_op_precedence){
    auto left = get_primary_expr();
    if(!left)
        return left;

    std::shared_ptr<expression> right = nullptr;
    auto token = _tokens->get_next();
    while(get_arith_op_precedence(reinterpret_arith_op(*token)) > prev_op_precedence){
        _tokens->next();
        right = bin_expr_parse(get_arith_op_precedence(reinterpret_arith_op(*token)));

        left = std::static_pointer_cast<expression>(std::make_shared<binary_expression>(reinterpret_arith_op(*token), left, right));
        token = _tokens->get_current();
        if(token->get_type() == token_type::PUNCTUATION &&
         static_cast<token_punctuation&>(*token).get_punctuation() ==punctuation_type::SEMICOLON){
            break;
        }
    }
    return left;
}

std::shared_ptr<statement> parser::generate_ast(token_storage& tokens, bool& result){
    try{
        _tokens = &tokens;
        
        auto root = expect_statement();

        result = true;
        return root;
    }catch(parsing_error& err){
        std::cerr << err.what();
    }

    result = false;
    return nullptr;
}

std::shared_ptr<statement> parser::expect_statement(){
    auto t = _tokens->get_current();
    if(is_match(t, token_type::END))
        return nullptr;

    std::shared_ptr<statement> node = nullptr;
    switch (t->get_type()){
        case token_type::END: return nullptr;
        case token_type::KEYWORD:{
            auto keyword_token = std::static_pointer_cast<token_keyword>(t);
            switch (keyword_token->get_keyword()){
                case keyword_type::PRINT: node =  parse_print(); break;
                case keyword_type::LET: node =  parse_variable_declaration(); break;
                default:
                    throw parsing_error("unexpected keyword\n", *_tokens);
            }
            break;
        }
        case token_type::IDENTIFIER: node = parse_assignment_statement(); break;
        default:
            throw parsing_error("unexpected token\n", *_tokens);
    }
    node->set_next(expect_statement());
    return node;
}

std::shared_ptr<class assignment_statement> parser::parse_assignment_statement(){
    auto t = _tokens->get_current();
    if(!is_match(t, token_type::IDENTIFIER)){
        throw parsing_error("identifier expected\n", *_tokens);
    }
    auto t_id = std::static_pointer_cast<token_identifier>(t);
    if(_declared_identifiers.find(t_id->get_identifier_code()) == _declared_identifiers.end()){
        throw parsing_error("undeclared identifier\n", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,operator_type::ASSIGN)){
        throw parsing_error("operator '=' expected\n", *_tokens);
    }
    _tokens->next();
    auto expr = binary_expr();
    if(!expr){
        throw parsing_error("expression expected\n", *_tokens);
    }
    t = _tokens->get_current();
    if(!is_match(t,punctuation_type::SEMICOLON)){
        throw parsing_error("semicolon expected\n", *_tokens);
    }
    _tokens->next();

    return std::make_shared<assignment_statement>(t_id->get_identifier_code(), expr);
}

std::shared_ptr<variable_declaration> parser::parse_variable_declaration(){
    auto t = _tokens->get_current();
    if(!is_match(t, keyword_type::LET)){
        throw parsing_error("'let' keyword expected\n", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t, token_type::IDENTIFIER)){
        throw parsing_error("identifier expected\n", *_tokens);
    }

    auto id_token = std::static_pointer_cast<token_identifier>(t);
    if(_declared_identifiers.find(id_token->get_identifier_code()) != _declared_identifiers.end()){
        throw parsing_error("identifier has already been declared\n", *_tokens);
    }
    _declared_identifiers.emplace(id_token->get_identifier_code());

    t = _tokens->get_next();
    if(!is_match(t,operator_type::ASSIGN)){
        throw parsing_error("operator '=' expected\n", *_tokens);
    }
    _tokens->next();

    auto expr = binary_expr();
    t = _tokens->get_current();
    if(!expr){
        throw parsing_error("expression expected\n", *_tokens);
    }


    if(!is_match(t,punctuation_type::SEMICOLON)){
        throw parsing_error("semicolon expected\n", *_tokens);
    }
    _tokens->next();
    return std::make_shared<variable_declaration>(id_token->get_identifier_code(), expr);
}

std::shared_ptr<print_statement> parser::parse_print(){
    auto t = _tokens->get_current();
    if(!is_match(t, keyword_type::PRINT)){
        throw parsing_error("print keyword expected\n", *_tokens);
    }
    //token = _tokens->get_next();
    //if(token.get_type() != token_type::OPERATOR || 
    //token.get_value() != static_cast<int>(operator_type::LPAR)){
    //    throw parsing_error("syntax error\nleft parenthesis expected\n", *_tokens);
    //}
    _tokens->next();

    auto expr = binary_expr();
    t = _tokens->get_current();

    if(!expr){
        throw parsing_error("expression expected\n", *_tokens);
    }
    //if(token.get_type() != token_type::OPERATOR || 
    //token.get_value() != static_cast<int>(operator_type::RPAR)){
    //    throw parsing_error("syntax error\nright parenthesis expected\n", *_tokens);
    //}
    if(!is_match(t, punctuation_type::SEMICOLON)){
        throw parsing_error("syntax error\nsemicolon expected\n", *_tokens);
    }
    _tokens->next();
    return std::make_shared<print_statement>(expr);
}

std::shared_ptr<expression> parser::binary_expr(){
    return bin_expr_parse(parser::get_arith_op_precedence(arithmetical_operation::END_EXPR));
}