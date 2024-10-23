#include <fstream>
#include <iostream>
#include <memory>
#include "lexer.h"
#include "token_types.h"

const std::unordered_set<std::string> lexer::_keywords = {
        "let", "return," "if", "for", "while"
    };

std::unique_ptr<symbol_table> global_sym_table = std::make_unique<symbol_table>();

int lexer::read_number(const std::string& line, int& idx) const noexcept{
    int num = 0;

    while(isdigit(line[idx])){
        num = num * 10 + line[idx] - '0';
        idx++;
    }

    idx--;
    return num;
}

std::string lexer::read_identifier(const std::string& line, int& idx) noexcept{
    int start = idx;
    do{
        idx++;
    }while(isalpha(line[idx]) || line[idx] == '_');
    return line.substr(start, idx-- - start);
}

void lexer::process_line(const std::string& line, std::list<std::shared_ptr<token>>& tokens){
    int n = line.size();
    for(int i = 0; i < n; i++){
        if(isspace(line[i]))
            continue;

        if(isdigit(line[i])){
            int num = read_number(line, i);
            tokens.emplace_back(std::make_shared<token_constant>(token_constant(num)));
            continue;
        }

        switch(line[i]){
            case '+': tokens.emplace_back(std::make_shared<token_operator>(token_operator(operator_type::ADD))); 
                break;
            case '-': tokens.emplace_back(std::make_shared<token_operator>(operator_type::SUB));
                break;
            case '*': tokens.emplace_back(std::make_shared<token_operator>(operator_type::MUL));
                break;
            case '/': tokens.emplace_back(std::make_shared<token_operator>(operator_type::DIV));
                break;
            case '(': tokens.emplace_back(std::make_shared<token_operator>(operator_type::LPAR));
                break;
            case ')': tokens.emplace_back(std::make_shared<token_operator>(operator_type::RPAR));
                break;
            case '=': 
                if(i + 1 < n){
                    if(line[i+1] == '>'){
                        tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::ARROW));
                        i++;
                    }else if(line[i+1] == '='){
                        tokens.emplace_back(std::make_shared<token_operator>(operator_type::EQUAL));
                        i++;
                    }
                }
                tokens.emplace_back(std::make_shared<token_operator>(operator_type::ASSIGN));
                break;
            case ':': tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::COLON));
                break;
            case ';': tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::SEMICOLON));
                break;                
            case ',': tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::COMMA));
                break;
            case '{': tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::LBRACE));
                break;
            case '}': tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::RBRACE));
                break;
            case '!' :
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(std::make_shared<token_operator>(operator_type::NEQUAL));
                    i++;
                }else{
                    tokens.emplace_back(std::make_shared<token_operator>(operator_type::NOT));
                }
                break;
            case '<':
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(std::make_shared<token_operator>(operator_type::LESS_EQUAl));
                    i++;
                }else{
                    tokens.emplace_back(std::make_shared<token_operator>(operator_type::LESS));
                }
                break;
            case '>':
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(std::make_shared<token_operator>(operator_type::GREATER_EQUAL));
                    i++;
                }else{
                    tokens.emplace_back(std::make_shared<token_operator>(operator_type::GREATER));
                }
                break;
            case 'l':
                if(i + 2 < n && (i+3 == n || isspace(line[i+3]) && line.substr(i, 3) == "let")){
                    i+=3;
                    tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::LET));
                }else{
                    tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, i)));
                }
                break;
            case 'r':
                if(i + 5 < n && (i + 6 == n || isspace(line[i+6])) && line.substr(i, 6) == "return"){
                    i+=6;
                    tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::RETURN));
                }else{
                    tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, i)));
                }
                break;
            case 'i':
                if((i + 1 == n || isspace(line[i+1] || line[i+1] == '(')) && line.substr(i,2) == "if"){
                    i+=2;
                    tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::IF));
                }else{
                    tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, i)));
                }
                break;
            case 'f':
                if(i + 2 < n && (i + 3 == n || isspace(line[i+3] || line[i+3] == '(')) && line.substr(i,3) == "for"){
                    i+=3;
                    tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::FOR));
                }else{
                    tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, i)));
                }
                break;
            case 'w':
                if(i + 4 < n && (i + 5 == n || isspace(line[i+5]) || line[i+5] == '(') && line.substr(i,3) == "while"){
                    i+=5;
                    tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::WHILE));
                }else{
                    tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, i)));
                }
                break;
            case 'p':
                if(i + 4 < n && (i + 5 == n || isspace(line[i+5]) || line[i+5] == '(') && line.substr(i,5) == "print"){
                    i+=5;
                    tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::PRINT));
                }else{
                    tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, i)));    
                }
                break;
            default:
                tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, i)));
        }
    }
}

lexer::lexer(const std::string& input_file) {
    _file.open(input_file);
    if(!_file.is_open()){
        _file.close();
        throw std::runtime_error(input_file + " - failed to open,\n");
    }

}
lexer::~lexer(){
    _file.close();
}

std::list<std::shared_ptr<token>> lexer::lexical_analysis(){
    std::list<std::shared_ptr<token>> tokens;


    while(!_file.eof()){
        std::string code_line;
        std::getline(_file, code_line);
        process_line(code_line, tokens);
        tokens.emplace_back(std::make_shared<token_new_line>());
    }

    return tokens;
}