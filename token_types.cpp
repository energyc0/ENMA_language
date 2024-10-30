#include <memory>
#include "token_types.h"
#include "lexer.h"

extern std::unique_ptr<symbol_table> global_sym_table;

std::ostream& operator<<(std::ostream& os, const token_type& t){
    switch (t)
    {
    case token_type::CONSTANT:        os << "CONSTANT";
        break;
    case token_type::IDENTIFIER:      os << "IDENTIFIER";
        break;
    case token_type::OPERATOR:        os << "OPERATOR";
        break;
    case token_type::KEYWORD:         os << "KEYWORD";
        break;
    case token_type::PUNCTUATION:     os << "PUNCTUATION";
        break;
    case token_type::END:             os << "END";
        break;
    case token_type::NEW_LINE:        os << "NEW_LINE";
        break;
    default:
        os << "*undefined token type*";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const operator_type& t){
    switch (t)
    {
    case operator_type::ADD:              os << "+";
        break;
    case operator_type::ASSIGN:           os << "=";
        break;
    case operator_type::DIV:              os << "/";
        break;
    case operator_type::EQUAL:            os << "==";
        break;
    case operator_type::GREATER:          os << ">";
        break;
    case operator_type::GREATER_EQUAL:    os << ">=";
        break;
    case operator_type::LESS:             os << "<";
        break;
    case operator_type::LESS_EQUAl:       os << "<=";
        break;
    case operator_type::LPAR:             os << "(";
        break;
    case operator_type::MUL:              os << "*";
        break;
    case operator_type::NEQUAL:           os << "!=";
        break;
    case operator_type::RPAR:             os << ")";
        break;
    case operator_type::SUB:              os << "-";
        break;
    default:
        os << "*undefined operator type*";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const punctuation_type& t){
    switch (t)
    {
    case punctuation_type::ARROW:     os << "=>";
        break;
    case punctuation_type::COLON:     os << ":";
        break;
    case punctuation_type::COMMA:     os << ",";
        break;
    case punctuation_type::LBRACE:    os << "{";
        break;
    case punctuation_type::RBRACE:    os << "}";
        break;
    case punctuation_type::SEMICOLON: os << ";";
        break;
    default:
        os << "*undefined punctuation type*";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const keyword_type& t){
    switch (t)
    {
    case keyword_type::LET:       os << "let";
        break;
    case keyword_type::RETURN:    os << "return";
        break;
    case keyword_type::IF:        os << "if";
        break;
    case keyword_type::ELSE:     os << "else";
        break;
    case keyword_type::FOR:       os << "for";
        break;
    case keyword_type::WHILE:     os << "while";
        break;
    case keyword_type::PRINT:     os << "print";
        break;
    case keyword_type::TO:        os << "to";
        break;
    default:
        os << "*undefined keyword type*";
        break;
    }
    return os;
}

token_identifier::token_identifier(const std::string& identifier) : token(token_type::IDENTIFIER){
    _id_idx = global_sym_table->try_set_identifier(identifier);
}

std::string token_identifier::get_identifier() const noexcept{
    return global_sym_table->get_identifier(_id_idx);
}