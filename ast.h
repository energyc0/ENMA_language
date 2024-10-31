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
    ASSIGN,
    COMPOUND,
    IF_HEAD,
    WHILE_LOOP,
    FOR_LOOP
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

//value is reserved, right node is for the next node and left node is reserved
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

//value is reserved, right node is for the next node and left node is for the inner statement
class compound_statement : public statement{
public:
    compound_statement(const std::shared_ptr<statement>& inner_stat = std::shared_ptr<statement>(),
     const std::shared_ptr<statement>& next = std::shared_ptr<statement>());
    compound_statement(std::shared_ptr<statement>&& inner_stat,
     std::shared_ptr<statement>&& next);

    compound_statement(const compound_statement& stat);
    compound_statement(compound_statement&& stat);

    inline void set_inner_statement(const std::shared_ptr<statement>& inner_stat) noexcept{
        _left = std::static_pointer_cast<ast_node>(inner_stat);
    }
    inline std::shared_ptr<statement> get_inner_statement() const noexcept{
        return std::static_pointer_cast<statement>(_left);
    }

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
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

//value is a number of the if statement,
//left node is for a conditional expression node,
//right node is for a next statement
//2 additional nodes: if and else compound statements
class if_statement : public statement{
private:
    std::shared_ptr<compound_statement> _if_stat;
    std::shared_ptr<compound_statement> _else_stat;
public:
    if_statement(const std::shared_ptr<expression>& cond = std::shared_ptr<expression>(),
     const std::shared_ptr<statement>& next = std::shared_ptr<statement>(),
     const std::shared_ptr<compound_statement>& if_inner_stat = std::shared_ptr<compound_statement>(),
     const std::shared_ptr<compound_statement>& else_inner_stat = std::shared_ptr<compound_statement>());
    if_statement(std::shared_ptr<expression>&& cond,
      std::shared_ptr<statement>&& next,
      std::shared_ptr<compound_statement>&& if_inner_stat,
      std::shared_ptr<compound_statement>&& else_inner_stat);
    if_statement(const if_statement& stat);
    if_statement(if_statement&& stat);

    inline void set_conditional_expression(const std::shared_ptr<expression>& cond) noexcept{
        _left = cond;
    }
    inline std::shared_ptr<expression> get_conditional_expression() const noexcept{
        return std::static_pointer_cast<expression>(_left);
    }
    inline void set_if_inner_statement(const std::shared_ptr<compound_statement>& stat) noexcept{
        _if_stat = stat;
    }
    inline std::shared_ptr<compound_statement> get_if_inner_statement() const noexcept{
        return _if_stat;
    };
    inline void set_else_inner_statement(const std::shared_ptr<compound_statement>& stat) noexcept{
        _else_stat = stat;
    }
    inline std::shared_ptr<compound_statement> get_else_inner_statement() const noexcept{
        return _else_stat;
    };

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is used for identifier, right node is for the next node and left node is for the expression
class statement_with_id : public statement{
protected:
    bool _is_id_set;
protected:
    statement_with_id(ast_node_type t);
    statement_with_id(ast_node_type t, int id_code, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());
    statement_with_id(ast_node_type t, const std::string& id, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>()) noexcept;
    statement_with_id(ast_node_type t, int id_code, std::shared_ptr<expression>&& expr);
    statement_with_id(ast_node_type t, const std::string& id, std::shared_ptr<expression>&& expr) noexcept;

public:
    virtual ~statement_with_id(){}

    virtual void set_identifier(const std::string& id);
    virtual std::string get_identifier() const;
    virtual int get_identifier_code() const;

    //throw an std::runtime_error if expr == nullptr
    virtual void set_expression(const std::shared_ptr<expression>& expr);
    virtual inline std::shared_ptr<expression> get_expression() const noexcept{
        return std::static_pointer_cast<expression>(_left);
    }
};

//value is used for identifier code, left node is for the expression and right node is for the next
class variable_declaration : public statement_with_id{
public:
    variable_declaration() noexcept;
    //throw an std::runtime_error exception if an identifier doesn't exist
    variable_declaration(int id_code, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());
    //create a new one identifier in the global symbol table if the identifier doesn't exist
    variable_declaration(const std::string& id, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>()) noexcept;

