#include <list>
#include <unordered_set>
#include <unordered_map>
#include <fstream>

class symbol_table{
private:
    std::unordered_map<std::string, int> _identifiers;
    std::unordered_map<int, std::string> _identifier_codes;
public:
    inline bool has_identifier(int id) const noexcept{
        return _identifier_codes.find(id) != _identifier_codes.end();
    }
    inline bool has_identifier(const std::string& id) const noexcept{
        return _identifiers.find(id) != _identifiers.end();
    }
    //check if an identifier exists, return the existing id code if it is or create a new one
    inline int try_set_identifier(const std::string& id) noexcept{
        if(_identifiers.find(id) != _identifiers.end()){
            return _identifiers[id];
        }else{
            _identifier_codes[_identifiers.size()] = id;
            return _identifiers[id] = _identifiers.size();
        }
    };
    //return an identifier code and throw an exception if the identifier doesn't exist
    inline int get_identifier(const std::string& id) const  {
        auto it = _identifiers.find(id);
        if(it != _identifiers.end())
            return it->second;
        throw std::runtime_error("identifier doesn't exist: " + id);
    }
    //return an identifier and throw an exception if the identifier doesn't exist
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
    std::list<class std::shared_ptr<class token>> _tokens;

    int read_number(const std::string& line, int& idx, bool is_negative) const noexcept;
    std::string read_identifier(const std::string& line, int& idx) noexcept;

    void emplace_identifier(const std::string& line, int& idx) noexcept;

    void process_line(const std::string& line);
public:
    lexer(const std::string& input_file);
    ~lexer();

    std::list<class std::shared_ptr<class token>> lexical_analysis();
};