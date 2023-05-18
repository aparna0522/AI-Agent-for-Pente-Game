#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <algorithm>
using namespace std;

#define MAXDEPTH 3
#define BOARD_SIZE 19

typedef pair<int, int> positions;
typedef vector<char> rowSequence;
typedef vector<vector<char>> Board;
typedef pair<rowSequence, pair<int, int>> key;
map<key, int> lookupTable;
int whitePieces = 0, blackPieces = 0;

struct nextMove
{
    int bestValue;
    vector<int> row;
    vector<int> col;
    int currHeight;
};

vector<int> maxConsecutives(const vector<char> &line, const char &letter, bool checkSpecialPatterns)
{
    int current_counter = 0;
    int closed2s = 0, closed3s = 0, closed4s = 0, open3s = 0, open4s = 0, pente = 0;

    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] == letter)
        {
            current_counter++;
        }
        else
        {
            if (current_counter == 2)
            {
                if ((line[i] == '.' && (i - 3 >= 0 && line[i - 3] != '.')) || ((line[i] != '.' && (i - 3 >= 0 && line[i - 3] == '.'))))
                    closed2s++;
            }
            else if (current_counter == 3)
            {
                if ((line[i] == '.' && (i - 4 >= 0 && line[i - 4] != '.')) || ((line[i] != '.' && (i - 4 >= 0 && line[i - 4] == '.'))))
                    closed3s++;
                else if ((line[i] == '.' && (i - 4 >= 0 && line[i - 4] == '.')))
                    open3s++;
            }
            else if (current_counter == 4)
            {
                if ((line[i] == '.' && (i - 5 >= 0 && line[i - 5] != '.')) || ((line[i] != '.' && (i - 5 >= 0 && line[i - 5] == '.'))))
                    closed4s++;
                else if ((line[i] == '.' && (i - 5 >= 0 && line[i - 5] == '.')))
                    open4s++;
            }
            else if (current_counter == 5)
            {
                pente++;
            }
            current_counter = 0;
        }
    }

    return {
        closed2s,
        closed3s,
        open3s,
        closed4s,
        open4s,
        pente};
}