    //set the code of an identifier, create a new one identifier if it doesn't exist 

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is used for identifier code, left node is for the expression and right node is for the next
//this statement can be without any expression and consist of only an identifier
class assignment_statement : public statement_with_id{
public:
    assignment_statement() noexcept;
    //throw an std::runtime_error exception if an identifier doesn't exist
    assignment_statement(int id_code, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());
    //throw an std::runtime_error exception if an identifier doesn't exist
    assignment_statement(const std::string& id, const std::shared_ptr<expression>& expr = std::shared_ptr<expression>());

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is reserved,
//left node is for a conditional expression and right node is for the next
//1 additional node for an inner statement
class while_statement : public statement{
private:
    std::shared_ptr<compound_statement> _inner_stat;

public:
    while_statement(const std::shared_ptr<expression>& expr = std::shared_ptr<expression>(),
    const std::shared_ptr<statement>& next = std::shared_ptr<statement>(),
     const std::shared_ptr<compound_statement>& inner_stat = std::shared_ptr<compound_statement>());
    while_statement(std::shared_ptr<expression>&& expr,
    std::shared_ptr<statement>&& next,
     std::shared_ptr<compound_statement>&& inner_stat);
    while_statement(const while_statement& stat);
    while_statement(while_statement&& stat);

    inline void set_conditional_expression(const std::shared_ptr<expression>& expr)noexcept{
        _left = expr;
    }
    inline std::shared_ptr<expression> get_conditional_expression() const noexcept{
        return std::static_pointer_cast<expression>(_left);
    }
    inline void set_inner_statement(const std::shared_ptr<compound_statement>& stat) noexcept{
        _inner_stat = stat;
    }
    inline std::shared_ptr<compound_statement> get_inner_statement() const noexcept{
        return _inner_stat;
    };

    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};

//value is reserved,
//left node is for a start statement and right node is for the next node
//3 additional nodes for a final expression, an iteration expression and an inner statement
class for_statement : public statement{
private:
    //assignment statement after iteration
    std::shared_ptr<expression> _expr_after_iter;
    std::shared_ptr<expression> _final_expr;
    std::shared_ptr<compound_statement> _inner_stat;

    void check_validity() const;
public:
    for_statement(const std::shared_ptr<statement_with_id>& start_stat = nullptr,
    const std::shared_ptr<statement>& next = nullptr,
    const std::shared_ptr<expression>& final_expr = nullptr,
    const std::shared_ptr<expression>& stat_after_iter = nullptr,
    const std::shared_ptr<compound_statement>& inner_stat = nullptr);

    for_statement(std::shared_ptr<statement_with_id>&& start_stat,
    std::shared_ptr<statement>&& next,
    std::shared_ptr<expression>&& final_expr,
    std::shared_ptr<expression>&& expr_after_iter,
     std::shared_ptr<compound_statement>&& inner_stat);

    for_statement(const for_statement& stat);
    for_statement(for_statement&& stat);

    inline void set_final_expression(const std::shared_ptr<expression>& expr)noexcept{
        _final_expr = expr;
    }
    inline std::shared_ptr<expression> get_final_expression() const noexcept{
        return _final_expr;
    }

    inline void set_start_statement(const std::shared_ptr<statement_with_id>& stat){
        check_validity();
        _left = stat;
    }
    inline std::shared_ptr<statement_with_id> get_start_statement() const noexcept{
        return std::static_pointer_cast<statement_with_id>(_left);
    }

    inline void set_inner_statement(const std::shared_ptr<compound_statement>& stat) noexcept{
        _inner_stat = stat;
    }
    inline std::shared_ptr<compound_statement> get_inner_statement() const noexcept{
        return _inner_stat;
    };

    inline void set_after_iter_expression(const std::shared_ptr<expression>& stat) noexcept{
        _expr_after_iter = stat;
    }
    inline std::shared_ptr<expression> get_after_iter_expression() const noexcept{
        return _expr_after_iter;
    };
    virtual int accept_visitor(code_generator& visitor) const;
    friend code_generator;
};