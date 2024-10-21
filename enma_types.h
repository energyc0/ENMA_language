#include <iostream>
#include <string>

enum class token_type{
    CONSTANT,
    IDENTIFIER,
    OPERATOR,
    KEYWORD,
    PUNCTUATION,
    NEW_LINE,
    END
};
std::ostream& operator<<(std::ostream& os, const token_type& t);

enum class operator_type{
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
std::ostream& operator<<(std::ostream& os, const operator_type& t);

enum class punctuation_type{
    COLON,
    SEMICOLON,
    COMMA,
    LBRACE,
    RBRACE,
    ARROW   //=>
};
std::ostream& operator<<(std::ostream& os, const punctuation_type& t);

enum class keyword_type{
    LET,
    RETURN,
    IF,
    FOR,
    WHILE,
    PRINT
};
std::ostream& operator<<(std::ostream& os, const keyword_type& t);

class token_t{
protected:
    token_type _type;
    int _val;
public:
    inline token_type get_type() const noexcept {return _type;}
    inline int get_value() const noexcept {return _val;}

    token_t(token_type t, int v) : _type(t), _val(v) {}
    token_t(const token_t& token) : _type(token._type), _val(token._val){}
    token_t(token_t&& token) : _type(token._type),_val(token._val){}

    constexpr token_t& operator=(const token_t& t) {
        _type = t._type;
        _val = t._val;
        return *this;
    }

    virtual ~token_t(){}
};