vector<vector<pair<int, int>>> preComputeDiagonals = {
    {{18, 0}},
    {{17, 0}, {18, 1}},
    {{16, 0}, {17, 1}, {18, 2}},
    {{15, 0}, {16, 1}, {17, 2}, {18, 3}},
    {{14, 0}, {15, 1}, {16, 2}, {17, 3}, {18, 4}},
    {{13, 0}, {14, 1}, {15, 2}, {16, 3}, {17, 4}, {18, 5}},
    {{12, 0}, {13, 1}, {14, 2}, {15, 3}, {16, 4}, {17, 5}, {18, 6}},
    {{11, 0}, {12, 1}, {13, 2}, {14, 3}, {15, 4}, {16, 5}, {17, 6}, {18, 7}},
    {{10, 0}, {11, 1}, {12, 2}, {13, 3}, {14, 4}, {15, 5}, {16, 6}, {17, 7}, {18, 8}},
    {{9, 0}, {10, 1}, {11, 2}, {12, 3}, {13, 4}, {14, 5}, {15, 6}, {16, 7}, {17, 8}, {18, 9}},
    {{8, 0}, {9, 1}, {10, 2}, {11, 3}, {12, 4}, {13, 5}, {14, 6}, {15, 7}, {16, 8}, {17, 9}, {18, 10}},
    {{7, 0}, {8, 1}, {9, 2}, {10, 3}, {11, 4}, {12, 5}, {13, 6}, {14, 7}, {15, 8}, {16, 9}, {17, 10}, {18, 11}},
    {{6, 0}, {7, 1}, {8, 2}, {9, 3}, {10, 4}, {11, 5}, {12, 6}, {13, 7}, {14, 8}, {15, 9}, {16, 10}, {17, 11}, {18, 12}},
    {{5, 0}, {6, 1}, {7, 2}, {8, 3}, {9, 4}, {10, 5}, {11, 6}, {12, 7}, {13, 8}, {14, 9}, {15, 10}, {16, 11}, {17, 12}, {18, 13}},
    {{4, 0}, {5, 1}, {6, 2}, {7, 3}, {8, 4}, {9, 5}, {10, 6}, {11, 7}, {12, 8}, {13, 9}, {14, 10}, {15, 11}, {16, 12}, {17, 13}, {18, 14}},
    {{3, 0}, {4, 1}, {5, 2}, {6, 3}, {7, 4}, {8, 5}, {9, 6}, {10, 7}, {11, 8}, {12, 9}, {13, 10}, {14, 11}, {15, 12}, {16, 13}, {17, 14}, {18, 15}},
    {{2, 0}, {3, 1}, {4, 2}, {5, 3}, {6, 4}, {7, 5}, {8, 6}, {9, 7}, {10, 8}, {11, 9}, {12, 10}, {13, 11}, {14, 12}, {15, 13}, {16, 14}, {17, 15}, {18, 16}},
    {{1, 0}, {2, 1}, {3, 2}, {4, 3}, {5, 4}, {6, 5}, {7, 6}, {8, 7}, {9, 8}, {10, 9}, {11, 10}, {12, 11}, {13, 12}, {14, 13}, {15, 14}, {16, 15}, {17, 16}, {18, 17}},
    {{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}, {10, 10}, {11, 11}, {12, 12}, {13, 13}, {14, 14}, {15, 15}, {16, 16}, {17, 17}, {18, 18}},
    {{18, 18}, {17, 17}, {16, 16}, {15, 15}, {14, 14}, {13, 13}, {12, 12}, {11, 11}, {10, 10}, {9, 9}, {8, 8}, {7, 7}, {6, 6}, {5, 5}, {4, 4}, {3, 3}, {2, 2}, {1, 1}, {0, 0}},
    {{17, 18}, {16, 17}, {15, 16}, {14, 15}, {13, 14}, {12, 13}, {11, 12}, {10, 11}, {9, 10}, {8, 9}, {7, 8}, {6, 7}, {5, 6}, {4, 5}, {3, 4}, {2, 3}, {1, 2}, {0, 1}},
    {{16, 18}, {15, 17}, {14, 16}, {13, 15}, {12, 14}, {11, 13}, {10, 12}, {9, 11}, {8, 10}, {7, 9}, {6, 8}, {5, 7}, {4, 6}, {3, 5}, {2, 4}, {1, 3}, {0, 2}},
    {{15, 18}, {14, 17}, {13, 16}, {12, 15}, {11, 14}, {10, 13}, {9, 12}, {8, 11}, {7, 10}, {6, 9}, {5, 8}, {4, 7}, {3, 6}, {2, 5}, {1, 4}, {0, 3}},
    {{14, 18}, {13, 17}, {12, 16}, {11, 15}, {10, 14}, {9, 13}, {8, 12}, {7, 11}, {6, 10}, {5, 9}, {4, 8}, {3, 7}, {2, 6}, {1, 5}, {0, 4}},
    {{13, 18}, {12, 17}, {11, 16}, {10, 15}, {9, 14}, {8, 13}, {7, 12}, {6, 11}, {5, 10}, {4, 9}, {3, 8}, {2, 7}, {1, 6}, {0, 5}},
    {{12, 18}, {11, 17}, {10, 16}, {9, 15}, {8, 14}, {7, 13}, {6, 12}, {5, 11}, {4, 10}, {3, 9}, {2, 8}, {1, 7}, {0, 6}},
    {{11, 18}, {10, 17}, {9, 16}, {8, 15}, {7, 14}, {6, 13}, {5, 12}, {4, 11}, {3, 10}, {2, 9}, {1, 8}, {0, 7}},
    {{10, 18}, {9, 17}, {8, 16}, {7, 15}, {6, 14}, {5, 13}, {4, 12}, {3, 11}, {2, 10}, {1, 9}, {0, 8}},
    {{9, 18}, {8, 17}, {7, 16}, {6, 15}, {5, 14}, {4, 13}, {3, 12}, {2, 11}, {1, 10}, {0, 9}},
    {{8, 18}, {7, 17}, {6, 16}, {5, 15}, {4, 14}, {3, 13}, {2, 12}, {1, 11}, {0, 10}},
    {{7, 18}, {6, 17}, {5, 16}, {4, 15}, {3, 14}, {2, 13}, {1, 12}, {0, 11}},
    {{6, 18}, {5, 17}, {4, 16}, {3, 15}, {2, 14}, {1, 13}, {0, 12}},
    {{5, 18}, {4, 17}, {3, 16}, {2, 15}, {1, 14}, {0, 13}},
    {{4, 18}, {3, 17}, {2, 16}, {1, 15}, {0, 14}},
    {{3, 18}, {2, 17}, {1, 16}, {0, 15}},
    {{2, 18}, {1, 17}, {0, 16}},
    {{1, 18}, {0, 17}},
    {{0, 18}}};

