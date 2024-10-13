#include <memory>
#include <list>

class token_storage{
private:
    std::list<token_t>& _tokens;
    std::list<token_t>::iterator _iter;
public:
    token_storage(std::list<token_t>& tokens) : _tokens(tokens), _iter(tokens.begin()) {}

    inline token_t get_current() noexcept{
        if(_iter == _tokens.end()){
            return token_t(token_type_e::END, 0);
        }
        return *_iter;
    }
    inline token_t get_next() noexcept {
        if(_iter == _tokens.end() || ++_iter == _tokens.end()){
            return token_t(token_type_e::END, 0);
        }
        return *_iter;
    }
    inline void next() noexcept{
        if(_iter == _tokens.end()){
            return;
        }
        ++_iter;
    }
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

    ast_node_t(): val(0), left(nullptr), right(nullptr), type(ast_node_type_e::END) {}

    ast_node_t(int _val, ast_node_type_e _t, ast_node_t* _left, ast_node_t* _right) :
     val(_val), left(_left), right(_right), type(_t) {}

    ~ast_node_t() {
        if(left)
            left.reset();
        if(right)
            right.reset();
    }

    int interpret_node(){
        switch(type){
            case ast_node_type_e::NUM:  return val;
            case ast_node_type_e::ADD:  return left->interpret_node() + right->interpret_node();
            case ast_node_type_e::SUB:  return left->interpret_node() - right->interpret_node();
            case ast_node_type_e::DIV:  return left->interpret_node() / right->interpret_node();
            case ast_node_type_e::MUL:  return left->interpret_node() * right->interpret_node();
            default:
                std::cerr<<"undefined ast_node type\n";
                break;
        }
        return -1;
    }
};

class parser{
private:
    static ast_node_type_e reinterpret_arith_op(const token_t& t);
    static int get_arith_op_precedence(ast_node_type_e op);
    ast_node_t* get_primary_expr(token_storage& tokens);
    ast_node_t* bin_expr_parse(token_storage& tokens, int prev_op_precedence);
public:
    std::shared_ptr<ast_node_t> binary_expr(token_storage& tokens, bool& result);
};