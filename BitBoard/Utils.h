#pragma once
#include <fstream>
#include <string>
#include "Board.h"
#include "Colors.h"

namespace Utils
{
    using namespace std;
    /* Input parsing */
    void LoadInputFile(string fileName, Pente::Board &outBoard, int &outPlayer)
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
                    outPlayer = line == "BLACK" ? colors::BLACK : colors::WHITE;
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
                            outBoard.Load(line[col] == 'w' ? colors::WHITE : colors::BLACK, row, col);
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

}