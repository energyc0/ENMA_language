#include "enma_types.h"

std::ostream& operator<<(std::ostream& os, const token_type_e& t){
    switch (t)
    {
    case token_type_e::CONSTANT:        os << "CONSTANT";
        break;
    case token_type_e::IDENTIFIER:      os << "IDENTIFIER";
        break;
    case token_type_e::OPERATOR:        os << "OPERATOR";
        break;
    case token_type_e::KEYWORD:         os << "KEYWORD";
        break;
    case token_type_e::PUNCTUATION:     os << "PUNCTUATION";
        break;
    case token_type_e::END:             os << "END";
        break;
    case token_type_e::NEW_LINE:        os << "NEW_LINE";
        break;
    default:
        os << "*undefined token type*";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const operator_type_e& t){
    switch (t)
    {
    case operator_type_e::ADD:              os << "+";
        break;
    case operator_type_e::ASSIGN:           os << "=";
        break;
    case operator_type_e::DIV:              os << "/";
        break;
    case operator_type_e::EQUAL:            os << "==";
        break;
    case operator_type_e::GREATER:          os << ">";
        break;
    case operator_type_e::GREATER_EQUAL:    os << ">=";
        break;
    case operator_type_e::LESS:             os << "<";
        break;
    case operator_type_e::LESS_EQUAl:       os << "<=";
        break;
    case operator_type_e::LPAR:             os << "(";
        break;
    case operator_type_e::MUL:              os << "*";
        break;
    case operator_type_e::NEQUAL:           os << "!=";
        break;
    case operator_type_e::NOT:              os << "!";
        break;
    case operator_type_e::RPAR:             os << ")";
        break;
    case operator_type_e::SUB:              os << "-";
        break;
    default:
        os << "*undefined operator type*";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const punctuation_type_e& t){
    switch (t)
    {
    case punctuation_type_e::ARROW:     os << "=>";
        break;
    case punctuation_type_e::COLON:     os << ":";
        break;
    case punctuation_type_e::COMMA:     os << ",";
        break;
    case punctuation_type_e::LBRACE:    os << "{";
        break;
    case punctuation_type_e::RBRACE:    os << "}";
        break;
    case punctuation_type_e::SEMICOLON: os << ";";
        break;
    default:
        os << "*undefined punctuation type*";
        break;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const keyword_type_e& t){
    switch (t)
    {
    case keyword_type_e::LET:       os << "let";
        break;
    case keyword_type_e::RETURN:    os << "return";
        break;
    case keyword_type_e::IF:        os << "if";
        break;
    case keyword_type_e::FOR:       os << "for";
        break;
    case keyword_type_e::WHILE:     os << "while";
        break;
    case keyword_type_e::PRINT:     os << "print";
        break;
    default:
        os << "*undefined keyword type*";
        break;
    }
    return os;
}