int getHeuristicApprox(Board &board, int row, int col, char player, int stones)
{
    vector<int> chainWeights(3);
    vector<vector<int>> distanceCost(9, vector<int>(9, 0));
    distanceCost = {
        {3, 1, 2, 1, 3, 1, 2, 1, 3},
        {1, 6, 2, 2, 6, 2, 2, 6, 1},
        {2, 2, 8, 3, 8, 3, 8, 2, 2},
        {1, 2, 3, 8, 8, 8, 3, 2, 1},
        {3, 6, 8, 8, 0, 8, 8, 6, 3},
        {1, 2, 3, 8, 8, 8, 3, 2, 1},
        {2, 2, 8, 3, 8, 3, 8, 2, 2},
        {1, 6, 2, 2, 6, 2, 2, 6, 1},
        {3, 1, 2, 1, 3, 1, 2, 1, 3}};
    chainWeights = {1, 2, 1};
    int eval = 0;
    int mid = (distanceCost[0].size() - 1) / 2;

    for (int i = row - mid; i <= row + mid; i++)
    {
        for (int j = col - mid; j <= col + mid; j++)
        {
            if (j > -1 && i > -1 && i < BOARD_SIZE && j < BOARD_SIZE)
            {
                // 0 - Null, 1 - White, 2 - Black
                int index;
                if (board[i][j] == '.')
                    index = 0;
                else if (board[i][j] == player)
                    index = 1;
                else
                    index = 2;
                eval += chainWeights[index] * distanceCost[(i - row) + mid][(j - col) + mid];
            }
        }
    }
    return eval - (stones * 20);
}

int evaluationHeuristic(Board &board, char stone, int stoneCaptures)
{
    int closed2s = 0, closed3s = 0, closed4s = 0, open3s = 0, open4s = 0, pente = 0;

    bool checkSpecialPatterns = false;
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[i][j] != '.')
                count++;
            if (count >= 6)
            {
                checkSpecialPatterns = true;
                break;
            }
        }
    }

    // Horizontal
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        vector<int> results = maxConsecutives(board[i], stone, checkSpecialPatterns);
        closed2s += results[0];
        closed3s += results[1];
        open3s += results[2];
        closed4s += results[3];
        open4s += results[4];
        pente += results[5];
    }

    // Vertical
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        vector<char> row;
        bool consecutiveCheck = false;
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (board[j][i] != '.')
                consecutiveCheck = true;
            row.push_back(board[j][i]);
        }

        if (consecutiveCheck)
        {
            vector<int> results = maxConsecutives(row, stone, checkSpecialPatterns);
            closed2s += results[0];
            closed3s += results[1];
            open3s += results[2];
            closed4s += results[3];
            open4s += results[4];
            pente += results[5];
        }
    }

    // Diagonals
    for (auto &lines : preComputeDiagonals)
    {
        vector<char> check;
        bool consecutiveCheck = false;
        for (auto &pos : lines)
        {
            if (board[pos.first][pos.second] != '.')
                consecutiveCheck = true;
            check.push_back(board[pos.first][pos.second]);
        }

        if (consecutiveCheck)
        {
            vector<int> results = maxConsecutives(check, stone, checkSpecialPatterns);
            closed2s += results[0];
            closed3s += results[1];
            open3s += results[2];
            closed4s += results[3];
            open4s += results[4];
            pente += results[5];
        }
    }

    int evaluation = 0;
    if (pente <= 0)
        evaluation = (stoneCaptures * (-200)) + (closed2s * (-20)) + (closed3s * 40) + (open3s * 100) + (closed4s * 150) + (open4s * 250) + (pente * 10000);
    else
        evaluation = 10000;
    return evaluation;
}

