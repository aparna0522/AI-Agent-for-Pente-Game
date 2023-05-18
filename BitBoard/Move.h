#pragma once
#include <unordered_map>
#include "Board.h"

namespace Pente
{
#define MAX_HEIGHT 3
#define MAX_HASHSAVES 500

    using namespace std;
    class Board;
    struct BoardHasher;
    /* Min max algorithm with alpha beta pruning on the possible moves to find the best position for the AI player to ensure it wins */
    class Move
    {
    private:
        int _maxDepth;
        struct action
        {
            int result;
            int row;
            int col;
            int level;
        };

        /* Min Value is a minimizer for the alpha-beta pruning algorithm */
        action _minValue(int player, Board &board, int alpha, int beta, int depth);

        /* Max Value is a Maximizer for the alpha-beta pruning algorithm */
        action _maxValue(int player, Board &board, int alpha, int beta, int depth);

        unordered_map<Board, int, BoardHasher> _HashTable;
        Board *_frequencies;
        int _frequencies_incr;
        bool _frequencies_filled;

        /* To save the board states for efficient retrieval */
        void _loadTransposTable();
        void _saveTransposTable();

    public:
        Move(int depth);

        /* Save the computed board evaluations hash table, cached between two executions */
        ~Move();

        /* Finds the best Move using the alpha beta pruning algorithm */
        void Get_Best_Move(int player, Board &board, int &outRow, int &outCol);
    };
}