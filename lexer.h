#include "enma_types.h"
#include <list>
#include <unordered_set>
#include <unordered_map>

class lexer{
private:
    static const std::unordered_set<std::string> _keywords;
    std::unordered_map<std::string, int> _identifiers;
    std::unordered_map<int, std::string> _identifier_codes;

    int read_number(const std::string& line, int& idx) const noexcept;
    int read_identifier(const std::string& line, int& idx) noexcept;
    bool process_line(const std::string& line, std::list<token_t>& tokens);
public:
    std::list<token_t> lexical_analysis(std::ifstream& file, bool& result);
    inline std::string get_identifier(int id) noexcept {return _identifier_codes[id];}
};