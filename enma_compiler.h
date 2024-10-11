#include <string>
#include "lexer.h"

class ENMA_compiler{
private:
    std::string _executable_name;
    lexer _lexer;
private:
    void debug_tokens(const class std::list<class token_t>& tokens);
    bool process_input_file(const std::string& filename);
public:
    ENMA_compiler();
    ~ENMA_compiler();

    bool process_arguments(int argc, char* argv[]);
};