bool resolveBoard(Board &board, int &whiteCapture, int &blackCapture, int row, int col)
{
    if (row < 0 || col < 0)
        return false;
    char inserted = board[row][col];
    if (row - 3 >= 0 && col - 3 >= 0 && board[row - 3][col - 3] == inserted)
    {
        int r = row - 2, c = col - 2;
        bool captured = true;
        for (; r < row && c < col; r++, c++)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row - 2, c = col - 2;
            for (; r < row && c < col; r++, c++)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }
    if (row - 3 >= 0 && board[row - 3][col] == inserted)
    {
        int r = row - 2, c = col;
        bool captured = true;
        for (; r < row; r++)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row - 2, c = col;
            for (; r < row; r++)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }
    if (row - 3 >= 0 && col + 3 < board.size() && board[row - 3][col + 3] == inserted)
    {
        int r = row - 2, c = col + 2;
        bool captured = true;
        for (; r < row && c > col; r++, c--)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row - 2, c = col + 2;
            for (; r < row && c > col; r++, c--)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }
    if (col - 3 >= 0 && board[row][col - 3] == inserted)
    {
        int r = row, c = col - 2;
        bool captured = true;
        for (; c < col; c++)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row, c = col - 2;
            for (; c < col; c++)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }
    if (col + 3 < board.size() && board[row][col + 3] == inserted)
    {
        int r = row, c = col + 2;
        bool captured = true;
        for (; c > col; c--)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row, c = col + 2;
            for (; c > col; c--)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }
    if (row + 3 < board.size() && col - 3 >= 0 && board[row + 3][col - 3] == inserted)
    {
        int r = row + 2, c = col - 2;
        bool captured = true;
        for (; r > row && c < col; r--, c++)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row + 2, c = col - 2;
            for (; r > row && c < col; r--, c++)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }
    if (row + 3 < board.size() && board[row + 3][col] == inserted)
    {
        int r = row + 2, c = col;
        bool captured = true;
        for (; r > row; r--)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row + 2, c = col;
            for (; r > row; r--)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }
    if (row + 3 < board.size() && col + 3 < board.size() && board[row + 3][col + 3] == inserted)
    {
        int r = row + 2, c = col + 2;
        bool captured = true;
        for (; r > row && c > col; r--, c--)
        {
            if (board[r][c] != '.' && board[r][c] != inserted)
                continue;
            else
            {
                captured = false;
                break;
            }
        }
        if (captured)
        {
            r = row + 2, c = col + 2;
            for (; r > row && c > col; r--, c--)
            {
                board[r][c] = '.';
            }
            if (inserted == 'w')
                blackCapture++;
            else
                whiteCapture++;
        }
    }

    vector<char> checkPente;
    int lowerBound_row = max(row - 4, 0);
    int upperBound_row = min(row + 4, BOARD_SIZE - 1);

    int lowerBound_col = max(col - 4, 0);
    int upperBound_col = min(col + 4, BOARD_SIZE - 1);

    for (int i = lowerBound_row, j = lowerBound_col; j <= upperBound_col && i <= upperBound_row; i++, j++)
    {
        checkPente.push_back(board[i][j]);
    }

    vector<int> results = maxConsecutives(checkPente, board[row][col], false);
    if (results[5] > 0)
        return true;

    checkPente.clear();
    for (int i = lowerBound_row, j = upperBound_col; j >= lowerBound_col && i <= upperBound_row; i++, j--)
    {
        checkPente.push_back(board[i][j]);
    }

    results = maxConsecutives(checkPente, board[row][col], false);
    if (results[5] > 0)
        return true;

    checkPente.clear();
    for (int i = 0; i < BOARD_SIZE - 1; i++)
    {
        checkPente.push_back(board[i][col]);
    }

    results = maxConsecutives(checkPente, board[row][col], false);
    if (results[5] > 0)
        return true;

    results = maxConsecutives(board[row], board[row][col], false);
    if (results[5] > 0)
        return true;

    if ((whiteCapture >= 5 && board[row][col] == 'b') || (blackCapture >= 5 && board[row][col] == 'w'))
        return true;

    return false;
}

