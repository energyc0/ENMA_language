#include <fstream>
#include <iostream>
#include <memory>
#include "lexer.h"
#include "token_types.h"

const std::unordered_set<std::string> lexer::_keywords = {
        "let", "return," "if", "for", "while"
    };

std::unique_ptr<symbol_table> global_sym_table = std::make_unique<symbol_table>();

int lexer::read_number(const std::string& line, int& idx, bool is_negative) const noexcept{
    int num = 0;

    while(isdigit(line[idx])){
        num = num * 10 + line[idx] - '0';
        idx++;
    }

    idx--;
    return is_negative ? -num : num;
}

std::string lexer::read_identifier(const std::string& line, int& idx) noexcept{
    int start = idx;
    do{
        idx++;
    }while(isalpha(line[idx]) || isdigit(line[idx]) || line[idx] == '_');
    return line.substr(start, idx-- - start);
}

void lexer::emplace_identifier(const std::string& line, int& idx) noexcept{
    auto iter = _tokens.rbegin();
    if(!_tokens.empty() && is_match(*iter, operator_type::SUB)){
        if(++iter == _tokens.rend() || 
        !is_match(*iter, token_type::CONSTANT) && !is_match(*iter, token_type::IDENTIFIER)){
            _tokens.pop_back();
            _tokens.emplace_back(std::make_shared<token_constant>(-1));
            _tokens.emplace_back(std::make_shared<token_operator>(operator_type::MUL));
        }
    }
    _tokens.emplace_back(std::make_shared<token_identifier>(read_identifier(line, idx)));
}

void lexer::process_line(const std::string& line){
    int n = line.size();
    for(int i = 0; i < n; i++){
        if(isspace(line[i]))
            continue;

        if(isdigit(line[i])){
            _tokens.emplace_back(std::make_shared<token_constant>(read_number(line, i, false)));
            continue;
        }

        switch(line[i]){
            case '+': _tokens.emplace_back(std::make_shared<token_operator>(token_operator(operator_type::ADD))); 
                break;
            case '-':{
                if((_tokens.empty() || (!is_match(_tokens.back(), token_type::CONSTANT) && !is_match(_tokens.back(), token_type::IDENTIFIER))) &&
                 i+1 < n && isdigit(line[i+1])){
                    _tokens.emplace_back(std::make_shared<token_constant>(read_number(line, ++i, true)));
                }else if (i+1 < n && line[i+1] == '('){
                    _tokens.emplace_back(std::make_shared<token_constant>(-1));
                    _tokens.emplace_back(std::make_shared<token_operator>(operator_type::MUL));
                }else{
                    _tokens.emplace_back(std::make_shared<token_operator>(operator_type::SUB));
                 }
                break;
            }
            case '*': _tokens.emplace_back(std::make_shared<token_operator>(operator_type::MUL));
                break;
            case '/': _tokens.emplace_back(std::make_shared<token_operator>(operator_type::DIV));
                break;
            case '(': _tokens.emplace_back(std::make_shared<token_operator>(operator_type::LPAR));
                break;
            case ')': _tokens.emplace_back(std::make_shared<token_operator>(operator_type::RPAR));
                break;
            case '=': 
                if(i + 1 < n){
                    if(line[i+1] == '>'){
                        _tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::ARROW));
                        i++;
                        break;
                    }else if(line[i+1] == '='){
                        _tokens.emplace_back(std::make_shared<token_operator>(operator_type::EQUAL));
                        i++;
                        break;
                    }
                }
                _tokens.emplace_back(std::make_shared<token_operator>(operator_type::ASSIGN));
                break;
            case ':': _tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::COLON));
                break;
            case ';': _tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::SEMICOLON));
                break;                
            case ',': _tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::COMMA));
                break;
            case '{': _tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::LBRACE));
                break;
            case '}': _tokens.emplace_back(std::make_shared<token_punctuation>(punctuation_type::RBRACE));
                break;
            case '!' :
                if(i+1 < n && line[i+1] == '='){
                    _tokens.emplace_back(std::make_shared<token_operator>(operator_type::NEQUAL));
                    i++;
                }else{
                    throw std::runtime_error("undefined symbol '!', '!=' expected\n");
                }
                break;
            case '<':
                if(i+1 < n && line[i+1] == '='){
                    _tokens.emplace_back(std::make_shared<token_operator>(operator_type::LESS_EQUAl));
                    i++;
                }else{
                    _tokens.emplace_back(std::make_shared<token_operator>(operator_type::LESS));
                }
                break;
            case '>':
                if(i+1 < n && line[i+1] == '='){
                    _tokens.emplace_back(std::make_shared<token_operator>(operator_type::GREATER_EQUAL));
                    i++;
                }else{
                    _tokens.emplace_back(std::make_shared<token_operator>(operator_type::GREATER));
                }
                break;
            case 'l':
                if(i + 2 < n && (i+3 >= n || isspace(line[i+3]) && line.substr(i, 3) == "let")){
                    i+=3;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::LET));
                }else{
                    emplace_identifier(line,i);
                }
                break;
            case 'r':
                if(i + 5 < n && (i + 6 >= n || isspace(line[i+6])) && line.substr(i, 6) == "return"){
                    i+=6;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::RETURN));
                }else{
                    emplace_identifier(line,i);
                }
                break;
            case 'i':
                if(i+1 < n &&  (i + 2 >= n || isspace(line[i+2]) || std::ispunct(line[i+2])) && line.substr(i,2) == "if"){
                    i++;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::IF));
                }else{
                    emplace_identifier(line,i);
                }
                break;
            case 'e':
                if(i+3 < n &&  (i + 4 >= n || isspace(line[i+4]) || std::ispunct(line[i+4])) && line.substr(i,4) == "else"){
                    i+=3;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::ELSE));
                }else{
                    emplace_identifier(line,i);
                }
                break;
            case 'f':
                if(i + 2 < n && (i + 3 >= n || isspace(line[i+3] || line[i+3] == '(')) && line.substr(i,3) == "for"){
                    i+=2;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::FOR));
                }else{
                    emplace_identifier(line,i);
                }
                break;
            case 'w':
                if(i + 4 < n && (i + 5 >= n || isspace(line[i+5]) || ispunct(line[i+5])) && line.substr(i,5) == "while"){
                    i+=4;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::WHILE));
                }else{
                    emplace_identifier(line,i);
                }
                break;
            case 'p':
                if(i + 4 < n && (i + 5 >= n || isspace(line[i+5]) || line[i+5] == '(') && line.substr(i,5) == "print"){
                    i+=4;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::PRINT));
                }else{
                    emplace_identifier(line,i);  
                }
                break;
            case 't':
                if(i + 1 < n && (i + 2 >= n || isspace(line[i+2])) && line.substr(i,2) == "to"){
                    i++;
                    _tokens.emplace_back(std::make_shared<token_keyword>(keyword_type::TO));
                }else{
                    emplace_identifier(line,i);  
                }
                break;
            default:
                emplace_identifier(line,i);
                break;
        }
    }
}

lexer::lexer(const std::string& input_file) : _file(input_file){
    if(!_file.is_open()){
        _file.close();
        throw std::runtime_error(input_file + " - failed to open,\n");
    }

}
lexer::~lexer(){
    _file.close();
}

std::list<std::shared_ptr<token>> lexer::lexical_analysis(){
    _tokens.clear();


    while(!_file.eof()){
        std::string code_line;
        std::getline(_file, code_line);
        process_line(code_line);
        _tokens.emplace_back(std::make_shared<token_new_line>());
    }

    return _tokens;
}