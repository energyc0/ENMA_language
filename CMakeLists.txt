cmake_minimum_required(VERSION 3.22 )

project("ENMA" 
LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_COMPILER g++)
add_compile_options("-g")

add_executable("enma" 
"enma_compiler.h"
"enma_compiler.cpp"
"lexer.h"
"lexer.cpp"
"token_types.h"
"token_types.cpp"
"parser.h"
"parser.cpp"
"code_generator.h"
"code_generator.cpp"
"ast.h"
"ast.cpp"
"enma.cpp")