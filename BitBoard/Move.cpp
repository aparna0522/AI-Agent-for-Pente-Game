#include "Move.h"
#include "Colors.h"

#include <fstream>
#include <vector>

namespace Pente
{
    using position = vector<int>;
    using moves = vector<position>;

    Move::action Move::_minValue(int player, Board &board, int alpha, int beta, int depth)
    {
        if (depth >= _maxDepth)
        {
            int evaluation;
            if (_HashTable.find(board) != _HashTable.end())
            {
                evaluation = _HashTable[board];
                _frequencies[_frequencies_incr] = board;
                _frequencies_incr = (_frequencies_incr + 1) % MAX_HASHSAVES;
                if (_frequencies_incr == 0)
                    _frequencies_filled = true;
            }
            else
                evaluation = _HashTable[board] = board.Evaluation_Heuristic(1 - player);
            return {evaluation, -1, -1, 0};
        }
        int value = INT_MAX;
        int minHeight = INT_MAX;
        int row = -1;
        int col = -1;
        moves nextActions = board.Next_Possibilities_For_Move(player, 1 - player);

        for (int i = 0; i < nextActions.size(); i++)
        {
            Board child = board;
            child.Place(player, nextActions[i][1], nextActions[i][2]);
            int res, res_height;

            if (child.IsOver())
            {
                res = -10000;
                res_height = -1;
            }
            else
            {
                action result = _maxValue(1 - player, child, alpha, beta, depth + 1);
                res = result.result;
                res_height = result.level;
            }
            if (res < value)
            {
                value = res;
                row = nextActions[i][1];
                col = nextActions[i][2];
                minHeight = res_height;
            }
            else if (res == value && minHeight > res_height)
            {
                value = res;
                row = nextActions[i][1];
                col = nextActions[i][2];
                minHeight = res_height;
            }
            if (value <= alpha)
            {
                return {value, row, col, minHeight + 1};
            }
            beta = beta > value ? value : beta;
        }
        return {value, row, col, minHeight + 1};
    }

    Move::action Move::_maxValue(int player, Board &board, int alpha, int beta, int depth)
    {
        if (depth >= _maxDepth)
        {
            int evaluation;
            if (_HashTable.find(board) != _HashTable.end())
            {
                evaluation = _HashTable[board];
                _frequencies[_frequencies_incr] = board;
                _frequencies_incr = (_frequencies_incr + 1) % MAX_HASHSAVES;
                if (_frequencies_incr == 0)
                    _frequencies_filled = true;
            }
            else
                evaluation = _HashTable[board] = board.Evaluation_Heuristic(player);
            return {evaluation, -1, -1, 0};
        }
        int value = INT_MIN;
        int minHeight = INT_MAX;
        int row = -1;
        int col = -1;
        moves nextActions = board.Next_Possibilities_For_Move(player, player);

        for (int i = 0; i < nextActions.size(); i++)
        {
            Board child = board;
            child.Place(player, nextActions[i][1], nextActions[i][2]);
            int res, res_height;

            if (child.IsOver())
            {
                res = 10000;
                res_height = -1;
            }
            else
            {
                action result = _minValue(1 - player, child, alpha, beta, depth + 1);
                res = result.result;
                res_height = result.level;
            }
            if (res > value)
            {
                value = res;
                row = nextActions[i][1];
                col = nextActions[i][2];
                minHeight = res_height;
            }
            else if (res == value && minHeight > res_height)
            {
                value = res;
                row = nextActions[i][1];
                col = nextActions[i][2];
                minHeight = res_height;
            }
            if (value >= beta)
            {
                return {value, row, col, minHeight + 1};
            }
            alpha = alpha < value ? value : alpha;
        }
        return {value, row, col, minHeight + 1};
    }

    void Move::_loadTransposTable()
    {
        std::ifstream playdata("playerdata.txt", std::ios::binary);
        if (playdata.is_open())
        {

            size_t numElements;
            playdata.read(reinterpret_cast<char *>(&numElements), sizeof(numElements));

            for (size_t i = 0; i < numElements; ++i)
            {
                Board board;
                playdata.read(reinterpret_cast<char *>(&board), sizeof(board));

                int value;
                playdata.read(reinterpret_cast<char *>(&value), sizeof(value));

                _HashTable[board] = value;
            }

            playdata.close();
        }
    }

    void Move::_saveTransposTable()
    {
        std::ofstream playerdata("playerdata.txt", std::ios::binary);
        if (playerdata.is_open())
        {
            size_t numElements;
            if (_frequencies_filled)
                numElements = MAX_HASHSAVES;
            else
                numElements = _frequencies_incr;

            playerdata.write(reinterpret_cast<const char *>(&numElements), sizeof(numElements));

            for (size_t inc = 0; inc <= numElements; inc++)
            {
                const auto &entry = _frequencies[inc];
                playerdata.write(reinterpret_cast<const char *>(&entry), sizeof(entry));
                playerdata.write(reinterpret_cast<const char *>(&_HashTable[entry]), sizeof(_HashTable[entry]));
            }

            playerdata.close();
        }
    }

    Move::Move(int depth) : _maxDepth(depth), _frequencies_incr(0), _frequencies_filled(false)
    {
        _frequencies = new Board[MAX_HASHSAVES];
        _loadTransposTable();
    }

    Move::~Move()
    {
        _saveTransposTable();
        delete (_frequencies);
    }

    void Move::Get_Best_Move(int player, Board &board, int &outRow, int &outCol)
    {
        if (player == colors::WHITE && (board.GetPieceCount(player) + board.GetPieceCount(1 - player)) == 0)
        {
            outRow = 9;
            outCol = 9;
            return;
        }

        action nextMove = _maxValue(player, board, INT_MIN, INT_MAX, 0);
        outRow = nextMove.row;
        outCol = nextMove.col;
    }
}