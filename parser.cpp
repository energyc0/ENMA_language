#include <iostream>
#include "parser.h"
#include "token_types.h"
#include "ast.h"

parsing_error::parsing_error(const char* msg, const token_storage& storage) noexcept :
     _msg(msg), _line(storage.get_line_number()), _token(storage.get_token_number()) {}

std::string parsing_error::what(){
    return "line " + std::to_string(_line) + ", token " + std::to_string(_token) + "\n" +
    "syntax error\n" +  _msg + "\n";
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
    if(_iter != _tokens.end()){
         ++_iter;
        _prev_token = _token_number++;
        skip_new_lines();
    }
}

std::shared_ptr<token> token_storage::check_next() noexcept{
    auto temp = _iter;
    std::shared_ptr<token> res = get_next();
    std::swap(temp,_iter);
    return res;
}

void token_storage::skip_new_lines(){
    _prev_line = _line_number;
    while(_iter != _tokens.end() && _iter->get()->get_type() == token_type::NEW_LINE){
        _token_number = 1;
        _line_number++;
         _iter++;
    }
}

bool parser::is_end_binary_expression_token(const std::shared_ptr<token>& t) const noexcept{
    return  is_match(t,operator_type::RPAR) || 
    is_match(t,punctuation_type::SEMICOLON) ||
    is_match(t, punctuation_type::RBRACE) ||
    is_match(t, keyword_type::TO) || 
    is_match(t,punctuation_type::COLON);
}

arithmetical_operation parser::reinterpret_arith_op(const std::shared_ptr<token>& t){
    if(is_end_binary_expression_token(t)){
        return arithmetical_operation::END_EXPR;
    }else if(!is_match(t, token_type::OPERATOR)){
        throw parsing_error("expected arithmetical operation", *_tokens);
    }
    switch (std::static_pointer_cast<token_operator>(t)->get_operator()){
        case operator_type::ADD: return arithmetical_operation::ADD;
        case operator_type::SUB: return arithmetical_operation::SUB;
        case operator_type::MUL: return arithmetical_operation::MUL;
        case operator_type::DIV: return arithmetical_operation::DIV;
        case operator_type::EQUAL: return arithmetical_operation::EQUAL;
        case operator_type::NEQUAL: return arithmetical_operation::NEQUAL;
        case operator_type::GREATER: return arithmetical_operation::GREATER;
        case operator_type::GREATER_EQUAL: return arithmetical_operation::GREATER_EQ;
        case operator_type::LESS: return arithmetical_operation::LESS;
        case operator_type::LESS_EQUAl: return arithmetical_operation::LESS_EQ;
        default:
            throw parsing_error("expected arithmetical operation", *_tokens);
    }
}

int parser::get_arith_op_precedence(arithmetical_operation op){
    switch (op){
        case arithmetical_operation::DIV:
        case arithmetical_operation::MUL:
            return 100;
        case arithmetical_operation::ADD:
        case arithmetical_operation::SUB:
            return 50;
        case arithmetical_operation::EQUAL:
        case arithmetical_operation::NEQUAL:
        case arithmetical_operation::GREATER:
        case arithmetical_operation::GREATER_EQ:
        case arithmetical_operation::LESS:
        case arithmetical_operation::LESS_EQ:
            return 10;
        case arithmetical_operation::END_EXPR:
            return 0;
        default:
            break;
    }
    throw parsing_error("expected arithmetical operation", *_tokens);
}

std::shared_ptr<expression> parser::get_primary_expr(){
    auto t = _tokens->get_current();
    switch(t->get_type()){
        case token_type::CONSTANT:
            return std::make_shared<number_expression>(static_cast<token_constant&>(*t).get_value());
        case token_type::IDENTIFIER:{
            auto t_id = static_cast<token_identifier&>(*t);
            if(_declared_identifiers.find(t_id.get_identifier_code()) == _declared_identifiers.end()){
                throw parsing_error("undeclared identifier", *_tokens);
            }
            return std::make_shared<identifier_expression>(static_cast<token_identifier&>(*t).get_identifier_code());
        }
        case token_type::END:
        case token_type::PUNCTUATION:
            return nullptr;
        case token_type::OPERATOR:{
            if(is_match(t, operator_type::LPAR)){
                _tokens->next();
                return bin_expr(get_arith_op_precedence(arithmetical_operation::END_EXPR));
            }
        }
        default:
            throw parsing_error("expression expected", *_tokens);
        break;
    }
    return nullptr;
}

