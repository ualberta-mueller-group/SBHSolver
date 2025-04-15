#ifndef SEARCH_H
#define SEARCH_H

#include "hash.hpp"
#include "board.hpp"


extern Hash hash;


class Search
{
public:
    int m_boardsize[2];
    int m_num_points;
    std::vector<std::vector<uint64_t>> m_hhtable;   // history heuristic table

    Search(int height, int width);
    ~Search() {};

    void initialize(int height, int width);

    bool negamax(NoGoBoard &board, uint64_t hashcode, int d=0);

    std::array<bool, 2> proof_negamax(NoGoBoard &board, uint64_t hashcode, int d=0);

    int h_history_heuristic(int side2move, std::vector<int> &legal_moves);

    int h_etc(uint64_t hashcode, std::vector<int> &legal_moves, int color);

    void print_hhtable();

    unsigned long num_nodes_searched();

    void print_verify_stats();

private:
    void update_hhtable(int side2move, int point, int depth);

    void print_search(uint64_t move, int d);
};

void sig_handler(int signum);

#endif
