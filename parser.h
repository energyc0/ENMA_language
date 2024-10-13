#include <memory>
#include <list>

class parsing_error {
private:
    std::string _msg;
    int _line;
    int _token;
public:
    explicit parsing_error(const char* msg, int line, int token) noexcept;

    std::string what();
};

class token_storage{
private:
    std::list<class token_t>& _tokens;
    std::list<class token_t>::iterator _iter;
    int _line_number = 1;
    int _token_number = 0;

    void skip_new_lines();
public:
    token_storage(std::list<token_t>& tokens);

    constexpr inline int get_token_number()const noexcept{return _token_number;}
    constexpr inline int get_line_number()const noexcept{return _line_number;}

    token_t get_current() noexcept;
    token_t get_next() noexcept ;
    void next() noexcept;
};

enum class ast_node_type_e{
    END,
    NUM,
    ADD,
    SUB,
    DIV,
    MUL
};

class ast_node_t{
public:
    std::shared_ptr<ast_node_t> left;
    std::shared_ptr<ast_node_t> right;
    ast_node_type_e type;
    int val;

    ast_node_t();
    ast_node_t(int _val, ast_node_type_e _t,
     const std::shared_ptr<ast_node_t>& _left, const std::shared_ptr<ast_node_t>& _right);

    ~ast_node_t();

    int interpret_node();
};

class parser{
private:
    token_storage* _tokens = nullptr;

private:
    ast_node_type_e reinterpret_arith_op(const token_t& t);
    int get_arith_op_precedence(ast_node_type_e op);
    std::shared_ptr<ast_node_t> get_primary_expr();
    std::shared_ptr<ast_node_t> bin_expr_parse(int prev_op_precedence);
public:
    std::shared_ptr<ast_node_t> binary_expr(token_storage& tokens, bool& result);
};