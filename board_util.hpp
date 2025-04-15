#ifndef BOARD_UTIL_H
#define BOARD_UTIL_H

#include <array>
#include <vector>
#include <string>


typedef std::array<int, 2>              Coord;
typedef std::vector<std::vector<int>>   Grid;


const int EMPTY = 0;
const int BLACK = 1;
const int WHITE = 2;
const int BORDER = 3;
const int PASS = -1;


/* 
 * 3x5 board point numbering
 * 
 * point
 * 24 25 26 27 28 29 30
 * 18 19 20 21 22 23
 * 12 13 14 15 16 17
 * 06 07 08 09 10 11
 * 00 01 02 03 04 05
 * 
 * string
 * xx xx xx xx xx xx xx
 * xx A3 B3 C3 D3 E3
 * xx A2 B2 C2 D2 E2
 * xx A1 B1 C1 D1 E1
 * xx xx xx xx xx xx
 * 
 * canonical point
 * 10 11 12 13 14
 * 05 06 07 08 09
 * 00 01 02 03 04
 * 
 * coord
 * 31 32 33 34 35
 * 21 22 23 24 25
 * 11 12 13 14 15
 */

class GoBoardUtil
{
public:
    /**
     * Given point and boardsize,
     * Returns canonical point.
     * e.g. point C2: 15 --> 7
     */
    static int point_to_canonical_point(int point, int* boardsize);

    /**
     * Given canonical point and boardsize,
     * Returns point.
     * e.g. point C2: 7 --> 15
     */
    static int canonical_point_to_point(int canonical_point, int* boardsize);

    /**
     * Given point and boardsize,
     * Returns (row, col).
     * e.g. point C2: 15 --> (2, 3) */
    static Coord point_to_coord(int point, int* boardsize);

    /**
     * Given (row, col) and boardsize,
     * Returns the point.
     * e.g. point C2: (2, 3) --> 15 */
    static int coord_to_point(int row, int col, int* boardsize);

    static std::string point_to_string(int point, int* boardsize);

    static int string_to_point(std::string point_str, int* boardsize);

    static void shuffle(std::vector<int> &v);
    
    static int opponent(int color);

    static std::string get_twoD_board(Grid &board2d);
};

#endif
