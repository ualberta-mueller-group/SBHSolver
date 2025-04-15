#include <unistd.h>
#include <iostream>
#include <cassert>

#include "search.hpp"


uint64_t node_count = 0;
uint64_t nodes_at_depth[100] = { 0 };


void sig_handler(int signum)
{
    alarm(10);

    std::fprintf(stderr, "\33[2K\r%lu nodes/s", node_count / 10);
    std::fflush(stderr);
    node_count = 0;
}

Search::Search(int height, int width)
{
    initialize(height, width);
}

void Search::initialize(int height, int width)
{
    m_boardsize[0] = height;
    m_boardsize[1] = width;
    m_num_points = height * width;
    m_hhtable = std::vector<std::vector<uint64_t>>(2, std::vector<uint64_t>(m_num_points, 0));
}

bool Search::negamax(NoGoBoard &board, uint64_t hashcode, int d)
/* Return 0 indicating the current board is losing;
 * Return 1 if winning */
{
    uint64_t true_hashcode = hash.linear_congruence_func(hashcode);
    int value = hash.get(true_hashcode);

    // already inside transposition table
    if (value != -1) {
        node_count++;
        return value;
    }

    std::vector<int> valid_moves = board.generate_legal_moves(board.current_player);
    int valid_moves_size = (int) valid_moves.size();

    // terminal state - no legal moves
    if (valid_moves_size == 0) {
        hash.insert(true_hashcode, false);
        node_count++;
        return 0;
    }

    int idx = h_etc(hashcode, valid_moves, board.current_player);
    if (idx != -1) {
        hash.insert(true_hashcode, true);
        update_hhtable(board.current_player, valid_moves[idx], d);
        node_count++;
        return 1;
    }

    for (int i = 0; i < valid_moves_size; i++) {
        idx = h_history_heuristic(board.current_player, valid_moves);
        int move = valid_moves[idx];
        
        uint64_t next_hashcode = hash.hash_func(hashcode, move, board.current_player);

        bool played = board.play_move(move, board.current_player);
        assert(played);
        value = 1 - negamax(board, next_hashcode, d+1);     // equivelant to negating the minimax value
        board.undo_move(move);

        if (value == 1) {
            hash.insert(true_hashcode, true);
            update_hhtable(board.current_player, move, d);
            node_count++;
            return 1;
        }

        valid_moves.erase(valid_moves.begin()+idx);
    }

    assert(valid_moves.size() == 0);
    hash.insert(true_hashcode, false);
    node_count++;
    return 0;
}

std::array<bool, 2> Search::proof_negamax(NoGoBoard &board, uint64_t hashcode, int d)
{
    uint64_t true_hashcode = hash.linear_congruence_func(hashcode);
    int predicted_value = hash.get(true_hashcode);
    bool proved = hash.get_proof_bit(true_hashcode);

    if (proved == true) {
        bool value = predicted_value;
        return {value, true};
    }

    if (predicted_value == -1) {
        return {false, false};
    }

    std::vector<int> valid_moves = board.generate_legal_moves(board.current_player);
    int valid_moves_size = (int) valid_moves.size();

    // terminal state - no legal moves
    if (valid_moves_size == 0) {
        bool bit_changed = hash.set_proof_bit(true_hashcode);
        nodes_at_depth[d] += bit_changed;
        return {false, predicted_value==false};
    }

    if (predicted_value == 1) {
        int i = h_etc(hashcode, valid_moves, board.current_player);

        if (i == -1) {
            return {false, false};
        }
        else {
            int move = valid_moves[i];
            uint64_t next_hashcode = hash.hash_func(hashcode, move, board.current_player);

            bool played = board.play_move(move, board.current_player);
            assert(played);
            std::array<bool, 2> result = proof_negamax(board, next_hashcode, d+1);
            board.undo_move(move);

            int value = 1 - result[0];

            bool bit_changed = hash.set_proof_bit(true_hashcode);
            nodes_at_depth[d] += bit_changed;
            return {true, result[1] == true && predicted_value == value};
        }
    }

    for (int i = 0; i < valid_moves_size; i++) {
        int move = valid_moves[i];
        uint64_t next_hashcode = hash.hash_func(hashcode, move, board.current_player);

        bool played = board.play_move(move, board.current_player);
        assert(played);
        std::array<bool, 2> result = proof_negamax(board, next_hashcode, d+1);
        board.undo_move(move);

        int value = 1 - result[0];

        if (result[1] == false || predicted_value != value) {
            return {false, false};
        }
    }

    bool bit_changed = hash.set_proof_bit(true_hashcode);
    nodes_at_depth[d] += bit_changed;
    return {false, true};
}

int Search::h_history_heuristic(int side2move, std::vector<int> &legal_moves)
{
    int length = (int) legal_moves.size();

    int best_move_idx = 0;
    uint64_t best_value = 0;
    for (int i = 0; i < length; i++) {
        int canonical_move = GoBoardUtil::point_to_canonical_point(legal_moves[i], m_boardsize);
        uint64_t value = m_hhtable[side2move-1][canonical_move];
        if (value > best_value) {
            best_value = value;
            best_move_idx = i;
        }
    }

    return best_move_idx;
}

int Search::h_etc(uint64_t hashcode, std::vector<int> &legal_moves, int color)
/* Find the child that is losing, so the parent is winning.
 * Returns the idx of a move that leads to a losing child node; if not exists, returns -1 */
{
    int length = (int) legal_moves.size();

    for (int i = 0; i < length; i++) {
        uint64_t new_hashcode = hash.hash_func(hashcode, legal_moves[i], color);
        uint64_t true_new_hashcode = hash.linear_congruence_func(new_hashcode);
        int value = hash.get(true_new_hashcode);
        if (value == 0) {
            return i;
        }
    }
    return -1;
}

unsigned long Search::num_nodes_searched()
{
    return node_count;
}

void Search::update_hhtable(int side2move, int point, int depth)
{
    uint64_t d_heuristic_value = m_num_points -1 - depth;
    d_heuristic_value *= d_heuristic_value;
    int canonical_point = GoBoardUtil::point_to_canonical_point(point, m_boardsize);
    m_hhtable[side2move-1][canonical_point] += d_heuristic_value;
}

void Search::print_hhtable()
{
    for (int i = 0; i < m_boardsize[0]; i++) {
        for (int j = 0; j < m_boardsize[1]; j++) {
            std::cerr << m_hhtable[i][j] << "\t";
        }
        std::cerr << std::endl;
    }
}

void Search::print_search(uint64_t move, int d)
{
    std::cerr << std::string(d, '\t') << move << std::endl;
    return;
}

void Search::print_verify_stats()
{
    std::cerr << "in this solution...\n";
    for (int i = 0; i < m_num_points; i++) {
        std::cerr << "nodes at depth " << i << ": " << nodes_at_depth[i] << std::endl;
    }
    return;
}
