#include "board_util.hpp"


int GoBoardUtil::point_to_canonical_point(int point, int* boardsize)
{
    int NS = boardsize[1] + 1;
    int r = point / NS - 1;
    int c = point % NS - 1;
    int canonical_point = r * boardsize[1] + c;
    return canonical_point;
}

int GoBoardUtil::canonical_point_to_point(int canonical_point, int* boardsize)
{
    int NS = boardsize[1] + 1;
    int r = canonical_point / boardsize[1];
    int c = canonical_point % boardsize[1];
    int point = (r + 1) * NS + (c + 1);
    return point;
}

Coord GoBoardUtil::point_to_coord(int point, int* boardsize)
{
    int NS = boardsize[1] + 1;
    Coord coord;
    coord[0] = point / NS;      // row ~ 1...boardsize
    coord[1] = point % NS;      // col ~ 1...boardsize
    return coord;
}

int GoBoardUtil::coord_to_point(int row, int col, int* boardsize)
{
    int NS = boardsize[1] + 1;
    return NS * row + col;
}

std::string GoBoardUtil::point_to_string(int point, int* boardsize)
{
    int NS = boardsize[1] + 1;
    int row = point / NS;
    int col = point % NS;
    return (char)('A'+col-1) + std::to_string(row);
}

int GoBoardUtil::string_to_point(std::string point_str, int* boardsize)
{
    int NS = boardsize[1] + 1;
    int row = std::stoi(point_str.substr(1, point_str.length()-1));
    int col = point_str[0] - 64;
    return NS * row + col;
}

void GoBoardUtil::shuffle(std::vector<int> &v)
{
    int v_size = v.size();
    for (int i = 0; i < v_size; i++) {
        int j = i + rand() % (v_size - i);
        std::swap(v[i], v[j]);
    }
}

int GoBoardUtil::opponent(int color)
{
    return BLACK + WHITE - color;
}

std::string GoBoardUtil::get_twoD_board(Grid &board2d)
{
    std::string board2d_str;
    int num_rows = board2d.size();
    int num_cols = board2d[0].size();
    for (int i = 0; i < num_rows; i++) {
        for (int j = 0; j < num_cols; j++) {
            board2d_str += std::to_string(board2d[i][j]) + " ";
        }
        if (i < num_rows-1) {
            board2d_str += "\n";
        }
    }
    return board2d_str;
}
