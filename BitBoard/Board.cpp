#include "Board.h"

#include <algorithm>
#include <iostream>
#include "Colors.h"

namespace Pente
{
/* Finding out the cell position in the one dimensional bit array */
#define convert2index(row, col) (row * BOARD_SIZE + col)

/* Checks if the bit is set at the row and col for the given board */
#define at(board, row, col) (((board) & (bitShift << convert2index(row, col))) != bitZero)

/* Sets the bit at the row and col for the given board */
#define place(board, row, col) ((board) |= (bitShift << convert2index(row, col)))

/* Resets the bit at the row and col for the given board */
#define remove(board, row, col) ((board) &= (~(bitShift << convert2index(row, col))))

    const BitBoard bitShift = 1;
    const BitBoard bitZero = 0;

    Board::cellEval::cellEval()
    {
        closedFours = 0;
        openFours = 0;
        openThrees = 0;
        closedThrees = 0;
        closedTwos = 0;
        chainFives = 0;
    }

    Board::cellEval &Board::cellEval::operator+=(const Board::cellEval &other)
    {
        closedFours += other.closedFours;
        openFours += other.openFours;
        closedThrees += other.closedThrees;
        openThrees += other.openThrees;
        closedTwos += other.closedTwos;
        chainFives += other.chainFives;

        return *this;
    }

