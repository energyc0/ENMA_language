#include <list>
#include <unordered_set>
#include <unordered_map>

class symbol_table{
private:
    std::unordered_map<std::string, int> _identifiers;
    std::unordered_map<int, std::string> _identifier_codes;
public:
    bool has_identifier(int id){
        return _identifier_codes.find(id) != _identifier_codes.end();
    }
    inline int try_set_identifier(const std::string& id) noexcept{
        if(_identifiers.find(id) != _identifiers.end()){
            return _identifiers[id];
        }else{
            _identifier_codes[_identifiers.size()] = id;
            return _identifiers[id] = _identifiers.size();
        }
    };
    inline int get_identifier(const std::string& id) noexcept {
        if(_identifiers.find(id) == _identifiers.end())
            return -1;
        return _identifiers[id];
    }
    inline std::string get_identifier(int id) noexcept {
        return _identifier_codes[id];
    }
};

class lexer{
private:
    static const std::unordered_set<std::string> _keywords;

    int read_number(const std::string& line, int& idx) const noexcept;
    int read_identifier(const std::string& line, int& idx) noexcept;
    bool process_line(const std::string& line, std::list<class token_t>& tokens);
public:
    std::list<class token_t> lexical_analysis(std::ifstream& file, bool& result);
};