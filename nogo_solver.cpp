#include <signal.h>
#include <unistd.h>
#include <iostream>

#include "nogo_solver.hpp"


void NoGo::boardsize(int height, int width)
{
    board.reset(height, width);
    hash.change_boardsize(height, width);
    search.initialize(height, width);
    line_of_plays[0] = 0;
    return;
}

void NoGo::clear_board()
{
    board.reset(board.size[0], board.size[1]);
    line_of_plays[0] = 0;
    return;
}

std::string NoGo::showboard()
{
    std::vector<std::vector<int>> board2d = board.twoD_board();
    std::string board2d_str = GoBoardUtil::get_twoD_board(board2d);
    return board2d_str;
}

int NoGo::play_move(int color, int point)
/* Return 0 if all good;
 * -1 if illegal move;
 * -2 if wrong color. */
{
    if (color != board.current_player) {
        return -2;
    }

    bool success = board.play_move(point, color);
    if (! success) {
        return -1;
    }

    Grid board2d = board.twoD_board();
    uint64_t next_true_hashcode = hash.linear_congruence_func(hash.hash_func(board2d));
    int next_value = hash.get(next_true_hashcode);
    if (next_value == 1) {
        std::cerr << "losing\n";
    }
    else if (next_value == 0)
    {
        std::cerr << "winning\n";
    }
    else {
        std::cerr << "unknown value\n";
    }
    
    line_of_plays[0] += 1;
    line_of_plays[line_of_plays[0]] = point;
    return 0;
}

int NoGo::genmove(int color)
/* Return point if all good;
 * -1 if PASS;
 * -2 if wrong color;
 * -move if illegal move. */
{
    if (color != board.current_player) {
        return -2;
    }

    int move = get_move(color);

    if (move == PASS) {
        return move;
    }

    if (board.is_legal(move, color) == true) {
        board.play_move(move, color);
        line_of_plays[0] += 1;
        line_of_plays[line_of_plays[0]] = move;
    }
    else {
        return -1 * move;
    }

    return move;
}

int NoGo::undo()
/* Return 0 if succeed;
 * -1 otherwise. */
{
    int d = line_of_plays[0];
    if (d == 0) {
        return -1;
    }
    else {
        board.undo_move(line_of_plays[d]);
    }
    line_of_plays[0] -= 1;
    return 0;
}

int NoGo::solve()
{
    std::chrono::steady_clock::time_point beg = std::chrono::steady_clock::now();

    Grid board2d = board.twoD_board();
    uint64_t hashcode = hash.hash_func(board2d);
    std::vector<int> empty_points = board.get_empty_points();
    int d = (board.size[0] * board.size[1] - (int) empty_points.size());

    signal(SIGALRM, sig_handler);
    alarm(10);
    int value = search.negamax(board, hashcode, d);
    alarm(0);
    std::fprintf(stderr, "\33[2K\r");   // clear intermediate prints

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(end - beg);

    return value;
}

bool NoGo::prove()
{
    Grid board2d = board.twoD_board();
    uint64_t hashcode = hash.hash_func(board2d);

    std::array<bool, 2> result = search.proof_negamax(board, hashcode);
    if (result[1] == true) {
        std::cerr << "PROOF completed.\n";
        search.print_verify_stats();
    }
    else {
        std::cerr << "PROOF failed.\n";
    }
    return result[0];
}

std::string NoGo::store_solution(std::string f_name)
{
    return hash.store(f_name);
}

std::string NoGo::load_solution(std::string f_name)
{
    solution_loaded = f_name;
    std::cerr << "trying to load solution...\n";
    return hash.load(f_name);
}

int NoGo::get_move(int color)
{
    Grid board2d = board.twoD_board();
    uint64_t hashcode = hash.hash_func(board2d);
    uint64_t true_hashcode = hash.linear_congruence_func(hashcode);
    int value = hash.get(true_hashcode);
    if (value == 1) {
        std::cerr << "winning\n";
        std::vector<int> legal_moves = board.generate_legal_moves(board.current_player);
        for (int move : legal_moves) {
            uint64_t next_hashcode = hash.hash_func(hashcode, move, board.current_player);
            uint64_t true_next_hashcode = hash.linear_congruence_func(next_hashcode);
            if (hash.get(true_next_hashcode) == 0)
                return move;
        }
    }
    else if (value == 0)
        std::cerr << "losing\n";
    else
        std::cerr << "unknown\n";

    return board.generate_random_move(board.current_player);
}

std::string NoGo::plays_to_string()
{
    std::string plays;
    int num_plays = line_of_plays[0];
    for (int i = 1; i < num_plays+1; i++) {
        plays += GoBoardUtil::point_to_string(line_of_plays[i], board.size);
    }
    return plays;
}