    Board::cellEval Board::_getCellEval(int row, int col)
    {
        ushort lower_bound_x = max(0, row - 4);
        ushort upper_bound_x = min(BOARD_SIZE - 1, row + 4);

        ushort lower_bound_y = max(0, col - 4);
        ushort upper_bound_y = min(BOARD_SIZE - 1, col + 4);

        ushort cell_color = at(_all_blacks, row, col) ? colors::BLACK : colors::WHITE;
        BitBoard *playerGrid = cell_color == colors::BLACK ? &_all_blacks : &_all_whites;
        BitBoard *opponentGrid = cell_color == colors::BLACK ? &_all_whites : &_all_blacks;

        ushort consecutives[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* Boolean checking openness on either sides */
        bool opens[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        /* rowIncrementer and colIncrementer iterate over the 3*3 matrix to find the number of consecutive pieces of the player */
        int rowIncrementer[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
        int colIncrementer[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

        /* Represents the cell value that needs to be updated in consecutives */
        int consecutiveIndices[] = {0, 4, 2, 6, 0, 7, 3, 5, 1};

        /* 0-1 '\' , 2-3 '/' , 4-5 '|' , 6-7 '-' */
        for (int itr = 0; itr < 9; itr++)
        {
            if (itr == 4)
                continue;

            ushort row_bound = rowIncrementer[itr] == -1 ? lower_bound_x : upper_bound_x;
            ushort col_bound = colIncrementer[itr] == -1 ? lower_bound_y : upper_bound_y;

            auto rowCheck = [&row_bound, &rowIncrementer, &itr](int val)
            {
                if (rowIncrementer[itr] == -1)
                    return val >= row_bound;
                else
                    return val <= row_bound;
            };

            auto colCheck = [&col_bound, &colIncrementer, &itr](int val)
            {
                if (colIncrementer[itr] == -1)
                    return val >= col_bound;
                else
                    return val <= col_bound;
            };

            for (ushort r = row, c = col; rowCheck(r) && colCheck(c); r += rowIncrementer[itr], c += colIncrementer[itr])
            {
                if (at((*playerGrid), r, c))
                    consecutives[consecutiveIndices[itr]]++;
                else
                {
                    if (at((*opponentGrid), r, c))
                        opens[consecutiveIndices[itr]] = false;
                    else
                        opens[consecutiveIndices[itr]] = true;
                    break;
                }
            }
        }

        cellEval output;

        for (int inc = 0; inc < 8; inc += 2)
        {
            int together = consecutives[inc] + consecutives[inc + 1] - 1;
            switch (together)
            {
            case 3:
            {
                if ((opens[inc] && !opens[inc + 1]) || (!opens[inc] && opens[inc + 1]))
                    output.closedThrees++;
                else if (opens[inc] && opens[inc + 1])
                    output.openThrees++;

                break;
            }
            case 4:
            {
                if ((opens[inc] && !opens[inc + 1]) || (!opens[inc] && opens[inc + 1]))
                    output.closedFours++;
                else if (opens[inc] && opens[inc + 1])
                    output.openFours++;

                break;
            }
            case 2:
            {
                if ((opens[inc] && !opens[inc + 1]) || (!opens[inc] && opens[inc + 1]))
                    output.closedTwos++;
                break;
            }
            case 5:
            {
                output.chainFives++;
                break;
            }
            default:
                break;
            }
        }

        return output;
    }

    int Board::_getEvalScore(int player)
    {
        BitBoard *grid = player == colors::BLACK ? &_all_blacks : &_all_whites;
        cellEval answer;
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (at((*grid), i, j))
                    answer += _getCellEval(i, j);
            }
        }
        return ((player == colors::BLACK ? _blackCaptures : _whiteCaptures) * (-500) + (answer.openFours * 550) + (answer.closedFours * 400) + (answer.openThrees * 200) + (answer.closedThrees * 100) + (answer.closedTwos * (-50)) + (answer.chainFives * 10000));
    }

    void Board::_resolve(int row, int col, int player)
    {
        BitBoard *currentGrid = player == colors::BLACK ? &_all_blacks : &_all_whites;
        BitBoard *opponentGrid = player == colors::WHITE ? &_all_blacks : &_all_whites;

        if (_getCellEval(row, col).chainFives > 0)
        {
            _gameEnd = true;
            _gameWinner = player;
        }

        int rowIncrementer[] = {1, 1, 1, 0, 0, 0, -1, -1, -1};
        int colIncrementer[] = {1, 0, -1, 1, 0, -1, 1, 0, -1};

        int inc = 0;
        for (int i = -3; i <= 3; i += 3)
        {
            for (int j = -3; j <= 3; j += 3)
            {
                if (row + i >= 0 && row + i < BOARD_SIZE && col + j >= 0 && col + j < BOARD_SIZE)
                {
                    if (i != 0 || j != 0)
                    {
                        if (at((*currentGrid), (row + i), (col + j)))
                        {
                            int r1 = row + i + (rowIncrementer[inc] * 1);
                            int c1 = col + j + (colIncrementer[inc] * 1);
                            int r2 = row + i + (rowIncrementer[inc] * 2);
                            int c2 = col + j + (colIncrementer[inc] * 2);

                            if ((at((*opponentGrid), r1, c1)) && (at((*opponentGrid), r2, c2)))
                            {
                                if (player == colors::BLACK)
                                    _whiteCaptures++;
                                else
                                    _blackCaptures++;

                                remove((*opponentGrid), r1, c1);
                                remove((*opponentGrid), r2, c2);
                            }
                        }
                    }
                }
                inc++;
            }
        }

        if (_blackCaptures >= 5)
        {
            _gameEnd = true;
            _gameWinner = colors::WHITE;
        }
        else if (_whiteCaptures >= 5)
        {
            _gameEnd = true;
            _gameWinner = colors::BLACK;
        }
    }

    BitBoard Board::_getPossibleMoves(bool specialSelect)
    {
        BitBoard moves = 0;
        for (uint16_t row = 0; row < BOARD_SIZE; row++)
        {
            for (uint16_t col = 0; col < BOARD_SIZE; col++)
            {
                if (at(_all_blacks, row, col) || at(_all_whites, row, col))
                {
                    uint16_t minX = row - (NEIGHBOUR_WINDOW / 2) < 0 ? 0 : row - (NEIGHBOUR_WINDOW / 2);
                    uint16_t maxX = row + (NEIGHBOUR_WINDOW / 2) >= BOARD_SIZE ? BOARD_SIZE - 1 : row + (NEIGHBOUR_WINDOW / 2);

                    uint16_t minY = col - (NEIGHBOUR_WINDOW / 2) < 0 ? 0 : col - (NEIGHBOUR_WINDOW / 2);
                    uint16_t maxY = col + (NEIGHBOUR_WINDOW / 2) >= BOARD_SIZE ? BOARD_SIZE - 1 : col + (NEIGHBOUR_WINDOW / 2);

                    for (uint16_t r = minX; r <= maxX; r++)
                    {
                        for (uint16_t c = minY; c <= maxY; c++)
                        {
                            place(moves, r, c);
                        }
                    }
                }
            }
        }

        for (uint16_t row = 0; row < BOARD_SIZE; row++)
        {
            for (uint16_t col = 0; col < BOARD_SIZE; col++)
            {
                if (at(_all_blacks, row, col) || at(_all_whites, row, col))
                {
                    remove(moves, row, col);
                }
                else if (specialSelect && (row > 6 && row < 12) && (col > 6 && col < 12))
                {
                    remove(moves, row, col);
                }
            }
        }

        /*
        {
            for (short col = -1; col < 19; col++)
            {
                // First Column
                if (col == -1)
                {
                    cout << "   ";
                }
                else
                {

                    cout << col;
                    if (col >= 10)
                        cout << " ";
                    else
                        cout << "  ";
                }
            }
            cout << endl;
            for (short row = 0; row < 19; row++)
            {
                for (short col = -1; col < 19; col++)
                {
                    // First Column
                    if (col == -1)
                    {

                        int val = row;
                        cout << val;
                        if (val >= 10)
                            cout << " ";
                        else
                            cout << "  ";
                    }
                    else
                    {
                        if (at(moves, row, col))
                            cout << "1"
                                 << "  ";
                        else
                            cout << "."
                                 << "  ";
                    }
                }
                cout << endl;
            }
        }
        */

        return moves;
    }

    Board::Board() : _all_blacks(0), _all_whites(0), _blackCaptures(0), _whiteCaptures(0), _gameEnd(false), _gameWinner(-1) {}

    void Board::Load(int player, int row, int col)
    {
        if (player == colors::WHITE)
        {
            /* Sets the bit at _all_whites[row][col] = 1 */
            place(_all_whites, row, col);
        }
        if (player == colors::BLACK)
        {
            /* Sets the bit at _all_blacks[row][col] = 1 */
            place(_all_blacks, row, col);
        }
    }

    void Board::SetCaptures(int blackC, int whiteC)
    {
        _blackCaptures = blackC;
        _whiteCaptures = whiteC;
    }

    int Board::GetPieceCount(int player)
    {
        if (player == colors::WHITE)
            return _all_whites.count();
        else
            return _all_blacks.count();
    }

    int Board::Get_Heuristic_Approx(int player, int row, int col)
    {
        int BlackHeuristic = _blackCaptures * (-20);
        int WhiteHeuristic = _whiteCaptures * (-20);
        int range = (MOVEORDER_WINDOW - 1) / 2;
        for (int r = row - range; r <= (row + range); r++)
        {
            for (int c = col - range; c <= (col + range); c++)
            {
                if (r < 0 || c < 0 || r >= BOARD_SIZE || c >= BOARD_SIZE)
                    continue;
                int white_idx = 0;
                int black_idx = 0;
                if ((at(_all_whites, r, c)) || (at(_all_blacks, r, c)))
                {
                    if ((at(_all_whites, r, c)))
                        white_idx = 1;
                    else
                        white_idx = 2;

                    if ((at(_all_blacks, r, c)))
                        black_idx = 1;
                    else
                        black_idx = 2;
                }
                int i = (r - row) + range;
                int j = (c - col) + range;
                BlackHeuristic += _chainStructure[black_idx] * _distanceFallOff[i * MOVEORDER_WINDOW + j];
                WhiteHeuristic += _chainStructure[white_idx] * _distanceFallOff[i * MOVEORDER_WINDOW + j];
            }
        }

        if (player == colors::BLACK)
            return BlackHeuristic - WhiteHeuristic;
        else
            return WhiteHeuristic - BlackHeuristic;
    }

    bool Board::IsOver()
    {
        return _gameEnd;
    }

    unsigned long Board::Hash() const
    {
        const uint64_t fnvPrime = 1099511628211u;
        const uint64_t offsetBasis = 14695981039346656037u;

        uint64_t hash = offsetBasis;

        hash ^= std::hash<BitBoard>()(_all_whites) * fnvPrime;
        hash ^= std::hash<BitBoard>()(_all_blacks) * fnvPrime;
        hash ^= std::hash<int>()(_whiteCaptures) * fnvPrime;
        hash ^= std::hash<int>()(_blackCaptures) * fnvPrime;

        return static_cast<std::size_t>(hash);
    }

    int Board::Evaluation_Heuristic(int player)
    {
        int _blackHeuristic = _getEvalScore(colors::BLACK);
        int _whiteHeuristic = _getEvalScore(colors::WHITE);

        if (player == colors::BLACK)
            return _blackHeuristic - _whiteHeuristic;
        else
            return _whiteHeuristic - _blackHeuristic;
    }

    void Board::Place(int player, int row, int col)
    {
        if (at(_all_blacks, row, col) || at(_all_whites, row, col))
        {
            std::cout << "Already placed here" << std::endl;
            return;
        }

        if (player == colors::BLACK)
            place(_all_blacks, row, col);
        else
            place(_all_whites, row, col);

        _resolve(row, col, player);
    }

    moves Board::Next_Possibilities_For_Move(int player, int maximizer)
    {
        /* Special case for the starting few positions */
        bool specialSelect = false;
        if (player == colors::WHITE && GetPieceCount(player) == 1 && GetPieceCount(1 - player) == 1)
            specialSelect = true;

        BitBoard possibleMoves = _getPossibleMoves(specialSelect);
        moves nextMoves;
        for (ushort row = 0; row < BOARD_SIZE; row++)
        {
            for (ushort col = 0; col < BOARD_SIZE; col++)
            {
                if (at(possibleMoves, row, col))
                {
                    Board action = *this;
                    action.Place(player, row, col);
                    nextMoves.push_back({action.Get_Heuristic_Approx(maximizer, row, col), row, col});
                }
            }
        }

        std::sort(nextMoves.begin(), nextMoves.end(), [&player, &maximizer](const position &A, const position &B) -> bool
                  {
			if (player == maximizer)
				return A[0] > B[0];
			else
				return A[0] < B[0]; });

        return nextMoves;
    }

    bool Board::operator==(const Board &other) const
    {
        return _all_whites == other._all_whites &&
               _all_blacks == other._all_blacks &&
               _whiteCaptures == other._whiteCaptures &&
               _blackCaptures == other._blackCaptures;
    }

    void Board::DisplayBoard(bool modernStyle)
    {
        // First Row
        cout << "BLACK CAPTURES: " << _blackCaptures << "     WHITE CAPTURES: " << _whiteCaptures << endl;

        for (short col = -1; col < 19; col++)
        {
            // First Column
            if (col == -1)
            {
                cout << "   ";
            }
            else
            {
                if (modernStyle)
                {
                    char ch = col + 'A';
                    ch += ch >= 'I' ? 1 : 0;
                    cout << ch << "  ";
                }
                else
                {
                    cout << col;
                    if (col >= 10)
                        cout << " ";
                    else
                        cout << "  ";
                }
            }
        }
        cout << endl;
        for (short row = 0; row < 19; row++)
        {
            for (short col = -1; col < 19; col++)
            {
                // First Column
                if (col == -1)
                {

                    int val = row;
                    if (modernStyle)
                        val = (BOARD_SIZE - 1 - row) + 1;
                    cout << val;
                    if (val >= 10)
                        cout << " ";
                    else
                        cout << "  ";
                }
                else
                {
                    if (at(_all_whites, row, col))
                        cout << "w"
                             << "  ";
                    else if (at(_all_blacks, row, col))
                        cout << "b"
                             << "  ";
                    else
                        cout << "."
                             << "  ";
                }
            }
            cout << endl;
        }
    }

    short Board::_chainStructure[3] = {1, 2, 1};
    short Board::_distanceFallOff[MOVEORDER_WINDOW * MOVEORDER_WINDOW] =
        {
            9, 3, 6, 3, 9, 3, 6, 3, 9,
            3, 12, 6, 6, 12, 6, 6, 12, 3,
            6, 6, 12, 9, 18, 9, 12, 6, 6,
            3, 6, 9, 18, 18, 18, 12, 6, 3,
            9, 12, 18, 18, 0, 18, 18, 12, 9,
            3, 6, 9, 18, 18, 18, 12, 6, 3,
            6, 6, 12, 9, 18, 9, 12, 6, 6,
            3, 12, 6, 6, 12, 6, 6, 12, 3,
            9, 3, 6, 3, 9, 3, 6, 3, 9};

    unsigned long BoardHasher::operator()(const Board &board) const
    {
        return board.Hash();
    }
}
