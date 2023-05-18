#pragma once

#include <bitset>
#include <vector>

namespace Pente
{
#define BOARD_SIZE 19
#define NEIGHBOUR_WINDOW 7
#define MOVEORDER_WINDOW 9

    using namespace std;

    /* Defining some variables (typedef) */
    typedef unsigned short ushort;
    using BitBoard = bitset<BOARD_SIZE * BOARD_SIZE>;
    using position = vector<int>;
    using moves = vector<position>;

    class Board
    {
    private:
        /* Defining two bit boards */
        /* _all_whites have all white pieces set(1) */
        /* _all_blacks have all black pieces set(1) */
        BitBoard _all_whites;
        BitBoard _all_blacks;

        /* Number of black and white pairs captured till now */
        int _blackCaptures;
        int _whiteCaptures;

        /* Defines the game end conditions */
        bool _gameEnd;
        int _gameWinner;

        /* Heuristics definition */
        static short _chainStructure[3];
        static short _distanceFallOff[MOVEORDER_WINDOW * MOVEORDER_WINDOW];

        struct cellEval
        {
            ushort closedFours;
            ushort openFours;
            ushort openThrees;
            ushort closedThrees;
            ushort closedTwos;
            ushort chainFives;

            cellEval();
            cellEval &
            operator+=(const cellEval &other);
        };

        /* Finds if [row,col] is a part of any of the openFours, closedFours, openThrees, closedThrees, closedTwos or chainFives */
        cellEval _getCellEval(int row, int col);

        /* Evaluates and assigns the placement score based on the parameters openFours, closedFours, openThrees, closedThrees, closedTwos, chainFives and the number of captures */
        int _getEvalScore(int player);

        /* Resolves the board and removes the opponent pieces from the board if there is a capture */
        void _resolve(int row, int col, int player);

        /* Returns all the possible positions neighbouring each pieces' radius of NEIGHBOUR_WINDOW */
        BitBoard _getPossibleMoves(bool specialSelect = false);

    public:
        Board();
        void Load(int player, int row, int col);
        void SetCaptures(int blackC, int whiteC);

        /* Finds the total number of pieces of a particular player on the board */
        int GetPieceCount(int player);

        /* Approximate Heuristic (Computationally efficient) */
        /* This is calculated after every action (Evaluates how good the placement is) */
        int Get_Heuristic_Approx(int player, int row, int col);

        /* End Game condition checked */
        bool IsOver();

        /* Hash function which helps in saving the board */
        unsigned long Hash() const;

        /* Heuristic that calculates the open and closed chains (More accurate) */
        int Evaluation_Heuristic(int player);

        /* Places the piece and resolves the board, if placement not possible, returns */
        void Place(int player, int row, int col);

        /* Finds the next possibilities for the move and places the player at those possible positions */
        /* Finds the approximate heuristic and sorts the nextMoves in an ascending or descending order based on if the player is a maximizer or a minimizer */
        moves Next_Possibilities_For_Move(int player, int maximizer);

        /* Operator definition for comparing the two boards */
        bool operator==(const Board &other) const;

        /* To display the board */
        void DisplayBoard(bool modernStyle = false);
    };

    /* Hashing and storing the board for efficient retrival */
    struct BoardHasher
    {
        unsigned long operator()(const Board &board) const;
    };
}