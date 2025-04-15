#include <cassert>
#include <iostream>

#include "board.hpp"


NoGoBoard::NoGoBoard()
{
    initialize_empty_points(board);
}

NoGoBoard::NoGoBoard(int size)
{
    NoGoBoard::reset(size, size);
}

NoGoBoard::NoGoBoard(int v_size, int h_size)
{
    NoGoBoard::reset(v_size, h_size);
}

NoGoBoard::~NoGoBoard() {}

void NoGoBoard::reset(int v_size, int h_size)
{
    size[0] = v_size;
    size[1] = h_size;
    NS = h_size + 1;
    WE = 1;
    current_player = BLACK;
    maxpoint = (v_size + 2) * NS + 1;
    board = std::vector<int>(maxpoint, BORDER);
    initialize_empty_points(board);
}

NoGoBoard NoGoBoard::copy()
{
    NoGoBoard b = NoGoBoard(size[0], size[1]);
    b.current_player = current_player;
    b.board = board;
    return b;
}

int NoGoBoard::get_color(int point)
{
    return board[point];
}

int NoGoBoard::pt(int row, int col)
{
    return GoBoardUtil::coord_to_point(row, col, size);
}

bool NoGoBoard::is_legal(int point, int color)
{
    if (get_color(point) != EMPTY) {
        return false;
    }

    board[point] = color;

    if (not has_liberty(point)) {
        board[point] = EMPTY;
        return false;
    }

    int opp_color = GoBoardUtil::opponent(color);
    std::vector<int> nbrs = neighbors(point);
    for (int i = 0; i < (int) nbrs.size(); i++) {
        int nbr = nbrs[i];
        if (board[nbr] == opp_color && not has_liberty(nbr)) {
            board[point] = EMPTY;
            return false;
        }
    }

    board[point] = EMPTY;
    return true;
}

std::vector<int> NoGoBoard::get_empty_points()
{
    std::vector<int> empty_points;
    for (int p = 0; p < maxpoint; p++) {
        if (board[p] == EMPTY) {
            empty_points.push_back(p);
        }
    }
    return empty_points;
}

int NoGoBoard::row_start(int row)
{
    return row * NS + 1;
}

void NoGoBoard::initialize_empty_points(std::vector<int> &board)
{
    for (int r = 1; r <= size[0]; r++) {
        int start = row_start(r);
        for (int c = 1; c <= size[1]; c++) {
            board[start++] = EMPTY;
        }
    }
}

bool NoGoBoard::is_eye(int point, int color)
{
    if (! is_surrounded(point, color)) {
        return false;
    }
    int opp_color = GoBoardUtil::opponent(color);
    int false_count = 0, at_edge = 0;
    std::vector<int> diag_nbrs = diag_neighbors(point);
    for (int i = 0; i < (int) diag_nbrs.size(); i++) {
        if (board[diag_nbrs[i]] == BORDER) {
            at_edge = 1;
        }
        if (board[diag_nbrs[i]] == opp_color) {
            false_count++;
        }
    }
    return false_count <= 1 - at_edge;
}

bool NoGoBoard::is_surrounded(int point, int color)
{
    std::vector<int> nbrs = neighbors(point);
    for (int i = 0; i < (int) nbrs.size(); i++) {
        int nbr_color = board[nbrs[i]];
        if (nbr_color != BORDER && nbr_color != color) {
            return false;
        }
    }
    return true;
}

bool NoGoBoard::has_liberty(int point)
{
    std::vector<bool> marker = std::vector<bool>(maxpoint, false);
    std::vector<int> pointstack = {point};
    int color = get_color(point);
    marker[point] = true;
    while (! pointstack.empty()) {
        int p = pointstack.back();
        pointstack.pop_back();
        std::vector<int> nbrs = neighbors(p);
        for (int i = 0; i < (int) nbrs.size(); i++) {
            if (get_color(nbrs[i]) == EMPTY) {
                return true;
            }
            if (! marker[nbrs[i]] && get_color(nbrs[i]) == color) {
                marker[nbrs[i]] = true;
                pointstack.push_back(nbrs[i]);
            }
        }
    }
    return false;
}

bool NoGoBoard::play_move(int point, int color, bool check_legality)
{
    if (check_legality == true && not is_legal(point, color)) {
        return false;
    }
    
    board[point] = color;

    current_player = GoBoardUtil::opponent(color);
    return true;
}

bool NoGoBoard::undo_move(int point)
{
    assert(board[point] != EMPTY);
    board[point] = EMPTY;
    current_player = GoBoardUtil::opponent(current_player);
    return true;
}

std::vector<int> NoGoBoard::neighbors_of_color(int point, int color)
{
    std::vector<int> nbc;
    std::vector<int> nbrs = neighbors(point);
    for (int i = 0; i < (int) nbrs.size(); i++) {
        int c = get_color(nbrs[i]);
        if (c == color) {
            nbc.push_back(nbrs[i]);
        }
    }
    return nbc;
}

std::vector<int> NoGoBoard::neighbors(int point)
{
    std::vector<int> nbrs = {point - 1, point + 1, point - NS, point + NS};
    return nbrs;
}

std::vector<int> NoGoBoard::diag_neighbors(int point)
{
    std::vector<int> diag_nbrs = {point - NS - 1,
                                point - NS + 1,
                                point + NS - 1,
                                point + NS + 1};
    return diag_nbrs;
}

std::vector<int> NoGoBoard::generate_legal_moves(int color)
{
    std::vector<int> moves = get_empty_points();
    std::vector<int> legal_moves = {};
    for (int i = 0; i < (int) moves.size(); i++) {
        int move = moves[i];
        if (is_legal(move, color)) {
            legal_moves.push_back(move);
        }
    }
    return legal_moves;
}

int NoGoBoard::generate_random_move(int color)
{
    std::vector<int> moves = get_empty_points();
    GoBoardUtil::shuffle(moves);
    for (int i = 0; i < (int) moves.size(); i++) {
        int move = moves[i];
        if (is_legal(move, color)) {
            return move;
        }
    }
    return -1;
}

Grid NoGoBoard::twoD_board()
{
    Grid board2d;
    for (int r = size[0]; r > 0; r--) {
        std::vector<int> row;
        int start = row_start(r);
        for (int c = 1; c <= size[1]; c++) {
            row.push_back(board[start++]);
        }
        board2d.push_back(row);
    }
    return board2d;
}