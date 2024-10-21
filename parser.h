#include <memory>
#include <list>
#include "ast.h"

class token_storage{
private:
    std::list<class token_t>& _tokens;
    std::list<class token_t>::iterator _iter;
    int _line_number = 1;
    int _token_number = 0;
    int _prev_line = 1;
    int _prev_token = 1;
    void skip_new_lines();
public:
    token_storage(std::list<token_t>& tokens);

    constexpr inline int get_token_number() const noexcept{return _prev_token;}
    constexpr inline int get_line_number() const noexcept{return _prev_line;}

    token_t get_current() noexcept;
    token_t get_next() noexcept ;
    void next() noexcept;
};

class parsing_error {
private:
    std::string _msg;
    int _line;
    int _token;
public:
    explicit parsing_error(const char* msg, const token_storage& storage) noexcept;

    std::string what();
};

class parser{
private:
    token_storage* _tokens = nullptr;

private:
    operator_type reinterpret_arith_op(const class token_t& t);
    int get_arith_op_precedence(operator_type op);
    std::shared_ptr<expression> get_primary_expr();
    std::shared_ptr<expression> bin_expr_parse(int prev_op_precedence);
    std::shared_ptr<expression> binary_expr();
    std::shared_ptr<print_statement> parse_print();
public:
    std::shared_ptr<statement> generate_ast(token_storage& tokens, bool& result);
};