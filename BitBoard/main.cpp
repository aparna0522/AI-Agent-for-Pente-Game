#include <iostream>
#include "Board.h"
#include "Move.h"
#include "Utils.h"
using namespace std;

int main()
{
    Pente::Board board;
    int AIplayer;

    /* Parse input from the input file and load the initial state of the board */
    Utils::LoadInputFile("input.txt", board, AIplayer);

    Pente::Move move(MAX_HEIGHT);
    int row, col;

    move.Get_Best_Move(AIplayer, board, row, col);

    cout << row << "," << col << endl;

    return 0;
}