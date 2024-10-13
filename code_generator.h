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
    static constexpr int _registers_count = 14;
    static constexpr std::array<const char*, _registers_count> _registers = {
        "rax", "rbx", "rdx", "rsi", "rdi", "rsp", "rbp",
        "r8","r9","r10","r12","r13","r14","r15"
    };
    std::ofstream* _file;
    std::array<bool, 14> _free_reg;

    void output_preamble();
    void free_reg(int reg);
    int find_free_reg();
    void mov_reg(int reg, int val);
    void add_reg(int left, int right);
    void sub_reg(int left, int right);
    void mul_reg(int left, int right);
    void div_reg(int left, int right);
    void output_ending();
public:
    code_generator() : _free_reg({}), _file(nullptr) {}
    bool generate_code(typename std::ofstream& file, class std::shared_ptr<class ast_node_t>& root);
    ~code_generator(){}
};