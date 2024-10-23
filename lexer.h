#include <list>
#include <unordered_set>
#include <unordered_map>
#include <fstream>

class symbol_table{
private:
    std::unordered_map<std::string, int> _identifiers;
    std::unordered_map<int, std::string> _identifier_codes;
public:
    bool has_identifier(int id){
        return _identifier_codes.find(id) != _identifier_codes.end();
    }
    //check if identifier exists, return existing id code if it is or create a new one
    inline int try_set_identifier(const std::string& id) noexcept{
        if(_identifiers.find(id) != _identifiers.end()){
            return _identifiers[id];
        }else{
            _identifier_codes[_identifiers.size()] = id;
            return _identifiers[id] = _identifiers.size();
        }
    };
    inline int get_identifier(const std::string& id) const  {
        auto it = _identifiers.find(id);
        if(it != _identifiers.end())
            return it->second;
        throw std::runtime_error("identifier doesn't exist: " + id);
    }
    inline std::string get_identifier(int id) const {
        auto it = _identifier_codes.find(id);
        if(it != _identifier_codes.end())
            return it->second;
        throw std::runtime_error("identifier doesn't exist: " + std::to_string(id));
    }
};

class lexer{
private:
    static const std::unordered_set<std::string> _keywords;
    std::ifstream _file;

    int read_number(const std::string& line, int& idx) const noexcept;
    int read_identifier(const std::string& line, int& idx) noexcept;
    void process_line(const std::string& line, std::list<class token_t>& tokens);
public:
    lexer(const std::string& input_file);
    ~lexer();

    std::list<class token_t> lexical_analysis();
};