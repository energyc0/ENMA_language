#include <iostream>
#include <cstring>
#include <vector>
#include "enma_compiler.h"

int main(int argc, char* argv[]){
    if(argc == 1){
        std::cout << "Please, enter input files\nuse --help to show commands.";
        return 1;
    }else if(argc == 2 && strcmp(argv[1], "--help") == 0){
        std::cout.width(10);
        std::cout << std::left << "Usage:\n" << 
        "enma [options] <path-to-source-files>\n" << 
        "Options\n" <<
        "-o <executable-name>\tto specify the executable name\n";
        return 0;
    }

    std::vector<const char*> input_files;
    int exe_name_idx = -1;
    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-o") == 0){
            if(i + 1 >= argc){
                std::cerr << "Please, enter the name of the executable.\n";
                return 1;
            }
            exe_name_idx = ++i;
        }else{
            input_files.push_back(argv[i]);
        }
    }
    ENMA_compiler compiler(exe_name_idx == -1 ? "output" : argv[exe_name_idx]);

    if(compiler.process_input(input_files)){
        return 0;
    }else{
        std::cerr << "compilation terminated.\n";
        return 1;
    }
}