void nextPossibilitiesForMove(Board &board, positions playerPos, set<positions> &neighbours)
{
    if (neighbours.find(playerPos) != neighbours.end())
    {
        neighbours.erase(playerPos);
    }
    for (int i = -3; i <= 3; i++)
    {
        for (int j = -3; j <= 3; j++)
        {
            if (playerPos.first + i < 0 || playerPos.first + i >= 18 || playerPos.second + j < 0 || playerPos.second + j >= 18)
                continue;
            if (i == 0 && j == 0)
                continue;
            if (board[playerPos.first + i][playerPos.second + j] == '.')
                neighbours.insert({playerPos.first + i, playerPos.second + j});
        }
    }
}

set<positions> getNeighbours(Board &board)
{
    set<positions> neighbours;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            if (board[i][j] != '.')
                nextPossibilitiesForMove(board, {i, j}, neighbours);
        }
    }
    if (whitePieces == 1 && blackPieces == 1)
    {
        for (int i = 7; i <= 11; i++)
        {
            for (int j = 7; j <= 11; j++)
            {
                if (neighbours.find({i, j}) != neighbours.end())
                    neighbours.erase({i, j});
            }
        }
    }
    return neighbours;
}

vector<int> getPiecesOnBoard(Board &board)
{
    int getWhitePieces = 0, getBlackPieces = 0;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            if (board[i][j] == 'w')
                getWhitePieces++;
            else if (board[i][j] == 'b')
                getBlackPieces++;
        }
    }
    return {getWhitePieces, getBlackPieces};
}

nextMove maxValue(int player, Board &board, int alpha, int beta, int height, int whiteCaptures, int blackCaptures, vector<int> rows, vector<int> cols);

