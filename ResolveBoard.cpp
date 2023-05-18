#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#define BOARD_SIZE 19

#define BLACK 0
#define WHITE 1

using BitBoard = bitset<BOARD_SIZE * BOARD_SIZE>;
using position = vector<int>;
using moves = vector<position>;

const BitBoard bitShift = 1;
const BitBoard bitZero = 0;

#define convert2index(row, col) (row * BOARD_SIZE + col)

#define at(board, row, col) (((board) & (bitShift << convert2index(row, col))) != bitZero)
#define place(board, row, col) ((board) |= (bitShift << convert2index(row, col)))
#define remove(board, row, col) ((board) &= ((~bitShift << convert2index(row, col)) | (~bitShift >> ((BOARD_SIZE * BOARD_SIZE) - convert2index(row, col)))))

#define BOARD_SIZE 19
#define LINUX

class Board
{
public:
    Board()
    {
        m_blackStones = 0;
        m_whiteStones = 0;

        m_blackCaptures = 0;
        m_whiteCaptures = 0;

        m_gameEnd = false;
        m_gameWinner = -1;
    }

    void DisplayBoard(bool modernStyle = false)
    {
        // First Row
        cout << "BLACK CAPTURES: " << m_blackCaptures << "     WHITE CAPTURES: " << m_whiteCaptures << endl;

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
                    if (at(m_whiteStones, row, col))
                        cout << "w"
                             << "  ";
                    else if (at(m_blackStones, row, col))
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

    void Place(int player, int row, int col)
    {
        if (at(m_blackStones, row, col) || at(m_whiteStones, row, col))
        {
            cout << "Already placed here" << endl;
            return;
        }

        if (player == BLACK)
            place(m_blackStones, row, col);
        else
            place(m_whiteStones, row, col);

        m_resolve(row, col, player);
    }

    void Load(int player, int row, int col)
    {
        if (player == BLACK)
            place(m_blackStones, row, col);
        else
            place(m_whiteStones, row, col);
    }

    void SetCaptures(int whiteCaptures, int blackCaptures)
    {
        m_whiteCaptures = whiteCaptures;
        m_blackCaptures = blackCaptures;
    }

    inline bool IsOver()
    {
        return m_gameEnd;
    }

    int GetPieceCount(int player)
    {
        if (player == WHITE)
            return m_whiteStones.count();
        else
            return m_blackStones.count();
    }

public:
    BitBoard m_whiteStones;
    BitBoard m_blackStones;

    bool m_gameEnd;
    int m_gameWinner;

    int m_blackCaptures;
    int m_whiteCaptures;

    struct cellEval
    {
        uint16_t closedFours;
        uint16_t openFours;
        uint16_t openThrees;
        uint16_t closedThrees;
        uint16_t closedTwos;
        uint16_t chainFives;

        cellEval()
        {
            closedFours = 0;
            openFours = 0;
            openThrees = 0;
            closedThrees = 0;
            closedTwos = 0;
            chainFives = 0;
        }

        cellEval &operator+=(const cellEval &other)
        {
            closedFours += other.closedFours;
            openFours += other.openFours;
            closedThrees += other.closedThrees;
            openThrees += other.openThrees;
            closedTwos += other.closedTwos;
            chainFives += other.chainFives;

            return *this;
        }
    };

    cellEval m_getCellEval(int row, int col)
    {
        uint16_t lower_bound_x = max(0, row - 4);
        uint16_t upper_bound_x = min(BOARD_SIZE - 1, row + 4);

        uint16_t lower_bound_y = max(0, col - 4);
        uint16_t upper_bound_y = min(BOARD_SIZE - 1, col + 4);

        uint16_t cell_color = at(m_blackStones, row, col) ? BLACK : WHITE;
        BitBoard *playerGrid = cell_color == BLACK ? &m_blackStones : &m_whiteStones;
        BitBoard *opponentGrid = cell_color == BLACK ? &m_whiteStones : &m_blackStones;

        // 0-1 '\' , 2-3 '/' , 4-5 '|' , 6-7 '-'
        uint16_t consecutives[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        bool opens[8] = {0, 0, 0, 0, 0, 0, 0, 0};

        int rowIncrementer[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
        int colIncrementer[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
        int consecutiveIndices[] = {0, 4, 2, 6, 0, 7, 3, 5, 1};

        for (int itr = 0; itr < 9; itr++)
        {
            if (itr == 4)
                continue;

            uint16_t row_bound = rowIncrementer[itr] == -1 ? lower_bound_x : upper_bound_x;
            uint16_t col_bound = colIncrementer[itr] == -1 ? lower_bound_y : upper_bound_y;

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

            for (uint16_t r = row, c = col; rowCheck(r) && colCheck(c); r += rowIncrementer[itr], c += colIncrementer[itr])
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

    void m_resolve(int row, int col, int player)
    {
        BitBoard *currentGrid = player == BLACK ? &m_blackStones : &m_whiteStones;
        BitBoard *opponentGrid = player == WHITE ? &m_blackStones : &m_whiteStones;

        if (m_getCellEval(row, col).chainFives > 0)
        {
            m_gameEnd = true;
            m_gameWinner = player;
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
                                if (player == BLACK)
                                    m_whiteCaptures++;
                                else
                                    m_blackCaptures++;

                                remove((*opponentGrid), r1, c1);
                                remove((*opponentGrid), r2, c2);
                            }
                        }
                    }
                }
                inc++;
            }
        }

        if (m_blackCaptures >= 5)
        {
            m_gameEnd = true;
            m_gameWinner = WHITE;
        }
        else if (m_whiteCaptures >= 5)
        {
            m_gameEnd = true;
            m_gameWinner = BLACK;
        }
    }
};

void LoadInputFile(string fileName, Board &outBoard, int &outPlayer)
{
    float time;
    int blackCaptures;
    int whiteCaptures;

    string line;
    int lineCnt = 0;

    ifstream file;
    file.open(fileName);

    if (file.is_open())
    {
        int row = 0;
        while (getline(file, line))
        {
            switch (lineCnt)
            {
            case 0:
            {
                outPlayer = line == "BLACK" ? BLACK : WHITE;
                break;
            }
            case 1:
            {
                time = stof(line);
                break;
            }
            case 2:
            {
                string count = "";
                for (int i = 0; i < line.size(); i++)
                {
                    if (line[i] == ',')
                    {
                        blackCaptures = stoi(count);
                        count = "";
                    }
                    else
                    {
                        count += line[i];
                    }
                }
                whiteCaptures = stoi(count);

                outBoard.SetCaptures(whiteCaptures, blackCaptures);

                break;
            }
            default:
            {
                for (int col = 0; col < 19; col++)
                {
                    if (line[col] != '.')
                        outBoard.Load(line[col] == 'w' ? WHITE : BLACK, row, col);
                }
                row++;
                break;
            }
            }

            lineCnt++;
        }

        file.close();
    }
}

int main(int argc, char **argv)
{
    Board board;
    int AIplayer;

    LoadInputFile(argv[1], board, AIplayer);

    string move = argv[2];
    string number = "";
    int row = -1;
    int col = -1;
    for (int i = 0; i < move.size(); i++)
    {
        if (move[i] == ',')
        {
            row = stoi(number);
            number = "";
        }
        else
        {
            number += move[i];
        }
    }
    col = stoi(number);

    board.Place(AIplayer, row, col);

#ifdef LINUX
    system("clear");
#else
    system("clr");
#endif
    board.DisplayBoard();
    cout << endl;
    cout << ((AIplayer == 0) ? "BLACK" : "WHITE") << " played at " << row << ", " << col << endl;
    cout << (AIplayer == 0 ? "WHITE" : "BLACK") << " is playing now..." << endl;

    if (board.IsOver())
    {
#ifdef LINUX
        system("clear");
#else
        system("clr");
#endif
        cout << ((AIplayer == 0) ? "BLACK" : "WHITE") << " has Won!!!" << endl;
        board.DisplayBoard();

        return 1;
    }

    ofstream file(argv[3]);
    file << (AIplayer == 0 ? "WHITE" : "BLACK") << endl;
    file << "100.0" << endl;
    file << board.m_blackCaptures << "," << board.m_whiteCaptures << endl;
    for (int i = 0; i < 19; i++)
    {
        for (int j = 0; j < 19; j++)
        {
            if (at(board.m_blackStones, i, j))
                file << "b";
            else if (at(board.m_whiteStones, i, j))
                file << "w";
            else
                file << ".";
        }
        file << endl;
    }

    return 0;
}