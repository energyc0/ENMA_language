#!/bin/bash

cd build/
cmake ..
make

echo ""
for test in ../tests/*.em
    do
    test="${test%.em}"
    ./enma "${test}.em"
    
    ./output > result
    if ! cmp "${test}_res" result -s 
        then
            echo "${test#../tests/*} - FAILED"
        else
            echo "${test#../tests/*} - success"
        fi
    done
    rm -f result ../tests/*.asm output output.o


