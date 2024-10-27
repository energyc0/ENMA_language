#include <array>
#include <memory>
#include <fstream>
#include <vector>

class code_register;

class unhandled_register_error{
private:
    std::string _msg;
public:
    unhandled_register_error(const char* msg, const code_register& reg);
    std::string what();
};

class code_register {
private:
    const char* _name;
    bool _is_busy;
public:
    constexpr code_register(const char* name) : _name(name), _is_busy(false){}

    constexpr inline const char* get_name() const noexcept{return _name;}
    constexpr inline bool is_busy() const noexcept{return _is_busy;}

    inline void become_busy(){
        if(_is_busy)
            throw unhandled_register_error("register is busy already", *this);
        _is_busy = true;
    }
    inline void become_free(){
        if(!_is_busy)
            throw unhandled_register_error("register is free already", *this);
        _is_busy = false;
    }
};

class some_variable{
private:
    int _id_code;
    const std::string _name;
public:
    some_variable(const std::string& name);
    some_variable(std::string&& name);
    inline int get_id_code() const noexcept{return _id_code;}
    inline std::string get_asm_name() const noexcept{return _name;}

    friend bool operator<(const some_variable& a, const some_variable& b){
        return a._id_code < b._id_code;
    }
};

class code_generator{
private:
//rcx, r11
    static constexpr int _registers_count = 7;
    std::array<code_register, _registers_count> _registers = {
        "r8","r9","r10","r12","r13","r14","r15"
    };
    std::ofstream _file;

    std::vector<some_variable> _variables;
private:
    template<class T, class... arg>
    void check_valid_storage(T a, arg ...args) const{
        check_valid_storage(a);
        check_valid_storage(args...);
    }

    inline void check_valid_storage(const code_register& reg) const{
        if(!reg.is_busy())
            throw unhandled_register_error("undefined storage", reg);
    }

    void output_preamble();

    int find_free_reg();
    int mov_reg(int reg, int val);
    int mov_reg_var(int reg, const class identifier_expression* expr);

    int add_reg(int left, int right);
    int sub_reg(int left, int right);
    int mul_reg(int left, int right);
    int div_reg(int left, int right);

    int equal_reg(int left, int right);    
    int nequal_reg(int left, int right);
    int greater_reg(int left, int right);
    int greater_equal_reg(int left, int right);
    int less_reg(int left, int right);
    int less_equal_reg(int left, int right);

    void print_reg(int reg);
    void declare_variable(const class variable_declaration* stat);
    void assign_to_variable(const class assignment_statement* stat);

    void output_postamble();
    void output_variables();

    int node_interaction(const class number_expression* expr);
    int node_interaction(const class identifier_expression* expr);
    int node_interaction(const class binary_expression* expr);
    void node_interaction(const class print_statement* stat);
    void node_interaction(const class assignment_statement* stat);
    void node_interaction(const class variable_declaration* stat);
    void node_interaction(const class compound_statement* stat);
    void node_interaction(const class if_statement* stat);

    friend class number_expression;
    friend class identifier_expression;
    friend class binary_expression;
    friend class print_statement;
    friend class variable_declaration;
    friend class assignment_statement;
    friend class compound_statement;
    friend class if_statement;
public:
    code_generator(const std::string& output_filename);

    bool generate_code(const std::shared_ptr<class statement>& root);

    ~code_generator();
};