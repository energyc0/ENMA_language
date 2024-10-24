#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "enma_compiler.h"
#include "token_types.h"
#include "ast.h"
#include "lexer.h"
#include "code_generator.h"

extern std::unique_ptr<symbol_table> global_sym_table;

ENMA_compiler::ENMA_compiler(const char* exe_name): _executable_name(exe_name){}

ENMA_compiler::~ENMA_compiler(){}

bool ENMA_compiler::process_input(const std::vector<const char*>& args){
    for(auto& arg : args){
        if(!process_input_file(arg)){
            return false;
        }
    }

    std::string command = "nasm -f elf64 -o " + _executable_name + ".o ";
    for(auto i : args){
        std::string s(i);
        command += s.substr(0, s.size()-2) + "asm ";
    }
    system(command.c_str());
    command = "gcc -o " + _executable_name + " " + _executable_name + ".o -no-pie";
    system(command.c_str());
    return true;
}

void ENMA_debugger::debug_tokens(const class std::list<std::shared_ptr<token>>& tokens){
    std::cout << "Tokens debug:\n";
    for(const auto& i : tokens){
        std::cout.width(25);
        std::cout.fill(' ');
        std::cout << std::left << i->get_type();
        std::cout << "->\t\t";
        switch (i->get_type()){
        case token_type::CONSTANT: std::cout << static_cast<const token_constant&>(*i).get_value();
            break;
        case token_type::IDENTIFIER: std::cout << static_cast<const token_identifier&>(*i).get_identifier();
            break;
        case token_type::OPERATOR: std::cout << static_cast<const token_operator&>(*i).get_operator();
            break;
        case token_type::KEYWORD: std::cout << static_cast<const token_keyword&>(*i).get_keyword();
            break;
        case token_type::PUNCTUATION:std::cout << static_cast<const token_punctuation&>(*i).get_punctuation();
            break;
        case token_type::NEW_LINE: std::cout << "\\n";
            break;
        default:
            break;
        }
        std::cout << '\n';
    }
}

char ENMA_debugger::reinterpret_arith_op(ast_node_type t){
    switch (t){
        case ast_node_type::ADD: return '+';
        case ast_node_type::SUB: return '-';
        case ast_node_type::DIV: return '/';
        case ast_node_type::MUL: return '*';
        default:
            return ' ';
    }
}

void ENMA_debugger::debug_ast(const class std::shared_ptr<class ast_node>& node){
    switch (node->get_type()){
        case ast_node_type::PRINT: {
            std::cout << "print ";
            const auto& print_stat = std::static_pointer_cast<print_statement>(node);
            debug_ast(print_stat->get_expression());
            std::cout << '\n';
            if(print_stat->get_next()) debug_ast(print_stat->get_next());
            break;
        }
        case ast_node_type::ADD:
        case ast_node_type::SUB:
        case ast_node_type::DIV:
        case ast_node_type::MUL:{
            const auto& bin_expr = std::static_pointer_cast<binary_expression>(node);
            debug_ast(bin_expr->get_left());
            std::cout << reinterpret_arith_op(bin_expr->get_type());
            debug_ast(bin_expr->get_right());
            break;
        }
        case ast_node_type::NUM:{
         std::cout << std::static_pointer_cast<number_expression>(node)->get_number();
            break;
        }
        case ast_node_type::ID:{
            std::cout << global_sym_table->get_identifier(std::static_pointer_cast<identifier_expression>(node)->get_id());
            break;
        }
        case ast_node_type::VAR_DECL:{
            std::cout << "let ";
            const auto& var_stat = std::static_pointer_cast<variable_declaration>(node);
            std::cout << var_stat->get_identifier() <<" = ";
            debug_ast(var_stat->get_expression());
            std::cout << '\n';
            debug_ast(var_stat->get_next());
            break;
        }
        default:
            break;
    }
}

bool ENMA_compiler::process_input_file(const std::string& filename){
    try{
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
            throw std::runtime_error(filename + " - input file extension is not recognized.\nPossible extensions - '.em,'\n");
        }

        bool result;

        lexer my_lexer(filename);
        std::cout << filename << " - compiling.\n";
        auto tokens = my_lexer.lexical_analysis();
        ENMA_debugger::debug_tokens(tokens);

        token_storage storage(tokens);
        auto ast = _parser.generate_ast(storage, result);
        if(!result){
            return false;
        }else{
            auto temp_root = ast;
            ENMA_debugger::debug_ast(temp_root);
            std::cout << '\n';
        }
        
        code_generator code_gen(filename.substr(0, filename.size() - 2) + "asm");
        result = code_gen.generate_code(ast);


        if(!result){
            return false;
        }
        std::cout << "generated " << filename.substr(0, filename.size() - 2) << "asm\n";
        return true;
    }catch(std::runtime_error& err){
        std::cerr << err.what();
    }
    return false;
}