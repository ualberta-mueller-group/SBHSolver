#ifndef NOGO_SOLVER_H
#define NOGO_SOLVER_H

#include <chrono>

#include "search.hpp"


extern Hash hash;


class NoGo
{
public:
    std::string name = "NoGo";
    double version = 1.0;
    NoGoBoard board;
    Search search;
    std::array<int, 101> line_of_plays = { 0 };  // 0th element is the # of moves played
    std::string solution_loaded;
    std::chrono::seconds elapsed_time;


    NoGo(NoGoBoard& board, Search& search) :
        board(board), search(search) {};
    ~NoGo() {};

    void boardsize(int length, int width);

    void clear_board();

    std::string showboard();

    int play_move(int color, int point);

    int genmove(int color);

    int undo();

    int solve();

    bool prove();

    std::string store_solution(std::string f_name="solution");

    std::string load_solution(std::string f_name="solution");

    int get_move(int color);

    std::string plays_to_string();
};

#endif