std::shared_ptr<expression> parser::bin_expr(int prev_op_precedence){
    auto left = get_primary_expr();
    if(!left)
        return left;

    std::shared_ptr<expression> right = nullptr;
    auto t = _tokens->get_next();
    while(get_arith_op_precedence(reinterpret_arith_op(t)) > prev_op_precedence){
        _tokens->next();
        right = bin_expr(get_arith_op_precedence(reinterpret_arith_op(t)));

        left = std::static_pointer_cast<expression>(std::make_shared<binary_expression>(reinterpret_arith_op(t), left, right));
        t = _tokens->get_current();
        if(is_end_binary_expression_token(t)) {
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

std::shared_ptr<statement> parser::parse_statement(const std::shared_ptr<token>& t){
    switch (t->get_type()){
        case token_type::KEYWORD:{
            auto keyword_token = std::static_pointer_cast<token_keyword>(t);
            switch (keyword_token->get_keyword()){
                case keyword_type::PRINT: return parse_print();
                case keyword_type::LET: return parse_variable_declaration();
                case keyword_type::IF: return parse_if_statement();
                case keyword_type::WHILE: return parse_while_statement();
                case keyword_type::FOR: return parse_for_statement();
                default:
                    throw parsing_error("unexpected keyword", *_tokens);
            }
            break;
        }
        case token_type::IDENTIFIER: return parse_assignment_statement();
        default:
            break;
    }
    throw parsing_error("unexpected token", *_tokens);
}

std::shared_ptr<compound_statement> parser::expect_compound_statement(){
    auto t = _tokens->get_current();
    if(!is_match(t,punctuation_type::LBRACE)){
        throw parsing_error("expected left brace", *_tokens);
    }

    auto root = std::make_shared<compound_statement>();
    t = _tokens->get_next();
    //empty compound statement
    if(is_match(t, punctuation_type::RBRACE)){
        return root;
    }

    root->set_inner_statement(parse_statement(t));
    auto node = root->get_inner_statement();

    t = _tokens->get_current();
    while(true){
        while(is_match(t, punctuation_type::SEMICOLON)){
            t = _tokens->get_next();
        }
        if(is_match(t,punctuation_type::RBRACE)){
            break;
        }
        if(is_match(t,token_type::END)){
            throw parsing_error("unclosed right brace", *_tokens);
        }

        node->set_next(parse_statement(t));
        node = node->get_next();
        t = _tokens->get_current();
    }
    //skip a right brace
    _tokens->next();

    return root;
}

std::shared_ptr<statement> parser::expect_statement(){
    auto t = _tokens->get_current();

    while(is_match(t, punctuation_type::SEMICOLON)){
        t = _tokens->get_next();
    }
    if(is_match(t, token_type::END)){
        return nullptr;
    }
    std::shared_ptr<statement> node = parse_statement(t);
    if(node){
        node->set_next(expect_statement());
    }
    return node;
}

std::shared_ptr<if_statement> parser::parse_if_statement(){
    auto t = _tokens->get_current();
    if(!is_match(t,keyword_type::IF)){
        throw parsing_error("'if' keyword expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,punctuation_type::ARROW)){
        throw parsing_error("'=>' expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,operator_type::LPAR)){
        throw parsing_error("left parenthesis expected", *_tokens);
    }
    _tokens->next();

    auto cond_expr = parse_binary_expression();
    
    t = _tokens->get_current();
    if(!is_match(t,operator_type::RPAR)){
        throw parsing_error("right parenthesis expected", *_tokens);
    }

    _tokens->next();
    auto inner_if_head_stat = expect_compound_statement();

    t = _tokens->get_current();
    if(!is_match(t,keyword_type::ELSE)){
        return std::make_shared<if_statement>(cond_expr, nullptr, inner_if_head_stat,nullptr);
    }

    _tokens->next();
    auto inner_else_stat = expect_compound_statement();
    return std::make_shared<if_statement>(cond_expr, nullptr, inner_if_head_stat,inner_else_stat);
}

std::shared_ptr<for_statement> parser::parse_for_statement(){
    auto t = _tokens->get_current();
    if(!is_match(t,keyword_type::FOR)){
        throw parsing_error("'for' keyword expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,punctuation_type::ARROW)){
        throw parsing_error("'=>' expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,operator_type::LPAR)){
        throw parsing_error("left parenthesis expected", *_tokens);
    }

    t = _tokens->get_next();
    std::shared_ptr<statement_with_id> start_statement = nullptr;
    if(is_match(t,token_type::IDENTIFIER)){
        auto temp = _tokens->check_next();
        if(is_match(temp,keyword_type::TO)){
            _tokens->next();
            auto token_id = std::static_pointer_cast<token_identifier>(t);
            start_statement = std::make_shared<assignment_statement>(token_id->get_identifier_code());
        }else{
            start_statement = parse_assignment_statement(false);
        }
    }else if(is_match(t,keyword_type::LET)){
        start_statement = parse_variable_declaration(false);
    }else{
        throw parsing_error("assignment or variable declaration statement expected", *_tokens);
    }

    t = _tokens->get_current();
    if(!is_match(t,keyword_type::TO)){
        throw parsing_error("'to' keyword expected", *_tokens);
    }
    _tokens->next();
    auto final_expr = parse_binary_expression();


    std::shared_ptr<expression> expr_after_iter = nullptr;
    t = _tokens->get_current();
    if(is_match(t,punctuation_type::COLON)){
        _tokens->next();
        expr_after_iter = parse_binary_expression();
        t = _tokens->get_current();
    }
    if(is_match(t,operator_type::RPAR)){
        if(!expr_after_iter)
            expr_after_iter = std::make_shared<number_expression>(1);
    }else{
        throw parsing_error("right parenthesis expected", *_tokens);
    }
    _tokens->next();
    auto inner_statements = expect_compound_statement();

    return std::make_shared<for_statement>(std::move(start_statement), nullptr,
     std::move(final_expr), std::move(expr_after_iter), std::move(inner_statements));
}

std::shared_ptr<while_statement> parser::parse_while_statement(){
    auto t = _tokens->get_current();
    if(!is_match(t,keyword_type::WHILE)){
        throw parsing_error("'while' keyword expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,punctuation_type::ARROW)){
        throw parsing_error("'=>' expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,operator_type::LPAR)){
        throw parsing_error("left parenthesis expected", *_tokens);
    }
    
    _tokens->next();
    auto conditional_expr = parse_binary_expression();

    t = _tokens->get_current();
    if(!is_match(t,operator_type::RPAR)){
        throw parsing_error("right parenthesis expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t,punctuation_type::LBRACE)){
        throw parsing_error("left brace expected", *_tokens);
    }

    auto inner_statements = expect_compound_statement();

    return std::make_shared<while_statement>(conditional_expr, nullptr, inner_statements);
}

std::shared_ptr<class assignment_statement> parser::parse_assignment_statement(bool expect_semicolon){
    auto t = _tokens->get_current();
    if(!is_match(t, token_type::IDENTIFIER)){
        throw parsing_error("identifier expected", *_tokens);
    }
    auto t_id = std::static_pointer_cast<token_identifier>(t);
    if(_declared_identifiers.find(t_id->get_identifier_code()) == _declared_identifiers.end()){
        throw parsing_error("undeclared identifier", *_tokens);
    }

    t = _tokens->get_next();
    if(is_match(t, punctuation_type::SEMICOLON)){
        return std::make_shared<assignment_statement>(t_id->get_identifier_code());
    }

    if(!is_match(t,operator_type::ASSIGN)){
        throw parsing_error("operator '=' expected", *_tokens);
    }
    _tokens->next();
    auto expr = parse_binary_expression();
    if(!expr){
        throw parsing_error("expression expected", *_tokens);
    }

    if(expect_semicolon){
        t = _tokens->get_current();
        if(!is_match(t,punctuation_type::SEMICOLON)){
            throw parsing_error("semicolon expected", *_tokens);
        }
        _tokens->next();
    }

    return std::make_shared<assignment_statement>(t_id->get_identifier_code(), expr);
}

std::shared_ptr<variable_declaration> parser::parse_variable_declaration(bool expect_semicolon){
    auto t = _tokens->get_current();
    if(!is_match(t, keyword_type::LET)){
        throw parsing_error("'let' keyword expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t, token_type::IDENTIFIER)){
        throw parsing_error("identifier expected", *_tokens);
    }

    auto id_token = std::static_pointer_cast<token_identifier>(t);
    if(_declared_identifiers.find(id_token->get_identifier_code()) != _declared_identifiers.end()){
        throw parsing_error("identifier has already been declared", *_tokens);
    }
    _declared_identifiers.emplace(id_token->get_identifier_code());

    t = _tokens->get_next();
    if(!is_match(t,operator_type::ASSIGN)){
        throw parsing_error("operator '=' expected", *_tokens);
    }
    _tokens->next();

    auto expr = parse_binary_expression();
    if(!expr){
        throw parsing_error("expression expected", *_tokens);
    }

    if(expect_semicolon){
        t = _tokens->get_current();
        if(!is_match(t,punctuation_type::SEMICOLON)){
            throw parsing_error("semicolon expected", *_tokens);
        }
        _tokens->next();
    }
    return std::make_shared<variable_declaration>(id_token->get_identifier_code(), expr);
}

std::shared_ptr<print_statement> parser::parse_print(){
    auto t = _tokens->get_current();
    if(!is_match(t, keyword_type::PRINT)){
        throw parsing_error("print keyword expected", *_tokens);
    }
    t = _tokens->get_next();
    if(!is_match(t, operator_type::LPAR)){
        throw parsing_error("left parenthesis expected", *_tokens);
    }
    _tokens->next();

    auto expr = parse_binary_expression();
    t = _tokens->get_current();

    if(!expr){
        throw parsing_error("expression expected", *_tokens);
    }

    if(!is_match(t, operator_type::RPAR)){
        throw parsing_error("right parenthesis expected", *_tokens);
    }

    t = _tokens->get_next();
    if(!is_match(t, punctuation_type::SEMICOLON)){
        throw parsing_error("semicolon expected", *_tokens);
    }
    _tokens->next();
    return std::make_shared<print_statement>(expr);
}

std::shared_ptr<expression> parser::parse_binary_expression(){
    return bin_expr(parser::get_arith_op_precedence(arithmetical_operation::END_EXPR));
}