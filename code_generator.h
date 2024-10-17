#include <array>
#include <memory>
#include <fstream>

class unhandled_register_error{
private:
    std::string _msg;
public:
    unhandled_register_error(const char* msg, const char* reg);
    std::string what();
};

class code_generator{
private:
//rcx, r11
    static constexpr int _registers_count = 7;
    static constexpr std::array<const char*, _registers_count> _registers = {
        "r8","r9","r10","r12","r13","r14","r15"
    };
    std::ofstream* _file;
    std::array<bool, _registers_count> _free_reg;

    void output_preamble();
    void free_reg(int reg);
    int find_free_reg();
    int mov_reg(int reg, int val);
    int add_reg(int left, int right);
    int sub_reg(int left, int right);
    int mul_reg(int left, int right);
    int div_reg(int left, int right);
    void print_value(int val);
    void output_ending();

    int traverse_node(class std::shared_ptr<class ast_node_t>& node);
public:
    code_generator() : _file(nullptr) {_free_reg.fill(true);}
    bool generate_code(typename std::ofstream& file, class std::shared_ptr<class ast_node_t>& root);
    ~code_generator(){}
};