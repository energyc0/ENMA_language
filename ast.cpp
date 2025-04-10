#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "code_generator.h"

extern std::unique_ptr<symbol_table> global_sym_table;

ast_node::ast_node(ast_node_type t, int val,
     const std::shared_ptr<ast_node>& left, const std::shared_ptr<ast_node>& right) :
      _val(val), _left(left), _right(right), _type(t){}

ast_node::ast_node(ast_node_type t, int val,
     std::shared_ptr<ast_node>&& left, std::shared_ptr<ast_node>&& right):
      _val(val), _left(std::move(left)), _right(std::move(right)), _type(t){}

ast_node::~ast_node(){
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
ast_node_type binary_expression::convert_operation(arithmetical_operation op) const{
    switch (op){
        case arithmetical_operation::ADD: return ast_node_type::ADD;
        case arithmetical_operation::SUB: return ast_node_type::SUB;
        case arithmetical_operation::MUL: return ast_node_type::MUL;
        case arithmetical_operation::DIV: return ast_node_type::DIV;
        case arithmetical_operation::EQUAL: return ast_node_type::EQUAL;
        case arithmetical_operation::NEQUAL: return ast_node_type::NEQUAl;
        case arithmetical_operation::LESS: return ast_node_type::LESS;
        case arithmetical_operation::GREATER: return ast_node_type::GREATER;
        case arithmetical_operation::LESS_EQ: return ast_node_type::LESS_EQ;
        case arithmetical_operation::GREATER_EQ: return ast_node_type::GREATER_EQ;
        default:
            throw std::runtime_error("undefined binary operation\n");
    }
}
binary_expression::binary_expression(arithmetical_operation op,
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

binary_expression& binary_expression::operator=(const binary_expression& expr){
    this->_type = expr._type;
    this->_left = expr._left;
    this->_right = expr._right;

    return *this;
}

void statement::check_validity() const{
    switch(_type){
        case ast_node_type::PRINT:
        case ast_node_type::VAR_DECL:
        case ast_node_type::ASSIGN:
        case ast_node_type::COMPOUND:
        case ast_node_type::IF_HEAD:
        case ast_node_type::WHILE_LOOP:
        case ast_node_type::FOR_LOOP:
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

print_statement::print_statement(const std::shared_ptr<expression>& expr) :
 statement(ast_node_type::PRINT,0, expr, nullptr){}

print_statement::print_statement(std::shared_ptr<expression>&& expr) :
 statement(ast_node_type::PRINT,0, std::move(expr), nullptr){}

print_statement::print_statement(const print_statement& stat) : 
print_statement(stat.get_expression()) {}

print_statement::print_statement(print_statement&& stat):
print_statement(std::move(stat.get_expression())){}

if_statement::if_statement(const std::shared_ptr<expression>& cond,
     const std::shared_ptr<statement>& next,
     const std::shared_ptr<compound_statement>& if_inner_stat,
     const std::shared_ptr<compound_statement>& else_inner_stat) : 
     statement(ast_node_type::IF_HEAD,0,cond, next),
      _if_stat(if_inner_stat),
       _else_stat(else_inner_stat)  {}

if_statement::if_statement(std::shared_ptr<expression>&& cond,
      std::shared_ptr<statement>&& next,
      std::shared_ptr<compound_statement>&& if_inner_stat,
      std::shared_ptr<compound_statement>&& else_inner_stat) : 
     statement(ast_node_type::IF_HEAD,0,std::move(cond), std::move(next)),
      _if_stat(std::move(if_inner_stat)),
       _else_stat(std::move(else_inner_stat))  {}

if_statement::if_statement(const if_statement& stat) : 
     statement(ast_node_type::IF_HEAD,0,stat._left, stat._right),
      _if_stat(stat._if_stat), _else_stat(stat._else_stat)  {}

if_statement::if_statement(if_statement&& stat) : 
     statement(ast_node_type::IF_HEAD,0,std::move(stat._left), std::move(stat._right)),
      _if_stat(std::move(stat._if_stat)), _else_stat(std::move(stat._else_stat))  {}

compound_statement::compound_statement(const std::shared_ptr<statement>& inner_stat,
     const std::shared_ptr<statement>& next) :
      statement(ast_node_type::COMPOUND,0, inner_stat, next){}

compound_statement::compound_statement(std::shared_ptr<statement>&& inner_stat,
     std::shared_ptr<statement>&& next):
      statement(ast_node_type::COMPOUND,0, std::move(inner_stat), std::move(next)){}

compound_statement::compound_statement(const compound_statement& stat):
 statement(ast_node_type::COMPOUND, 0, stat._left, stat._right){}

compound_statement::compound_statement(compound_statement&& stat):
 statement(ast_node_type::COMPOUND, 0, std::move(stat._left), std::move(stat._right)){}

while_statement::while_statement(const std::shared_ptr<expression>& expr,
    const std::shared_ptr<statement>& next,
     const std::shared_ptr<compound_statement>& inner_stat) : 
     statement(ast_node_type::WHILE_LOOP, 0, expr, next), _inner_stat(inner_stat){}

while_statement::while_statement(std::shared_ptr<expression>&& expr,
    std::shared_ptr<statement>&& next,
     std::shared_ptr<compound_statement>&& inner_stat) : 
     statement(ast_node_type::WHILE_LOOP, 0, std::move(expr), std::move(next)),
      _inner_stat(std::move(inner_stat)){}

while_statement::while_statement(const while_statement& stat) : 
     statement(ast_node_type::WHILE_LOOP, 0, stat._left, stat._right), _inner_stat(stat._inner_stat){}

while_statement::while_statement(while_statement&& stat) : 
     statement(ast_node_type::WHILE_LOOP, 0, std::move(stat._left), std::move(stat._right)),
      _inner_stat(std::move(stat._inner_stat)){}

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

statement_with_id::statement_with_id(ast_node_type t) : statement(t), _is_id_set(false){}
statement_with_id::statement_with_id(ast_node_type t, int id_code, const std::shared_ptr<expression>& expr) :
 statement(t,id_code, expr, nullptr), _is_id_set(true){
    if(!global_sym_table->has_identifier(id_code))
        throw std::runtime_error("an identifier with the id code doesn't exist: " + std::to_string(id_code));

 }

statement_with_id::statement_with_id(ast_node_type t, const std::string& id, const std::shared_ptr<expression>& expr) noexcept :
 statement(t,0, expr, nullptr), _is_id_set(true){

 }
 
statement_with_id::statement_with_id(ast_node_type t, int id_code, std::shared_ptr<expression>&& expr) :
 statement(t,id_code, expr, nullptr), _is_id_set(true){
    if(!global_sym_table->has_identifier(id_code))
        throw std::runtime_error("an identifier with the id code doesn't exist: " + std::to_string(id_code));

 }

statement_with_id::statement_with_id(ast_node_type t, const std::string& id, std::shared_ptr<expression>&& expr) noexcept :
 statement(t,0, expr, nullptr), _is_id_set(true){

 }

void statement_with_id::set_identifier(const std::string& id){
    if(!global_sym_table->has_identifier(id)){
        throw std::runtime_error("an identifier doesn't exist: " + id);
    }
    _val = global_sym_table->get_identifier(id);
    _is_id_set = true;
}
std::string statement_with_id::get_identifier() const{
    if(!_is_id_set)
        throw std::runtime_error("undefined behaviour: identifier is not set");
    return global_sym_table->get_identifier(_val);
}
int statement_with_id::get_identifier_code() const{
    if(!_is_id_set)
        throw std::runtime_error("undefined behaviour: identifier is not set");
    return _val;
}
void statement_with_id::set_expression(const std::shared_ptr<expression>& expr){
    if(!expr)
        throw std::runtime_error("expression node == nullptr");
    _left = expr;
}

variable_declaration::variable_declaration() noexcept : statement_with_id(ast_node_type::VAR_DECL){}

variable_declaration::variable_declaration(int id_code, const std::shared_ptr<expression>& expr) 
: statement_with_id(ast_node_type::VAR_DECL, id_code, expr){}
variable_declaration::variable_declaration(const std::string& id, const std::shared_ptr<expression>& expr) noexcept :
statement_with_id(ast_node_type::VAR_DECL, global_sym_table->try_set_identifier(id), expr){}

assignment_statement::assignment_statement() noexcept : statement_with_id(ast_node_type::ASSIGN){}
//throw an std::runtime_error exception if an identifier doesn't exist
assignment_statement::assignment_statement(int id_code, const std::shared_ptr<expression>& expr):
 statement_with_id(ast_node_type::ASSIGN, id_code, expr){}
//throw an std::runtime_error exception if an identifier doesn't exist
assignment_statement::assignment_statement(const std::string& id, const std::shared_ptr<expression>& expr):
 statement_with_id(ast_node_type::ASSIGN){
    if(!global_sym_table->has_identifier(id))
        throw std::runtime_error("an identifier doesn't exist: " + id);
    _left = expr;
    _val = global_sym_table->try_set_identifier(id);
    _is_id_set = true;
}

void for_statement::check_validity() const{
    if(!_left)
        return;
    if(_left->get_type() != ast_node_type::ASSIGN && _left->get_type() != ast_node_type::VAR_DECL){
        throw std::runtime_error("for-loop start statement must be assignment or variable declaration");
    }
}

for_statement::for_statement(const std::shared_ptr<statement_with_id>& start_stat,
const std::shared_ptr<statement>& next,
const std::shared_ptr<expression>& final_expr,
const std::shared_ptr<expression>& stat_after_iter,
const std::shared_ptr<compound_statement>& inner_stat) : statement(ast_node_type::FOR_LOOP, 0, start_stat, next),
_final_expr(final_expr), _expr_after_iter(stat_after_iter), _inner_stat(inner_stat){
    check_validity();
}

for_statement::for_statement(std::shared_ptr<statement_with_id>&& start_stat,
std::shared_ptr<statement>&& next,
std::shared_ptr<expression>&& final_expr,
std::shared_ptr<expression>&& stat_after_iter,
std::shared_ptr<compound_statement>&& inner_stat): statement(ast_node_type::FOR_LOOP, 0, std::move(start_stat),std::move(next)),
_final_expr(std::move(final_expr)), _expr_after_iter(std::move(stat_after_iter)), _inner_stat(std::move(inner_stat)){
    check_validity();
}

for_statement::for_statement(const for_statement& stat) : 
for_statement(std::static_pointer_cast<statement_with_id>(stat._left), std::static_pointer_cast<statement>(stat._right),
 stat._final_expr, stat._expr_after_iter, stat._inner_stat){}

for_statement::for_statement(for_statement&& stat) :
for_statement(std::static_pointer_cast<statement_with_id>(std::move(stat._left)),
 std::static_pointer_cast<statement>(std::move(stat._right)),
 std::move(stat._final_expr), std::move(stat._expr_after_iter), std::move(stat._inner_stat)){}

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
int assignment_statement::accept_visitor(code_generator& visitor) const{
    visitor.node_interaction(this);
    return 0;
}
int variable_declaration::accept_visitor(code_generator& visitor) const{
    visitor.node_interaction(this);
    return 0;
}
int compound_statement::accept_visitor(code_generator& visitor) const{
    visitor.node_interaction(this);
    return 0;
}
int if_statement::accept_visitor(code_generator& visitor) const{
    visitor.node_interaction(this);
    return 0;
}

int while_statement::accept_visitor(code_generator& visitor) const{
    visitor.node_interaction(this);
    return 0;
}

int for_statement::accept_visitor(code_generator& visitor) const{
    visitor.node_interaction(this);
    return 0; 
}
