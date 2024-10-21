#include <fstream>
#include <iostream>
#include <memory>
#include "lexer.h"
#include "enma_types.h"

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

int lexer::read_identifier(const std::string& line, int& idx) noexcept{
    int start = idx;
    do{
        idx++;
    }while(isalpha(line[idx]) || line[idx] == '_');
    std::string id= line.substr(start, idx-- - start);
    return global_sym_table->try_set_identifier(id);
}

bool lexer::process_line(const std::string& line, std::list<token_t>& tokens){
    int n = line.size();
    for(int i = 0; i < n; i++){
        if(isspace(line[i]))
            continue;

        if(isdigit(line[i])){
            int num = read_number(line, i);
            tokens.emplace_back(token_t(token_type::CONSTANT, num));
            continue;
        }

        switch(line[i]){
            case '+': tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::ADD))); 
                break;
            case '-': tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::SUB)));
                break;
            case '*': tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::MUL)));
                break;
            case '/': tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::DIV)));
                break;
            case '(': tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::LPAR)));
                break;
            case ')': tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::RPAR)));
                break;
            case '=': 
                if(i + 1 < n){
                    if(line[i+1] == '>'){
                        tokens.emplace_back(token_t(token_type::PUNCTUATION, static_cast<int>(punctuation_type::ARROW)));
                        i++;
                    }else if(line[i+1] == '='){
                        tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::EQUAL)));
                        i++;
                    }else{
                        tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::ASSIGN)));
                    }
                    break;
                }
                tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::ASSIGN)));
                break;
            case ':': tokens.emplace_back(token_t(token_type::PUNCTUATION, static_cast<int>(punctuation_type::COLON)));
                break;
            case ';': tokens.emplace_back(token_t(token_type::PUNCTUATION, static_cast<int>(punctuation_type::SEMICOLON)));
                break;                
            case ',': tokens.emplace_back(token_t(token_type::PUNCTUATION, static_cast<int>(punctuation_type::COMMA)));
                break;
            case '{': tokens.emplace_back(token_t(token_type::PUNCTUATION, static_cast<int>(punctuation_type::LBRACE)));
                break;
            case '}': tokens.emplace_back(token_t(token_type::PUNCTUATION, static_cast<int>(punctuation_type::RBRACE)));
                break;
            case '!' :
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::NEQUAL)));
                    i++;
                }else{
                    tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::NOT)));
                }
                break;
            case '<':
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::LESS_EQUAl)));
                    i++;
                }else{
                    tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::LESS)));
                }
                break;
            case '>':
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::GREATER_EQUAL)));
                    i++;
                }else{
                    tokens.emplace_back(token_t(token_type::OPERATOR, static_cast<int>(operator_type::GREATER)));
                }
                break;
            case 'l':
                if(i + 2 < n && (i+3 == n || isspace(line[i+3]) && line.substr(i, 3) == "let")){
                    i+=3;
                    tokens.emplace_back(token_t(token_type::KEYWORD, static_cast<int>(keyword_type::LET)));
                }else{
                    tokens.emplace_back(token_t(token_type::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'r':
                if(i + 5 < n && (i + 6 == n || isspace(line[i+6])) && line.substr(i, 6) == "return"){
                    i+=6;
                    tokens.emplace_back(token_t(token_type::KEYWORD, static_cast<int>(keyword_type::RETURN)));
                }else{
                    tokens.emplace_back(token_t(token_type::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'i':
                if((i + 1 == n || isspace(line[i+1] || line[i+1] == '(')) && line.substr(i,2) == "if"){
                    i+=2;
                    tokens.emplace_back(token_t(token_type::KEYWORD, static_cast<int>(keyword_type::IF)));
                }else{
                    tokens.emplace_back(token_t(token_type::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'f':
                if(i + 2 < n && (i + 3 == n || isspace(line[i+3] || line[i+3] == '(')) && line.substr(i,3) == "for"){
                    i+=3;
                    tokens.emplace_back(token_t(token_type::KEYWORD, static_cast<int>(keyword_type::FOR)));
                }else{
                    tokens.emplace_back(token_t(token_type::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'w':
                if(i + 4 < n && (i + 5 == n || isspace(line[i+5]) || line[i+5] == '(') && line.substr(i,3) == "while"){
                    i+=5;
                    tokens.emplace_back(token_t(token_type::KEYWORD, static_cast<int>(keyword_type::WHILE)));
                }else{
                    tokens.emplace_back(token_t(token_type::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'p':
                if(i + 4 < n && (i + 5 == n || isspace(line[i+5]) || line[i+5] == '(') && line.substr(i,5) == "print"){
                    i+=5;
                    tokens.emplace_back(token_t(token_type::KEYWORD, static_cast<int>(keyword_type::PRINT)));
                }else{
                    tokens.emplace_back(token_t(token_type::IDENTIFIER, read_identifier(line, i)));    
                }
                break;
            default:
                tokens.emplace_back(token_t(token_type::IDENTIFIER, read_identifier(line, i)));
        }
    }
    return true;
}

std::list<token_t> lexer::lexical_analysis(std::ifstream& file, bool& result){
    std::list<token_t> tokens;
    result = true;

    while(!file.eof()){
        std::string code_line;
        std::getline(file, code_line);
        if(!process_line(code_line, tokens)){
            result = false;
            break;
        }
        tokens.emplace_back(token_type::NEW_LINE,0);
    }

    return tokens;
}