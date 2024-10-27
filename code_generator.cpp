#include <iostream>
#include "code_generator.h"
#include "parser.h"
#include "ast.h"
#include "lexer.h"

extern std::unique_ptr<symbol_table> global_sym_table;

unhandled_register_error::unhandled_register_error(const char* msg, const code_register& reg) 
: _msg("register: " + std::string(reg.get_name()) + "\n" + std::string(msg)) {}

std::string unhandled_register_error::what(){
    return _msg;
}

some_variable::some_variable(const std::string& name) : 
_name("_" + std::to_string(global_sym_table->get_identifier(name)) + "_" + name){
    _id_code = global_sym_table->get_identifier(name);
}
some_variable::some_variable(std::string&& name):
_name("_" + std::to_string(global_sym_table->get_identifier(name)) + "_" + name){
    _id_code = global_sym_table->get_identifier(name);
}

void code_generator::output_preamble(){
    _file   << "section .text\n"
            << "\textern printf\n"
            << "\tglobal main\n"
            << "main:\n"
            << "\tpush rbp\n"
            << "\tmov rbp, rsp\n\n";
}

void code_generator::output_variables(){
    _file  << "section .data\n"
            << "\td_fmt db '%d',10,0\n";

    for(const auto& usr_var : _variables){
        _file << "\t" << usr_var.get_asm_name() << " dq 0\n";
    }
}

void code_generator::output_postamble(){
    _file  << "\n\tpop rbp\n"
            << "\tmov rax, 60\n"
            << "\tmov rdi, 0\n"
            << "\tsyscall\n\n";
    output_variables();
}

int code_generator::find_free_reg(){
    for(int i = 0; i < _registers_count; i++){
        if(!_registers[i].is_busy()){
            return i;
        }
    }
    throw std::runtime_error("failed to find free register");
}

int code_generator::mov_reg_var(int reg, const class identifier_expression* expr){
    _file << "\tmov " << _registers[reg].get_name() << ", [" << _variables[expr->get_id()].get_asm_name() << "]\n";
    _registers[reg].become_busy();
    return reg;
}

int code_generator::mov_reg(int reg, int val){
    _file << "\tmov " << _registers[reg].get_name() << ", " << val << '\n';
    _registers[reg].become_busy();
    return reg;
}

int code_generator::equal_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);
    
    _file   << "\tcmp " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n'
            << "\tsete " << _registers[left].get_name() << "b\n"
            << "\tand " << _registers[left].get_name() << ", 255\n";

    _registers[right].become_free();
    return left;
}    
int code_generator::nequal_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);
    
    _file   << "\tcmp " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n'
            << "\tsetne " << _registers[left].get_name() << "b\n"
            << "\tand " << _registers[left].get_name() << ", 255\n";

    _registers[right].become_free();
    return left;
}
int code_generator::greater_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);
    
    _file   << "\tcmp " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n'
            << "\tsetg " << _registers[left].get_name() << "b\n"
            << "\tand " << _registers[left].get_name() << ", 255\n";

    _registers[right].become_free();
    return left;
}
int code_generator::greater_equal_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);
    
    _file   << "\tcmp " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n'
            << "\tsetge " << _registers[left].get_name() << "b\n"
            << "\tand " << _registers[left].get_name() << ", 255\n";

    _registers[right].become_free();
    return left;
}
int code_generator::less_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);
    
    _file   << "\tcmp " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n'
            << "\tsetl " << _registers[left].get_name() << "b\n"
            << "\tand " << _registers[left].get_name() << ", 255\n";

    _registers[right].become_free();
    return left;
}
int code_generator::less_equal_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);
    
        _file   << "\tcmp " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n'
            << "\tsetle " << _registers[left].get_name() << "b\n"
            << "\tand " << _registers[left].get_name() << ", 255\n";
    
    _registers[right].become_free();
    return left;
}

int code_generator::add_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);

    _file << "\tadd " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n';
    _registers[right].become_free();
    return left;
}

int code_generator::sub_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);

    _file << "\tsub " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n';
    _registers[right].become_free();
    return left;
}

int code_generator::mul_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);

    _file << "\timul " << _registers[left].get_name() << ", " << _registers[right].get_name() << '\n';
    _registers[right].become_free();
    return left;        
}

