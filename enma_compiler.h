#include <string>
#include "parser.h"

enum class ast_node_type;

class ENMA_debugger{
private:
    static const char* reinterpret_arith_op(ast_node_type t);
public:
    static void debug_tokens(const class std::list<class std::shared_ptr<class token>>& tokens);
    static void debug_ast(const class std::shared_ptr<class ast_node>& node);
};

class ENMA_compiler{
private:
    std::string _executable_name;
    parser _parser;
    bool _is_verbose;

private:
    bool process_input_file(const std::string& filename);
public:
    ENMA_compiler(const char* exe_name, bool is_verbose);
    ~ENMA_compiler();

    bool process_input(const class std::vector<const char*>& args);
};