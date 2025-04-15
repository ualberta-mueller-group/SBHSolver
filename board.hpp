#ifndef BOARD_H
#define BOARD_H

#include "board_util.hpp"


class NoGoBoard
{
public:
    int size[2] = {4, 4};   // (vertical size, horizontal size)
    int NS = size[1] + 1;
    int WE = 1;
    int current_player = BLACK;
    int maxpoint = (size[0] + 2) * NS + 1;
    std::vector<int> board = std::vector<int>(maxpoint, BORDER);

    NoGoBoard();
    NoGoBoard(int size);
    NoGoBoard(int v_size, int h_size);

    ~NoGoBoard();

    void reset(int v_size, int h_size);

    NoGoBoard copy();

    int get_color(int point);

    int pt(int row, int col);

    bool is_legal(int point, int color);

    std::vector<int> get_empty_points();

    int row_start(int row);

    bool is_eye(int point, int color);

    bool has_liberty(int point);

    bool play_move(int point, int color, bool check_legality=true);

    bool undo_move(int point);

    std::vector<int> neighbors_of_color(int point, int color);

    std::vector<int> generate_legal_moves(int color);

    int generate_random_move(int color);

    Grid twoD_board();

private:
    void initialize_empty_points(std::vector<int> &board);

    bool is_surrounded(int point, int color);

    std::vector<int> neighbors(int point);

    std::vector<int> diag_neighbors(int point);
};

#endif
