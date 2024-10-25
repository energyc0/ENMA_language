#include <memory>

class code_generator;
enum class operator_type;
enum class arithmetical_operation;

enum class ast_node_type{
    END,
    NUM,
    ID,
    ADD,
    SUB,
    DIV,
    MUL,
    EQUAL,
    NEQUAl,
    GREATER,
    GREATER_EQ,
    LESS,
    LESS_EQ,
    PRINT,
    VAR_DECL,
    ASSIGN
};

//value, left and right nodes are reserved
class ast_node{
protected:
    std::shared_ptr<ast_node> _left;
    std::shared_ptr<ast_node> _right;
    ast_node_type _type;
    int _val;
protected:
    ast_node(ast_node_type t, int val = 0,
     const std::shared_ptr<ast_node>& left = std::shared_ptr<ast_node>(),
      const std::shared_ptr<ast_node>& right = std::shared_ptr<ast_node>());

    ast_node(ast_node_type t, int val = 0,
     std::shared_ptr<ast_node>&& left = std::shared_ptr<ast_node>(),
      std::shared_ptr<ast_node>&& right = std::shared_ptr<ast_node>());
public:
    virtual ~ast_node();
    constexpr virtual inline ast_node_type get_type()const noexcept{ return _type; }

    virtual int accept_visitor(code_generator& visitor) const = 0;
};

//value, left and right nodes are reserved
class expression : public ast_node{
protected:
    expression(ast_node_type t, int val = 0,
     const std::shared_ptr<ast_node>& left = std::shared_ptr<ast_node>(),
      const std::shared_ptr<ast_node>& right = std::shared_ptr<ast_node>());

    expression(ast_node_type t, int val = 0,
     std::shared_ptr<ast_node>&& left = std::shared_ptr<ast_node>(),
      std::shared_ptr<ast_node>&& right = std::shared_ptr<ast_node>());

    expression(const expression& expr);
    expression(expression&& expr);
public:
    virtual ~expression();

    expression& operator=(const expression& expr);

    virtual int accept_visitor(code_generator& visitor) const = 0;
};

//value is used for constant storage, left and right nodes are reserved
class number_expression : public expression{
public:
    number_expression();
    number_expression(int num);
    inline void set_number(int num) noexcept {_val = num;}
    constexpr inline int get_number() const noexcept {return _val;}

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is used for identifier code, left and right nodes are reserved
class identifier_expression : public expression{
private:
    void check_for_validity() const;
public:
    identifier_expression();
    identifier_expression(int id);
    void set_id(int id);
    int get_id() const;

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};
/*
class assignment_expression : public expression{
public:
    assignment_expression();
    assignment_expression(const std::shared_ptr<expression>& lvalue, const std::shared_ptr<expression>& rvalue);
    void set_lvalue(const std::shared_ptr<expression>& expr);
    void set_rvalue(const std::shared_ptr<expression>& expr);
    inline std::shared_ptr<expression> get_lvalue() const noexcept{return _left;}
    inline std::shared_ptr<expression> get_rvalue() const noexcept{return _right;}
};
*/

//value is reserved, ast_node_type is for the arithmetical operation, left and right nodes are used for left and right expressions
class binary_expression : public expression{
private:
    ast_node_type convert_operation(arithmetical_operation op) const;
public:
    binary_expression(arithmetical_operation op,
     const std::shared_ptr<expression>& left = std::shared_ptr<expression>(),
      const std::shared_ptr<expression>& right = std::shared_ptr<expression>());
    
    binary_expression(const binary_expression& expr);
    binary_expression(binary_expression&& expr);

    void set_left(const std::shared_ptr<expression>& expr);
    void set_right(const std::shared_ptr<expression>& expr);
    inline std::shared_ptr<expression> get_left() const noexcept{return std::static_pointer_cast<expression>(_left);}
    inline std::shared_ptr<expression> get_right() const noexcept{return std::static_pointer_cast<expression>(_right);}

    binary_expression& operator=(const binary_expression& expr);
    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is reserved, left node is for the next node and right node is reserved
class statement : public ast_node{
protected:
    void check_validity() const;

    statement(ast_node_type t, int val = 0,
     const std::shared_ptr<ast_node>& left = std::shared_ptr<ast_node>(),
      const std::shared_ptr<ast_node>& right = std::shared_ptr<ast_node>());

    statement(ast_node_type t, int val,
     std::shared_ptr<ast_node>&& left,
      std::shared_ptr<ast_node>&& right);
public:
    virtual ~statement(){}

    virtual inline std::shared_ptr<statement> get_next() const noexcept{
        return std::static_pointer_cast<statement>(_right);
    }
    virtual inline void set_next(const std::shared_ptr<statement>& next) noexcept{
        _right = next;
    }

    virtual int accept_visitor(code_generator& visitor) const = 0;
};

//value is reserved, left node is for the expression and right node is for the next node
class print_statement : public statement{
public:
    print_statement(const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());
    print_statement(std::shared_ptr<expression>&& expr);

    print_statement(const print_statement& stat);
    print_statement(print_statement&& stat);

    void set_expression(const std::shared_ptr<expression>& expr);
    inline std::shared_ptr<expression> get_expression() const noexcept{
        return std::static_pointer_cast<expression>(_left);
    }

    print_statement& operator=(const print_statement& stat);

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is used for identifier code, left node is for the expression and right node is for the next
class variable_declaration : public statement{
private:
    bool _is_id_set;
public:
    variable_declaration() noexcept;
    //throw an std::runtime_error exception if an identifier doesn't exist
    variable_declaration(int id_code, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());
    //create a new one identifier in the global symbol table if the identifier doesn't exist
    variable_declaration(const std::string& id, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>()) noexcept;

    //set the code of an identifier, create a new one identifier if it doesn't exist 
    void try_set_identifier(const std::string& id) noexcept;
    std::string get_identifier() const;
    int get_identifier_code() const;

    void set_expression(const std::shared_ptr<expression>& expr);
    inline std::shared_ptr<expression> get_expression() const noexcept{
        return std::static_pointer_cast<expression>(_left);
    }

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is used for identifier code, left node is for the expression and right node is for the next
class assignment_statement : public statement{
private:
    bool _is_id_set;
public:
    assignment_statement() noexcept;
    //throw an std::runtime_error exception if an identifier doesn't exist
    assignment_statement(int id_code, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());
    //throw an std::runtime_error exception if an identifier doesn't exist
    assignment_statement(const std::string& id, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());

    //throw an std::runtime_error if the id doesn't exist
    void set_identifier(const std::string& id);
    //throw an std::runtime_error if the id is not set
    std::string get_identifier() const;
    //throw an std::runtime_error if the id is not set
    int get_identifier_code() const;


    //throw an std::runtime_error if expr == nullptr
    void set_expression(const std::shared_ptr<expression>& expr);
    inline std::shared_ptr<expression> get_expression() const noexcept{
        return std::static_pointer_cast<expression>(_left);
    }

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};