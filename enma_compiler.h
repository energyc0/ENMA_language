#include <string>
#include "lexer.h"
#include "parser.h"

class ENMA_compiler{
private:
    std::string _executable_name;
    lexer _lexer;
    parser _parser;
private:
    void debug_tokens(const class std::list<class token_t>& tokens);
    void debug_ast(const class ast_node_t* node);
    bool process_input_file(const std::string& filename);
public:
    ENMA_compiler(const char* exe_name);
    ~ENMA_compiler();

    bool process_input(const class std::vector<const char*>& args);
};