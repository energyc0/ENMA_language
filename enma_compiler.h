#include <string>
#include "lexer.h"
#include "parser.h"

class ENMA_debugger{
private:
    static char reinterpret_arith_op(ast_node_type t);
public:
    static void debug_tokens(const class std::list<class token_t>& tokens);
    static void debug_ast(const class std::shared_ptr<class ast_node>& node);
};

class ENMA_compiler{
private:
    std::string _executable_name;
    lexer _lexer;
    parser _parser;

private:
    bool process_input_file(const std::string& filename);
public:
    ENMA_compiler(const char* exe_name);
    ~ENMA_compiler();

    bool process_input(const class std::vector<const char*>& args);
};