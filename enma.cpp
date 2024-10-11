#include <iostream>
#include "enma_compiler.h"

int main(int argc, char* argv[]){
    ENMA_compiler compiler;

    if(compiler.process_arguments(argc, argv)){
        return 0;
    }else{
        std::cout << "compilation terminated.\n";
        return 1;
    }
}