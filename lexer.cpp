#include <fstream>
#include <iostream>
#include "lexer.h"

const std::unordered_set<std::string> lexer::_keywords = {
        "let", "return," "if", "for", "while"
    };

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
    if(_identifiers.find(id) != _identifiers.end()){
        return _identifiers[id];
    }else{
        _identifier_codes[_identifiers.size()] = id;
        return _identifiers[id] = _identifiers.size();
    }
}

bool lexer::process_line(const std::string& line, std::list<token_t>& tokens){
    int n = line.size();
    for(int i = 0; i < n; i++){
        if(isspace(line[i]))
            continue;

        if(isdigit(line[i])){
            int num = read_number(line, i);
            tokens.emplace_back(token_t(token_type_e::CONSTANT, num));
            continue;
        }

        switch(line[i]){
            case '+': tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::ADD))); 
                break;
            case '-': tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::SUB)));
                break;
            case '*': tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::MUL)));
                break;
            case '/': tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::DIV)));
                break;
            case '(': tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::LPAR)));
                break;
            case ')': tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::RPAR)));
                break;
            case '=': 
                if(i + 1 < n){
                    if(line[i+1] == '>'){
                        tokens.emplace_back(token_t(token_type_e::PUNCTUATION, static_cast<int>(punctuation_type_e::ARROW)));
                        i++;
                    }else if(line[i+1] == '='){
                        tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::EQUAL)));
                        i++;
                    }else{
                        tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::ASSIGN)));
                    }
                    break;
                }
                tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::ASSIGN)));
                break;
            case ':': tokens.emplace_back(token_t(token_type_e::PUNCTUATION, static_cast<int>(punctuation_type_e::COLON)));
                break;
            case ';': tokens.emplace_back(token_t(token_type_e::PUNCTUATION, static_cast<int>(punctuation_type_e::SEMICOLON)));
                break;                
            case ',': tokens.emplace_back(token_t(token_type_e::PUNCTUATION, static_cast<int>(punctuation_type_e::COMMA)));
                break;
            case '{': tokens.emplace_back(token_t(token_type_e::PUNCTUATION, static_cast<int>(punctuation_type_e::LBRACE)));
                break;
            case '}': tokens.emplace_back(token_t(token_type_e::PUNCTUATION, static_cast<int>(punctuation_type_e::RBRACE)));
                break;
            case '!' :
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::NEQUAL)));
                    i++;
                }else{
                    tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::NOT)));
                }
                break;
            case '<':
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::LESS_EQUAl)));
                    i++;
                }else{
                    tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::LESS)));
                }
                break;
            case '>':
                if(i+1 < n && line[i+1] == '='){
                    tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::GREATER_EQUAL)));
                    i++;
                }else{
                    tokens.emplace_back(token_t(token_type_e::OPERATOR, static_cast<int>(operator_type_e::GREATER)));
                }
                break;
            case 'l':
                if(i + 2 < n && (i+3 == n || isspace(line[i+3]) && line.substr(i, 3) == "let")){
                    i+=3;
                    tokens.emplace_back(token_t(token_type_e::KEYWORD, static_cast<int>(keyword_type_e::LET)));
                }else{
                    tokens.emplace_back(token_t(token_type_e::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'r':
                if(i + 5 < n && (i + 6 == n || isspace(line[i+6])) && line.substr(i, 6) == "return"){
                    i+=6;
                    tokens.emplace_back(token_t(token_type_e::KEYWORD, static_cast<int>(keyword_type_e::RETURN)));
                }else{
                    tokens.emplace_back(token_t(token_type_e::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'i':
                if((i + 1 == n || isspace(line[i+1])) && line.substr(i,2) == "if"){
                    i+=2;
                    tokens.emplace_back(token_t(token_type_e::KEYWORD, static_cast<int>(keyword_type_e::IF)));
                }else{
                    tokens.emplace_back(token_t(token_type_e::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'f':
                if(i + 2 < n && (i + 3 == n || isspace(line[i+3])) && line.substr(i,3) == "for"){
                    i+=3;
                    tokens.emplace_back(token_t(token_type_e::KEYWORD, static_cast<int>(keyword_type_e::FOR)));
                }else{
                    tokens.emplace_back(token_t(token_type_e::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            case 'w':
                if(i + 4 < n && (i + 5 == n || isspace(line[i+5])) && line.substr(i,3) == "while"){
                    i+=3;
                    tokens.emplace_back(token_t(token_type_e::KEYWORD, static_cast<int>(keyword_type_e::WHILE)));
                }else{
                    tokens.emplace_back(token_t(token_type_e::IDENTIFIER, read_identifier(line, i)));
                }
                break;
            default:
                tokens.emplace_back(token_t(token_type_e::IDENTIFIER, read_identifier(line, i)));
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
        tokens.emplace_back(token_type_e::NEW_LINE,0);
    }

    return tokens;
}