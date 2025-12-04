#!/bin/bash

g++ -c src/day04.c -o day04.o

if [ $? -eq 0 ]; 
then
        g++ day04.o

        ./a.out
else 
        echo "Compilation falied"
fi

