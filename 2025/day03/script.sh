#!/bin/bash

g++ -c src/day03.cpp -o day03.o

if [ $? -eq 0 ]; 
then
        g++ day03.o

        ./a.out
else 
        echo "Compilation falied"
fi