nextMove minValue(int player, Board &board, int alpha, int beta, int height, int whiteCaptures, int blackCaptures, vector<int> rows, vector<int> cols)
{
    char playerColor = player == 0 ? 'b' : 'w';
    char opponentColor = player == 0 ? 'w' : 'b';

    vector<char> rowSeq;
    for (int i = 0; i < rows.size(); i++)
    {
        rowSeq.push_back(rows[i] * BOARD_SIZE + cols[i]);
    }
    sort(rowSeq.begin(), rowSeq.end());
    if (height >= MAXDEPTH)
    {
        int returnAnswer;
        if (lookupTable.find({rowSeq, {whiteCaptures, blackCaptures}}) != lookupTable.end())
            returnAnswer = lookupTable[{rowSeq, {whiteCaptures, blackCaptures}}];
        else
        {
            int playerStones = player == 1 ? blackCaptures : whiteCaptures;
            int playerHeuristic = evaluationHeuristic(board, opponentColor, playerStones);

            int opponentStones = player == 1 ? whiteCaptures : blackCaptures;
            int opponentHeuristic = evaluationHeuristic(board, playerColor, opponentStones);
            returnAnswer = lookupTable[{rowSeq, {whiteCaptures, blackCaptures}}] = playerHeuristic - opponentHeuristic;
        }
        return {returnAnswer, {}, {}, 0};
    }

    vector<vector<int>> possibleMoves;
    set<positions> neighbours = getNeighbours(board);

    for (auto &x : neighbours)
    {
        Board newBoard = board;
        newBoard[x.first][x.second] = playerColor;
        int newBoardWC = whiteCaptures;
        int newBoardBC = blackCaptures;
        resolveBoard(newBoard, newBoardWC, newBoardBC, x.first, x.second);

        int playerStones = player == 1 ? newBoardBC : newBoardWC;
        int playerHeuristic = getHeuristicApprox(newBoard, x.first, x.second, playerColor, playerStones);

        int opponentStones = player == 1 ? newBoardWC : newBoardBC;
        int opponentHeuristic = getHeuristicApprox(newBoard, x.first, x.second, opponentColor, opponentStones);

        possibleMoves.push_back({playerHeuristic - opponentHeuristic, x.first, x.second});
    }

    std::sort(possibleMoves.begin(), possibleMoves.end(), [](const vector<int> &A, const vector<int> &B) -> bool
              { return A[0] < B[0]; });

    int value = INT_MAX;
    int bestHeight = INT_MAX;
    nextMove bestAction;
    nextMove temp;
    int row = 0, col = 0;
    for (auto &c : possibleMoves)
    {
        Board newBoard = board;
        newBoard[c[1]][c[2]] = playerColor;
        int newBoardWC = whiteCaptures;
        int newBoardBC = blackCaptures;
        bool gameComplete = resolveBoard(newBoard, newBoardWC, newBoardBC, c[1], c[2]);
        int testing_value;
        int answerHeight;
        if (gameComplete)
        {
            testing_value = -10000;
            answerHeight = -1;
        }
        else
        {
            nextMove maxAnswer = maxValue(1 - player, newBoard, alpha, beta, height + 1, newBoardWC, newBoardBC, rows, cols);
            testing_value = maxAnswer.bestValue;
            answerHeight = maxAnswer.currHeight;
            temp = maxAnswer;
        }
        if (value > testing_value)
        {
            bestAction = temp;
            bestAction.row.insert(bestAction.row.begin(), c[1]);
            bestAction.col.insert(bestAction.col.begin(), c[2]);

            value = testing_value;
            row = c[1];
            col = c[2];
            bestHeight = answerHeight;
        }
        else if (value == testing_value && bestHeight > answerHeight)
        {
            bestAction = temp;
            bestAction.row.insert(bestAction.row.begin(), c[1]);
            bestAction.col.insert(bestAction.col.begin(), c[2]);

            value = testing_value;
            row = c[1];
            col = c[2];
            bestHeight = answerHeight;
        }
        if (value <= alpha)
            return bestAction;
        beta = min(beta, value);
    }
    return bestAction;
}

