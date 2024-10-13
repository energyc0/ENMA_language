#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "enma_compiler.h"
#include "enma_types.h"

ENMA_compiler::ENMA_compiler(const char* exe_name): _executable_name(exe_name){}

ENMA_compiler::~ENMA_compiler(){}

bool ENMA_compiler::process_input(const std::vector<const char*>& args){
    for(auto& arg : args){
        if(!process_input_file(arg)){
            return false;
        }
    }
    return true;
}

void ENMA_compiler::debug_tokens(const class std::list<class token_t>& tokens){
    std::cout << "Tokens debug:\n";
    for(const auto& i : tokens){
        std::cout.width(25);
        std::cout.fill(' ');
        std::cout << std::left << i.get_type();
        std::cout << "->\t\t";
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
        case token_type_e::NEW_LINE: std::cout << "\\n";
            break;
        default:
            break;
        }
        std::cout << '\n';
    }
}

void ENMA_compiler::debug_ast(const ast_node_t* node){
    if(node->left){
        debug_ast(node->left.get());
    }

    switch (node->type)
    {
    case ast_node_type_e::ADD: std::cout << "+";
        break;
    case ast_node_type_e::SUB: std::cout << "-";
        break;
    case ast_node_type_e::DIV: std::cout << "/";
        break;
    case ast_node_type_e::MUL: std::cout << "*";
        break;
    case ast_node_type_e::NUM: std::cout << node->val;
        break;
    default:
        break;
    }

    if(node->right){
        debug_ast(node->right.get());
    }
}

bool ENMA_compiler::process_input_file(const std::string& filename){
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
        input_file.close();
        return false;
    }else{
        std::cout << filename << " - compiling.\n";
    }
    input_file.close();

    bool result;
    auto tokens = _lexer.lexical_analysis(input_file, result);
    if(!result){
        return false;
    }else{
        debug_tokens(tokens);
    }

    token_storage storage(tokens);
    auto ast = _parser.binary_expr(storage, result);
    if(!result){
        return false;
    }

    std::ofstream output_file;
    output_file.open(filename + ".asm");
    if(!output_file.is_open()){
        std::cerr << "failed to open the output file: " << filename << ".asm\n";
        output_file.close();
        return false;

    }
    result = _code_generator.generate_code(output_file, ast);
    output_file.close();
    if(!result){
        return false;
    }
    std::cout << "generated " << filename << ".asm\n";
    return true;
}