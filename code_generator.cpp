#include "code_generator.h"
#include "parser.h"

unhandled_register_error::unhandled_register_error(const char* msg, const char* reg) 
: _msg("register: " + std::string(reg) + "\n" + std::string(msg)) {}

std::string unhandled_register_error::what(){
    return _msg;
}

void code_generator::output_preamble(){
    *_file  << "section .text\n"
            << "\tglobal main\n"
            << "main:\n";
}

void code_generator::output_ending(){
    *_file  << "\tmov rax, 60\n"
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
        if(_free_reg[i])
            return i;
    }
    throw std::runtime_error("failed to find free register");
}

void code_generator::mov_reg(int reg, int val){
    *_file << "\tmov " << _registers[reg] << ", " << val << '\n';
}

void code_generator::add_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("addition - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("addition - undefined storage", _registers[right]);
    *_file << "\tadd " << _registers[left] << ", " << _registers[right] << '\n';
}

void code_generator::sub_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("substraction - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("substraction - undefined storage", _registers[right]);
    *_file << "\tsub " << _registers[left] << ", " << _registers[right] << '\n';
}

void code_generator::mul_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("multiplication - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("multiplication - undefined storage", _registers[right]);
    *_file << "\tmul " << _registers[left] << ", " << _registers[right] << '\n';        
}

void code_generator::div_reg(int left, int right){
    if(_free_reg[left])
        throw unhandled_register_error("division - undefined storage", _registers[left]);
    if(_free_reg[right])
        throw unhandled_register_error("division - undefined storage", _registers[right]);
    *_file << "\tdiv " << _registers[left] << ", " << _registers[right] << '\n';
}

bool code_generator::generate_code(std::ofstream& file, std::shared_ptr<ast_node_t>& root){
    _file = &file;
    output_preamble();

    output_ending();
    return true;
}