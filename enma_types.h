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

class token{
protected:
    token_type _type;

    token(token_type t) : _type(t) {}
    token(const token& token) : _type(token._type){}
    token(token&& token) : _type(token._type){}
public:
    virtual inline token_type get_type() const noexcept {return _type;}

    virtual token& operator=(const token& t) {
        _type = t._type;
        return *this;
    }

    virtual ~token(){}
};

class token_keyword : public token{
private:
    keyword_type _keyword;

public:
    token_keyword(keyword_type keyword) : token(token_type::KEYWORD), _keyword(keyword) {}
    token_keyword(const token_keyword& t) : token(token_type::KEYWORD), _keyword(t._keyword) {}
    token_keyword(token_keyword&& t) : token(token_type::KEYWORD), _keyword(t._keyword) {}

    token_keyword& operator=(const token& t) {
        token::operator=(t);
        try{
            const auto& t_k = dynamic_cast<const token_keyword&>(t);
            _keyword = t_k._keyword;
        }catch(std::bad_cast& err){
            std::cerr << err.what() << '\n';
            throw std::runtime_error("token_keyword = token\n");
        }

        return *this;
    }

    inline keyword_type get_keyword() const noexcept{return _keyword;}
};

class token_operator : public token{
private:
    operator_type _operator;

public:
    token_operator(operator_type op) : token(token_type::OPERATOR) , _operator(op){}
    token_operator(const token_operator& t) : token(token_type::OPERATOR) , _operator(t._operator){}
    token_operator(token_operator&& t) : token(token_type::OPERATOR) , _operator(t._operator){}

    token_operator& operator=(const token& t) {
        token::operator=(t);
        try{
            const auto& t_o = dynamic_cast<const token_operator&>(t);
            _operator = t_o._operator;
        }catch(std::bad_cast& err){
            std::cerr << err.what() << '\n';
            throw std::runtime_error("token_operator = token\n");
        }

        return *this;
    }

    inline operator_type get_operator() const noexcept{return _operator;}
};

class token_punctuation : public token{
private:
    punctuation_type _punctuation;

public:
    token_punctuation(punctuation_type punct) : token(token_type::PUNCTUATION), _punctuation(punct){}
    token_punctuation(const token_punctuation& t) : token(token_type::PUNCTUATION), _punctuation(t._punctuation){}
    token_punctuation(token_punctuation&& t) : token(token_type::PUNCTUATION), _punctuation(t._punctuation){}

    token_punctuation& operator=(const token& t) {
        token::operator=(t);
        try{
            const auto& t_p = dynamic_cast<const token_punctuation&>(t);
            _punctuation = t_p._punctuation;
        }catch(std::bad_cast& err){
            std::cerr << err.what() << '\n';
            throw std::runtime_error("token_punctuation = token\n");
        }

        return *this;
    }

    inline punctuation_type get_punctuation() const noexcept{return _punctuation;}
};

class token_constant : public token{
private:
    int _val;
public:
    token_constant(int val) : token(token_type::CONSTANT), _val(val){}

    inline int get_value() const noexcept {return _val;}
};

class token_identifier : public token{
private:
    int _id_idx;
public:
    token_identifier(const std::string& identifier);

    std::string get_identifier() const noexcept;
    inline int get_id_index() const noexcept{return _id_idx;}
};

class token_new_line : public token{
public:
    token_new_line() : token(token_type::NEW_LINE){}
};

class token_end : public token{
public:
    token_end() : token(token_type::END){}
};