nextMove maxValue(int player, Board &board, int alpha, int beta, int height, int whiteCaptures, int blackCaptures, vector<int> rows, vector<int> cols)
{
    char playerColor = player == 0 ? 'b' : 'w';
    char opponentColor = player == 0 ? 'w' : 'b';

    vector<char> rowSeq;

    for (int i = 0; i < rows.size(); i++)
    {
        rowSeq.push_back(rows[i] * BOARD_SIZE + cols[i]);
    }

    sort(rowSeq.begin(), rowSeq.end());

    if (height >= MAXDEPTH)
    {
        int returnAnswer;
        if (lookupTable.find({rowSeq, {whiteCaptures, blackCaptures}}) != lookupTable.end())
            returnAnswer = lookupTable[{rowSeq, {whiteCaptures, blackCaptures}}];
        else
        {
            int playerStones = player == 0 ? blackCaptures : whiteCaptures;
            int playerHeuristic = evaluationHeuristic(board, playerColor, playerStones);

            int opponentStones = player == 0 ? whiteCaptures : blackCaptures;
            int opponentHeuristic = evaluationHeuristic(board, opponentColor, opponentStones);
            returnAnswer = lookupTable[{rowSeq, {whiteCaptures, blackCaptures}}] = playerHeuristic - opponentHeuristic;
        }
        return {returnAnswer, {}, {}, 0};
    }

    vector<vector<int>> possibleMoves;
    set<positions> neighbours = getNeighbours(board);
    for (auto &x : neighbours)
    {
        Board newBoard = board;
        newBoard[x.first][x.second] = playerColor;
        int newBoardWC = whiteCaptures;
        int newBoardBC = blackCaptures;
        resolveBoard(newBoard, newBoardWC, newBoardBC, x.first, x.second);

        int playerStones = player == 0 ? newBoardBC : newBoardWC;
        int playerHeuristic = getHeuristicApprox(newBoard, x.first, x.second, playerColor, playerStones);

        int opponentStones = player == 0 ? newBoardWC : newBoardBC;
        int opponentHeuristic = getHeuristicApprox(newBoard, x.first, x.second, opponentColor, opponentStones);

        possibleMoves.push_back({playerHeuristic - opponentHeuristic, x.first, x.second});
    }

    std::sort(possibleMoves.begin(), possibleMoves.end(), [](const vector<int> &A, const vector<int> &B) -> bool
              { return A[0] > B[0]; });

    int value = INT_MIN;
    int bestHeight = INT_MAX;
    int row = 0, col = 0;
    nextMove bestAction;

    for (auto &c : possibleMoves)
    {
        Board newBoard = board;
        newBoard[c[1]][c[2]] = playerColor;
        int newBoardWC = whiteCaptures;
        int newBoardBC = blackCaptures;
        bool gameComplete = resolveBoard(newBoard, newBoardWC, newBoardBC, c[1], c[2]);
        int testing_value;
        int answerHeight;
        nextMove temp;

        if (gameComplete)
        {
            testing_value = 10000;
            answerHeight = -1;
        }
        else
        {
            vector<int> r = rows;
            r.push_back(c[1]);
            vector<int> column = cols;
            column.push_back(c[2]);

            nextMove minAnswer = minValue(1 - player, newBoard, alpha, beta, height + 1, newBoardWC, newBoardBC, r, column);
            testing_value = minAnswer.bestValue;
            answerHeight = minAnswer.currHeight;
            temp = minAnswer;
        }
        if (value < testing_value)
        {
            bestAction = temp;
            bestAction.row.insert(bestAction.row.begin(), c[1]);
            bestAction.col.insert(bestAction.col.begin(), c[2]);
            value = testing_value;
            row = c[1];
            col = c[2];
            bestHeight = answerHeight;
        }
        else if (value == testing_value && bestHeight > answerHeight)
        {
            bestAction = temp;
            bestAction.row.insert(bestAction.row.begin(), c[1]);
            bestAction.col.insert(bestAction.col.begin(), c[2]);
            value = testing_value;
            row = c[1];
            col = c[2];
            bestHeight = answerHeight;
        }

        if (value >= beta)
            return bestAction;
        alpha = max(alpha, value);
    }
    return bestAction;
}

nextMove alpha_beta_search(Board board, int player, int whiteCaptures, int blackCaptures)
{
    vector<int> pieceCount = getPiecesOnBoard(board);
    if (player == 1 && pieceCount[0] == 0 && pieceCount[1] == 0)
    {
        nextMove bestValue;
        bestValue.bestValue = 0;
        bestValue.row.push_back(9);
        bestValue.col.push_back(9);
        bestValue.currHeight = -1;
        return bestValue;
    }
    blackPieces = pieceCount[1];
    whitePieces = pieceCount[0];
    nextMove bestValue = maxValue(player, board, INT_MIN, INT_MAX, 0, whiteCaptures, blackCaptures, {}, {});
    return bestValue;
}

