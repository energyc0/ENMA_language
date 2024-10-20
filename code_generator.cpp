#include <iostream>
#include "code_generator.h"
#include "parser.h"

unhandled_register_error::unhandled_register_error(const char* msg, const char* reg) 
: _msg("register: " + std::string(reg) + "\n" + std::string(msg)) {}

std::string unhandled_register_error::what(){
    return _msg;
}

void code_generator::output_preamble(){
    *_file  << "section .data\n"
            << "\td_fmt db '%d',10,0\n\n"
            << "section .text\n"
            << "\textern printf\n"
            << "\tglobal main\n"
            << "main:\n"
            << "\tpush rbp\n"
            << "\tmov rbp, rsp\n\n";
}

void code_generator::output_postamble(){
    *_file  << "\n\tpop rbp\n"
            << "\tmov rax, 60\n"
            << "\tmov rdi, 0\n"
            << "\tsyscall";
}

void code_generator::free_reg(int reg){
    if(_free_reg[reg])
        throw unhandled_register_error("register is free already", _registers[reg]);
    _free_reg[reg] = true;
}

int code_generator::find_free_reg(){
    for(int i = 0; i < _registers_count; i++){
        if(_free_reg[i]){
            _free_reg[i] = false;
            return i;
        }
    }
    throw std::runtime_error("failed to find free register");
}

int code_generator::mov_reg(int reg, int val){
    *_file << "\tmov " << _registers[reg] << ", " << val << '\n';
    _free_reg[reg] = false;
    return reg;
}

int code_generator::add_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("addition - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("addition - undefined storage", _registers[right]);
    *_file << "\tadd " << _registers[left] << ", " << _registers[right] << '\n';
    free_reg(right);
    return left;
}

int code_generator::sub_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("substraction - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("substraction - undefined storage", _registers[right]);
    *_file << "\tsub " << _registers[left] << ", " << _registers[right] << '\n';
    free_reg(right);
    return left;
}

int code_generator::mul_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("multiplication - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("multiplication - undefined storage", _registers[right]);
    *_file << "\timul " << _registers[left] << ", " << _registers[right] << '\n';
    free_reg(right);
    return left;        
}

int code_generator::div_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("division - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("division - undefined storage", _registers[right]);
    *_file  << "\tmov rax, " << _registers[left] << "\n"
            << "\txor rdx, rdx\n"
            << "\tdiv " << _registers[right] << '\n'
            << "\tmov " << _registers[left] << ", rax\n";
    free_reg(right);
    return left;
}

void code_generator::print_reg(int reg){
    if(_free_reg[reg])
        throw unhandled_register_error("print_reg - undefined register storage", _registers[reg]);
    *_file  << "\n\tmov rdi, d_fmt\n"
            << "\tmov rsi, " << _registers[reg] << '\n'
            << "\tcall printf\n\n";
}

int code_generator::traverse_node(std::shared_ptr<ast_node_t>& node){
    int left_reg = 0,
    right_reg = 0;
    if(node->left){
        left_reg = traverse_node(node->left);
    }
    switch (node->type){
    case ast_node_type_e::PRINT:
        print_reg(left_reg);
        if(node->right){
            return traverse_node(node->right);
        }
        return -1;
    case ast_node_type_e::END: throw std::runtime_error("ast_node_type_e == END");
    default:
        break;
    }
    if(node->right){
        right_reg = traverse_node(node->right);
    }

    switch (node->type){
        case ast_node_type_e::NUM: return mov_reg(find_free_reg(), node->val);
        case ast_node_type_e::ADD: return add_reg(left_reg,right_reg);
        case ast_node_type_e::SUB: return sub_reg(left_reg,right_reg);
        case ast_node_type_e::DIV: return div_reg(left_reg,right_reg);
        case ast_node_type_e::MUL: return mul_reg(left_reg,right_reg);
        case ast_node_type_e::PRINT: throw std::runtime_error("ast_node_type_e == PRINT");
        case ast_node_type_e::END: throw std::runtime_error("ast_node_type_e == END");
    default:
        throw std::runtime_error("ast_node_type_e == undefined");
        break;
    }
    return -1;
}

bool code_generator::generate_code(std::ofstream& file, std::shared_ptr<ast_node_t>& root){
    try{
        _file = &file;
        output_preamble();
        if(root) {
            int reg = traverse_node(root);
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