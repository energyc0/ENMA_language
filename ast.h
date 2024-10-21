#include <memory>
#include "enma_types.h"

enum class ast_node_type{
    END,
    NUM,
    ID,
    ADD,
    SUB,
    DIV,
    MUL,
    PRINT,
    VAR_DECL,
    ASSIGN
};

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
    constexpr inline ast_node_type get_type()const noexcept{ return _type; }
};


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
};

class number_expression : public expression{
public:
    number_expression();
    number_expression(int num);
    inline void set_number(int num) noexcept {_val = num;}
    constexpr inline int get_number() const noexcept {return _val;}
};

class identifier_expression : public expression{
private:
    void check_for_validity() const;
public:
    identifier_expression();
    identifier_expression(int id);
    void set_id(int id);
    int get_id() const;
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

class binary_expression : public expression{
private:
    ast_node_type convert_operation(operator_type op) const;
public:
    binary_expression(operator_type op,
     const std::shared_ptr<expression>& left = std::shared_ptr<expression>(),
      const std::shared_ptr<expression>& right = std::shared_ptr<expression>());
    
    binary_expression(const binary_expression& expr);
    binary_expression(binary_expression&& expr);

    void set_left(const std::shared_ptr<expression>& expr);
    void set_right(const std::shared_ptr<expression>& expr);
    inline std::shared_ptr<expression> get_left() const noexcept{return std::static_pointer_cast<expression>(_left);}
    inline std::shared_ptr<expression> get_right() const noexcept{return std::static_pointer_cast<expression>(_right);}

    binary_expression& operator=(const binary_expression& expr);
};

class statement : public ast_node{
protected:
    void check_validity() const;

    statement(ast_node_type t, int val = 0,
     const std::shared_ptr<ast_node>& left = std::shared_ptr<ast_node>(),
      const std::shared_ptr<ast_node>& right = std::shared_ptr<ast_node>());

    statement(ast_node_type t, int val = 0,
     std::shared_ptr<ast_node>&& left = std::shared_ptr<ast_node>(),
      std::shared_ptr<ast_node>&& right = std::shared_ptr<ast_node>());
public:
    virtual ~statement(){}

    std::shared_ptr<statement> get_next() const noexcept{
        return std::static_pointer_cast<statement>(_right);
    }
    void set_next(const std::shared_ptr<statement>& next);

    //virtual int output_assembly() const = 0;
};

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

    //virtual int output_assembly() const;
};

class variable_declaration : public statement{
public:
//virtual int output_assembly() const{return 0;}
};