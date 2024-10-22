#include "ast.h"
#include "lexer.h"
#include "code_generator.h"

extern std::unique_ptr<symbol_table> global_sym_table;

ast_node::ast_node(ast_node_type t, int val,
     const std::shared_ptr<ast_node>& left, const std::shared_ptr<ast_node>& right) :
      _val(val), _left(left), _right(right), _type(t){
        std::cout << "ast_node copied\n";
}

ast_node::ast_node(ast_node_type t, int val,
     std::shared_ptr<ast_node>&& left, std::shared_ptr<ast_node>&& right):
      _val(val), _left(std::move(left)), _right(std::move(right)), _type(t){
        std::cout << "ast_node moved\n";
}

ast_node::~ast_node(){
    std::cout << "deleted node\n";
    if(_left)
        _left.reset();
    if(_right)
        _right.reset();
}

expression::expression(ast_node_type t, int val,
     const std::shared_ptr<ast_node>& left, const std::shared_ptr<ast_node>& right) :
      ast_node(t,val,left,right){}

expression::expression(ast_node_type t, int val,
     std::shared_ptr<ast_node>&& left, std::shared_ptr<ast_node>&& right):
    ast_node(t,val,std::move(left),std::move(right)){}

expression::expression(const expression& expr) :
expression(expr._type, expr._val, expr._left, expr._right){}
expression::expression(expression&& expr):
expression(expr._type, expr._val, std::move(expr._left), std::move(expr._right)){}
expression::~expression(){}
expression& expression::operator=(const expression& expr){
    this->_val = expr._val;
    this->_type = expr._type;
    this->_left = expr._left;
    this->_right = expr._right;
    return *this;
}

number_expression::number_expression() : expression(ast_node_type::NUM, 0, nullptr, nullptr){}
number_expression::number_expression(int num) : expression(ast_node_type::NUM, num, nullptr, nullptr){}

void identifier_expression::check_for_validity() const{
    if(!global_sym_table->has_identifier(_val)){
        throw std::runtime_error("invalid identifier expression\n");
    }
}
identifier_expression::identifier_expression() : expression(ast_node_type::ID, -1, nullptr, nullptr){}
identifier_expression::identifier_expression(int id): expression(ast_node_type::ID, id, nullptr, nullptr){
    check_for_validity();
}
void identifier_expression::set_id(int id){
    _val = id;
    check_for_validity();
}
int identifier_expression::get_id() const{
    check_for_validity();
    return _val;
}

/*
assignment_expression::assignment_expression(){}
assignment_expression::assignment_expression(const std::shared_ptr<expression>& lvalue, const std::shared_ptr<expression>& rvalue) : 
expression(ast_node_type::{}
void assignment_expression::set_lvalue(const std::shared_ptr<expression>& expr){

}
void assignment_expression::set_rvalue(const std::shared_ptr<expression>& expr){

}
*/
ast_node_type binary_expression::convert_operation(operator_type op) const{
    switch (op){
        case operator_type::ADD: return ast_node_type::ADD;
        case operator_type::SUB: return ast_node_type::SUB;
        case operator_type::MUL: return ast_node_type::MUL;
        case operator_type::DIV: return ast_node_type::DIV;
        default:
            throw std::runtime_error("undefined binary operation\n");
    }
}
binary_expression::binary_expression(operator_type op,
 const std::shared_ptr<expression>& left,
  const std::shared_ptr<expression>& right) : expression(convert_operation(op), 0, left, right){}

binary_expression::binary_expression(const binary_expression& expr) : 
expression(expr._type, expr._val, expr._left, expr._right){}

binary_expression::binary_expression(binary_expression&& expr) :
expression(expr._type, expr._val, std::move(expr._left), std::move(expr._right)){}

void binary_expression::set_left(const std::shared_ptr<expression>& expr){
    if(!expr)
        throw std::runtime_error("expression node == nullptr\n");
    _left = expr;
}
void binary_expression::set_right(const std::shared_ptr<expression>& expr){
    if(!expr)
        throw std::runtime_error("expression node == nullptr\n");
    _right = expr;
}

void statement::check_validity() const{
    switch(_type){
        case ast_node_type::PRINT:
        case ast_node_type::VAR_DECL:
            break;
        default:
            throw std::runtime_error("undefined statement type\n");
    }
}

statement::statement(ast_node_type t, int val,
 const std::shared_ptr<ast_node>& left, const std::shared_ptr<ast_node>& right) :
    ast_node(t, val, left, right){
    check_validity();
}

statement::statement(ast_node_type t, int val,
     std::shared_ptr<ast_node>&& left,
      std::shared_ptr<ast_node>&& right) :
    ast_node(t, val, std::move(left), std::move(right)){
    check_validity();
}

void statement::set_next(const std::shared_ptr<statement>& next){
    if(!next)
        throw std::runtime_error("next statement node == nullptr\n");
    _right = next;
}

print_statement::print_statement(const std::shared_ptr<expression>& expr) :
 statement(ast_node_type::PRINT,0, expr, nullptr){}

print_statement::print_statement(std::shared_ptr<expression>&& expr) :
 statement(ast_node_type::PRINT,0, std::move(expr), nullptr){}

print_statement::print_statement(const print_statement& stat) : 
print_statement(stat.get_expression()) {}

print_statement::print_statement(print_statement&& stat):
print_statement(std::move(stat.get_expression())){}


void print_statement::set_expression(const std::shared_ptr<expression>& expr){
    if(!expr)
        throw std::runtime_error("next statement node == nullptr\n");
    _left = expr;
}
print_statement& print_statement::operator=(const print_statement& stat){
    this->_left = stat._left;
    this->_right = stat._right;

    return *this;
}

int number_expression::accept_visitor(code_generator& visitor) const{
    return visitor.node_interaction(this);
}
int identifier_expression::accept_visitor(code_generator& visitor) const{
    return visitor.node_interaction(this);
}
int binary_expression::accept_visitor(code_generator& visitor) const{
    return visitor.node_interaction(this);
}
int print_statement::accept_visitor(code_generator& visitor) const{
    visitor.node_interaction(this);
    return 0;
}