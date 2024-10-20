#include <iostream>
#include <string>

enum class token_type_e{
    CONSTANT,
    IDENTIFIER,
    OPERATOR,
    KEYWORD,
    PUNCTUATION,
    NEW_LINE,
    END
};
std::ostream& operator<<(std::ostream& os, const token_type_e& t);

enum class operator_type_e{
    END,
    ADD,
    SUB,
    DIV,
    MUL,
    LPAR,
    RPAR,
    ASSIGN,
    EQUAL,
    NEQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAl,
    NOT
};
std::ostream& operator<<(std::ostream& os, const operator_type_e& t);

enum class punctuation_type_e{
    COLON,
    SEMICOLON,
    COMMA,
    LBRACE,
    RBRACE,
    ARROW   //=>
};
std::ostream& operator<<(std::ostream& os, const punctuation_type_e& t);

enum class keyword_type_e{
    LET,
    RETURN,
    IF,
    FOR,
    WHILE,
    PRINT
};
std::ostream& operator<<(std::ostream& os, const keyword_type_e& t);

class token_t{
protected:
    token_type_e _type;
    int _val;
public:
    inline token_type_e get_type() const noexcept {return _type;}
    inline int get_value() const noexcept {return _val;}

    token_t(token_type_e t, int v) : _type(t), _val(v) {}
    token_t(const token_t& token) : _type(token._type), _val(token._val){}
    token_t(token_t&& token) : _type(token._type),_val(token._val){}

    constexpr token_t& operator=(const token_t& t) {
        _type = t._type;
        _val = t._val;
        return *this;
    }

    virtual ~token_t(){}
};