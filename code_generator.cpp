#include <iostream>
#include "parser.h"
#include "code_generator.h"

unhandled_register_error::unhandled_register_error(const char* msg, const code_register& reg) 
: _msg("register: " + std::string(reg.get_name()) + "\n" + std::string(msg)) {}

std::string unhandled_register_error::what(){
    return _msg;
}

void code_generator::output_preamble(){
    _file  << "section .data\n"
            << "\td_fmt db '%d',10,0\n\n"
            << "section .text\n"
            << "\textern printf\n"
            << "\tglobal main\n"
            << "main:\n"
            << "\tpush rbp\n"
            << "\tmov rbp, rsp\n\n";
}

void code_generator::output_postamble(){
    _file  << "\n\tpop rbp\n"
            << "\tmov rax, 60\n"
            << "\tmov rdi, 0\n"
            << "\tsyscall";
}

int code_generator::find_free_reg(){
    for(int i = 0; i < _registers_count; i++){
        if(!_registers[i].is_busy()){
            return i;
        }
    }
    throw std::runtime_error("failed to find free register");
}

int code_generator::mov_reg(int reg, int val){
    _file << "\tmov " << _registers[reg].get_name() << ", " << val << '\n';
    _registers[reg].become_busy();
    return reg;
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

void code_generator::print_reg(int reg){
    check_valid_storage(_registers[reg]);

    _file  << "\n\tmov rdi, d_fmt\n"
            << "\tmov rsi, " << _registers[reg].get_name() << '\n'
            << "\tcall printf\n\n";
}

/*
int code_generator::traverse_node(std::shared_ptr<ast_node>& node){
    if(!node)
        return -1;
    
    int left_reg = 0,
    right_reg = 0;
    if(node->left){
        left_reg = traverse_node(node->left);
    }
    switch (node->type){
    case ast_node_type::PRINT:
        print_reg(left_reg);
        if(node->right){
            return traverse_node(node->right);
        }
        return -1;
    case ast_node_type::END: throw std::runtime_error("ast_node_type == END");
    default:
        break;
    }
    if(node->right){
        right_reg = traverse_node(node->right);
    }

    switch (node->type){
        case ast_node_type::NUM: return mov_reg(find_free_reg(), node->val);
        case ast_node_type::ADD: return add_reg(left_reg,right_reg);
        case ast_node_type::SUB: return sub_reg(left_reg,right_reg);
        case ast_node_type::DIV: return div_reg(left_reg,right_reg);
        case ast_node_type::MUL: return mul_reg(left_reg,right_reg);
        case ast_node_type::PRINT: throw std::runtime_error("ast_node_type == PRINT");
        case ast_node_type::END: throw std::runtime_error("ast_node_type == END");
    default:
        throw std::runtime_error("ast_node_type == undefined");
        break;
    }
    return -1;
}
*/

int code_generator::node_interaction(const number_expression* expr){
    return mov_reg(find_free_reg(),expr->get_number());
}
int code_generator::node_interaction(const identifier_expression* expr){
    return expr->get_id();
}
int code_generator::node_interaction(const binary_expression* expr){
    int left_reg = expr->get_left()->accept_visitor(*this);
    int right_reg = expr->get_right()->accept_visitor(*this);

    switch (expr->get_type()){
        case ast_node_type::ADD: return add_reg(left_reg,right_reg);
        case ast_node_type::SUB: return sub_reg(left_reg,right_reg);
        case ast_node_type::DIV: return div_reg(left_reg,right_reg);
        case ast_node_type::MUL: return mul_reg(left_reg,right_reg);
        default:
            throw std::runtime_error("undefined binary expression operator\n");
    }
}
void code_generator::node_interaction(const print_statement* expr) {
    print_reg(expr->get_expression()->accept_visitor(*this));
    if(expr->get_next())
        expr->get_next()->accept_visitor(*this);
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