int code_generator::div_reg(int left, int right){
    check_valid_storage(_registers[left], _registers[right]);

    _file  << "\tmov rax, " << _registers[left].get_name() << "\n"
            << "\txor rdx, rdx\n"
            << "\tdiv " << _registers[right].get_name() << '\n'
            << "\tmov " << _registers[left].get_name() << ", rax\n";
    _registers[right].become_free();
    return left;
}

void code_generator::assign_to_variable(const assignment_statement* stat){
    if(stat->get_identifier_code() >= _variables.size()){
        throw std::runtime_error("undeclared identifier\n");
    }
    int reg = stat->get_expression()->accept_visitor(*this);
    _file << "\tmov [" << _variables[stat->get_identifier_code()].get_asm_name() << "], "  << _registers[reg].get_name() << "\n\n";
    _registers[reg].become_free();
}

void code_generator::declare_variable(const variable_declaration* stat){
    _variables.emplace_back(some_variable(stat->get_identifier()));

    int reg = stat->get_expression()->accept_visitor(*this);
    _file << "\tmov [" << _variables.back().get_asm_name() << "], " << _registers[reg].get_name() << "\n\n";
    _registers[reg].become_free();
}

void code_generator::print_reg(int reg){
    check_valid_storage(_registers[reg]);

    _file  << "\n\tmov rdi, d_fmt\n"
            << "\tmov rsi, " << _registers[reg].get_name() << '\n'
            << "\tcall printf\n\n";
    _registers[reg].become_free();
}

int code_generator::node_interaction(const number_expression* expr){
    return mov_reg(find_free_reg(),expr->get_number());
}
int code_generator::node_interaction(const identifier_expression* expr){
    return mov_reg_var(find_free_reg(), expr);
}
int code_generator::node_interaction(const binary_expression* expr){
    int left_reg = expr->get_left()->accept_visitor(*this);
    int right_reg = expr->get_right()->accept_visitor(*this);

    switch (expr->get_type()){
        case ast_node_type::ADD: return add_reg(left_reg,right_reg);
        case ast_node_type::SUB: return sub_reg(left_reg,right_reg);
        case ast_node_type::DIV: return div_reg(left_reg,right_reg);
        case ast_node_type::MUL: return mul_reg(left_reg,right_reg);
        case ast_node_type::EQUAL: return equal_reg(left_reg,right_reg);
        case ast_node_type::NEQUAl: return nequal_reg(left_reg,right_reg);
        case ast_node_type::GREATER: return greater_reg(left_reg,right_reg);
        case ast_node_type::GREATER_EQ: return greater_equal_reg(left_reg,right_reg);
        case ast_node_type::LESS: return less_reg(left_reg,right_reg);
        case ast_node_type::LESS_EQ: return less_equal_reg(left_reg,right_reg);
        default:
            throw std::runtime_error("undefined binary expression operator\n");
    }
}
void code_generator::node_interaction(const print_statement* stat) {
    print_reg(stat->get_expression()->accept_visitor(*this));
    if(stat->get_next())
        stat->get_next()->accept_visitor(*this);
}
void code_generator::node_interaction(const assignment_statement* stat){
    assign_to_variable(stat);
    if(stat->get_next()){
        stat->get_next()->accept_visitor(*this);
    }
}
void code_generator::node_interaction(const variable_declaration* stat){
    declare_variable(stat);
    if(stat->get_next()){
        stat->get_next()->accept_visitor(*this);
    }
}
void code_generator::node_interaction(const class compound_statement* stat){
    return;
}
void code_generator::node_interaction(const class if_statement* stat){

}

code_generator::code_generator(const std::string& output_filename){
    _file.open(output_filename);
    if(!_file.is_open())
        throw std::runtime_error("failed to open the file: " + output_filename + "\n");
}

code_generator::~code_generator(){
    _file.close();
}

bool code_generator::generate_code(const std::shared_ptr<statement>& root){
    try{
        output_preamble();
        if(root) {
            root->accept_visitor(*this);
        }
        output_postamble();
        return true;
    }catch(std::runtime_error& err){
        std::cerr << err.what() << '\n';
    }catch(unhandled_register_error& err){
        std::cerr << err.what() << '\n';
    }
    return false;
}