#!/bin/bash

#Compiling program 1 (agent1)
echo "Compiling Program 1 (agent 1)"
cd BitBoard
g++ -std=c++11 -c Board.cpp Move.cpp main.cpp 
g++ -O3 -o bitboard.out Board.o main.o Move.o
cp bitboard.out ../agent1.out
cd ..
echo "Compilaition Done"

#Compiling program 2 (agent2)
echo "Compiling Program 2 (agent 2)"
cd CharBoard
g++ -std=c++11 -c main.cpp 
g++ -O3 -o charboard.out main.o
cp charboard.out ../agent2.out
cd ..
echo "Compilaition Done"

#Compiling the resolution program
echo "Compiling Resolve Board"
g++ -std=c++11 ResolveBoard.cpp -o resolver.out
echo "Compilaition Done"

cp InitialBoard.txt input.txt 

status=0;

echo "Playing the first move"
while [ $status -ne 1 ]
do
    # Run program A and resolve the board to give as input to program B
    outputA=$("./agent1.out");
    ./resolver.out "input.txt" ${outputA} "input.txt";
    status=$?;
    
    # If board is complete (Someone won) status will be 1 and thus exit the loop
    if [ $status -ne 0 ]
    then
        break;
    fi

    # Run program B and resolve the board to give as input to program A
    outputB=$("./agent2.out");
    ./resolver.out "input.txt" ${outputB} "input.txt";
    status=$?;
done