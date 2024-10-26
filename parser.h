#include <memory>
#include <list>
#include <unordered_set>

class token_storage{
private:
    std::list<std::shared_ptr<class token>>& _tokens;
    std::list<std::shared_ptr<class token>>::iterator _iter;
    int _line_number = 1;
    int _token_number = 0;
    int _prev_line = 1;
    int _prev_token = 1;
    void skip_new_lines();
public:
    token_storage(std::list<std::shared_ptr<class token>>& tokens);

    constexpr inline int get_token_number() const noexcept{return _prev_token;}
    constexpr inline int get_line_number() const noexcept{return _prev_line;}

    std::shared_ptr<class token> get_current() noexcept;
    std::shared_ptr<class token> get_next() noexcept ;
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

enum class arithmetical_operation{
    GREATER,
    GREATER_EQ,
    LESS,
    LESS_EQ,
    EQUAL,
    NEQUAL,
    ADD,
    SUB,
    DIV,
    MUL,
    END_EXPR
};

class parser{
private:
    token_storage* _tokens = nullptr;
    //declared identifiers id to check if a variable is not declared
    std::unordered_set<int> _declared_identifiers;
private:
    arithmetical_operation reinterpret_arith_op(const class std::shared_ptr<class token>& t);
    int get_arith_op_precedence(arithmetical_operation op);
    std::shared_ptr<class expression> get_primary_expr();
    std::shared_ptr<class expression> bin_expr_parse(int prev_op_precedence);
    std::shared_ptr<class expression> binary_expr();
    std::shared_ptr<class print_statement> parse_print();
    std::shared_ptr<class variable_declaration> parse_variable_declaration();
    std::shared_ptr<class assignment_statement> parse_assignment_statement();
    std::shared_ptr<class compound_statement> expect_compound_statement();
    std::shared_ptr<class statement> parse_statement(const class std::shared_ptr<class token>& t);
    std::shared_ptr<class statement> expect_statement();
public:
    std::shared_ptr<class statement> generate_ast(token_storage& tokens, bool& result);
};