void populateDataForPlayData()
{
    ifstream fin("playdata.txt", ios::in | ios::binary);

    // Check if the file was opened successfully
    if (!fin)
        return;

    // Read the size of the lookup table from the file
    int size;
    fin.read(reinterpret_cast<char *>(&size), sizeof(size));

    // Iterate over the file and read the keys and values into the lookup table
    for (int i = 0; i < size; i++)
    {
        // Read the key from the file
        int row_size;
        fin.read(reinterpret_cast<char *>(&row_size), sizeof(row_size));
        rowSequence rs(row_size);
        fin.read(reinterpret_cast<char *>(&rs[0]), sizeof(char) * row_size);
        int pos1, pos2;
        fin.read(reinterpret_cast<char *>(&pos1), sizeof(pos1));
        fin.read(reinterpret_cast<char *>(&pos2), sizeof(pos2));
        key k(rs, make_pair(pos1, pos2));

        // Read the value from the file and add it to the lookup table
        int v;
        fin.read(reinterpret_cast<char *>(&v), sizeof(v));
        lookupTable[k] = v;
    }

    fin.close();
}

void saveDataForPlayData()
{
    ofstream fout("playdata.txt", ios::out | ios::binary);

    // Check if the file was opened successfully
    if (!fout)
        return;

    // Write the size of the lookup table to the file
    int size = lookupTable.size();
    fout.write(reinterpret_cast<const char *>(&size), sizeof(size));

    // Iterate over the map and write the keys and values to the file
    for (auto &entry : lookupTable)
    {
        const key &k = entry.first;
        int v = entry.second;

        // Write the key to the file
        const rowSequence &rs = k.first;
        positions pos = k.second;
        int row_size = rs.size();
        int pos1 = pos.first;
        int pos2 = pos.second;
        fout.write(reinterpret_cast<const char *>(&row_size), sizeof(row_size));
        fout.write(reinterpret_cast<const char *>(&rs[0]), sizeof(char) * row_size);
        fout.write(reinterpret_cast<const char *>(&pos1), sizeof(pos1));
        fout.write(reinterpret_cast<const char *>(&pos2), sizeof(pos2));

        // Write the value to the file
        fout.write(reinterpret_cast<const char *>(&v), sizeof(v));
    }

    fout.close();
}

int main()
{
    ifstream inputFile("input.txt");
    string color = "";
    float timeLeft = 100.0;
    int numberOfWhiteCaptured = 0, numberOfBlackCaptured = 0;
    vector<vector<char>> initialBoard(19, vector<char>(19, '.'));

    string line;
    int lineNumber = 0;
    int row = 0, col = 0;
    if (inputFile.is_open())
    {
        while (getline(inputFile, line))
        {
            if (lineNumber == 0)
                color = line;
            else if (lineNumber == 1)
                timeLeft = stof(line);
            else if (lineNumber == 2)
            {
                string number = "";
                for (int i = 0; i < line.size(); i++)
                {
                    if (line[i] == ',')
                    {
                        numberOfBlackCaptured = stoi(number) / 2;
                        number = "";
                    }
                    else
                    {
                        number += line[i];
                    }
                }
                numberOfWhiteCaptured = stoi(number);
            }
            else
            {
                for (int i = 0; i < 19; i++)
                {
                    initialBoard[row][col] = line[i];
                    col++;
                }
                col = 0;
                row++;
            }
            lineNumber++;
        }
        inputFile.close();
    }

    nextMove nextBest = alpha_beta_search(initialBoard, color == "BLACK" ? 0 : 1, numberOfWhiteCaptured, numberOfBlackCaptured);

    cout << nextBest.row[0] << "," << nextBest.col[0];

    saveDataForPlayData();

    return 0;
}