#include <iostream>
#include <fstream>
#include "enma_compiler.h"

ENMA_compiler::ENMA_compiler(): _executable_name("NO NAME"){}

ENMA_compiler::~ENMA_compiler(){}

bool ENMA_compiler::process_arguments(int argc,char* argv[]){
    if(argc == 1){
        std::cout << "Please, enter input files\n";
        return false;
    }
    for(int i = 1; i < argc; i++){
        if(!process_input_file(argv[i])){
            return false;
        }
    }
    return true;
}

void ENMA_compiler::debug_tokens(const class std::list<class token_t>& tokens){
    std::cout << "Tokens debug:\n";
    for(const auto& i : tokens){
        std::cout << i.get_type() << "\t\t->\t\t";
        switch (i.get_type()){
        case token_type_e::CONSTANT: std::cout << i.get_value();
            break;
        case token_type_e::IDENTIFIER: std::cout << _lexer.get_identifier(i.get_value());
            break;
        case token_type_e::OPERATOR: std::cout << static_cast<operator_type_e>(i.get_value());
            break;
        case token_type_e::KEYWORD: std::cout << static_cast<keyword_type_e>(i.get_value());
            break;
        case token_type_e::PUNCTUATION:std::cout << static_cast<punctuation_type_e>(i.get_value());
            break;
        default:
            break;
        }
        std::cout << '\n';
    }
}

bool ENMA_compiler::process_input_file(const std::string& filename){
    if(_executable_name == "NO NAME"){
        _executable_name = filename;
    }
    bool is_correct_ext = false;
    for(int i = filename.size() - 1; i >= 0; i--){
        if(filename[i] == '.'){
            if(filename.size() - i == 3 && filename.substr(i+1,2) == "em"){
                is_correct_ext = true;
            }
            break;
        }
    }
    if(!is_correct_ext){
        std::cout << filename << " - input file extension is not recognized.\nPossible extensions - '.em,'\n";
        return false;
    }


    std::ifstream input_file;
    input_file.open(filename);
    if(!input_file.is_open()){
        std::cerr << filename << " - failed to open,\n";
        return false;
    }else{
        std::cout << filename << " - compiling.\n";
    }

    bool result;
    auto tokens = _lexer.lexical_analysis(input_file, result);
    if(!result){
        return false;
    }else{
        debug_tokens(tokens);
    }

    input_file.close();
    return true;
}