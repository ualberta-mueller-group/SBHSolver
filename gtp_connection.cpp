#include <iostream>
#include <cassert>
#include <fstream>

#include "gtp_connection.hpp"


GtpConnection::GtpConnection(NoGo &nogo_engine, bool debug_mode) :
    nogo_engine(nogo_engine)
{
    this->m_debug_mode = debug_mode;
}

void GtpConnection::start_connection()
{
    std::cerr << "<- CONNECTION READY ->" << std::endl;
    std::string line;
    while (m_quit == false) {
        std::getline(std::cin, line);
        get_cmd(line);
    }
}

void GtpConnection::register_cmd()
{
    commands.push_back(&GtpConnection::protocol_version_cmd);
}

void GtpConnection::get_cmd(std::string &line)
{
    std::vector<std::string> elements;
    int head = 0;
    int line_len = line.length();
    for (int i = 0; i < line_len; i++) {
        if (line[i] == ' ') {
            if (head < i) {
                elements.push_back(line.substr(head, i-head));
            }
            head = i + 1;
        }
    }
    elements.push_back(line.substr(head, line_len-head+1));

    if (elements.size() == 0) {
        return;
    }
    std::string command_name = elements[0];
    std::vector<std::string> args;
    for (int i = 1; i < (int) elements.size(); i++) {
        args.push_back(elements[i]);
    }
    execute_cmd(command_name, args);
}

void GtpConnection::execute_cmd(std::string command_name, std::vector<std::string> &args)
{
    for (unsigned int i = 0; i < command_names.size(); i++) {
        if (! command_names[i].compare(command_name)) {
            (this->*commands[i])(args);
            return;
        }
    }
    respond("command not found: " + command_name);
}

void GtpConnection::respond(std::string response)
{
    std::cout << "= " + response + "\n\n";
}

void GtpConnection::protocol_version_cmd(std::vector<std::string> &args)
{
    respond("2");
}

void GtpConnection::quit_cmd(std::vector<std::string> &args)
{
    m_quit = true;
    respond("quiting...");
}

void GtpConnection::name_cmd(std::vector<std::string> &args)
{
    respond(nogo_engine.name);
}

void GtpConnection::version_cmd(std::vector<std::string> &args)
{
    respond(std::to_string(nogo_engine.version));
}

void GtpConnection::boardsize_cmd(std::vector<std::string> &args)
{
    nogo_engine.clear_board();
    std::cerr << "WARNING: Do not change the board size! Modify configs.hpp and re-compile for a different board size.\n";
    respond();
}

void GtpConnection::clear_board_cmd(std::vector<std::string> &args)
{
    nogo_engine.clear_board();
    respond();
}

void GtpConnection::showboard_cmd(std::vector<std::string> &args)
{
    std::string board2d_str = nogo_engine.showboard();
    respond("\n"+board2d_str);
}

void GtpConnection::komi_cmd(std::vector<std::string> &args)
{
    std::cerr << "N/A\n";
    respond();
}

void GtpConnection::list_commands_cmd(std::vector<std::string> &args)
{
    std::string joined = "";
    for (int i = 0; i < (int) gogui_commands.size(); i++) {
        joined += gogui_commands[i] + " ";
    }
    respond(joined);
}

void GtpConnection::play_cmd(std::vector<std::string> &args)
{
    std::string msg;
    if (args.size() != 2) {
        msg = "argument error!";
        respond(msg);
        return;
    }
    string_to_upper(args[1]);
    int color = color_to_int(args[0]);
    int move = GoBoardUtil::string_to_point(args[1], nogo_engine.board.size);

    int code = nogo_engine.play_move(color, move);
    assert(code == 0 || code == -1 || code == -2);
    if (code == 0) {
        msg = "";
    }
    else if (code == -1) {
        msg = "illegal move";
    }
    else if (code == -2) {
        msg = "wrong color";
    }
    respond(msg);
}

void GtpConnection::genmove_cmd(std::vector<std::string> &args)
{
    int color = color_to_int(args[0]);
    bool valid = true;

    int move = nogo_engine.genmove(color);

    if (move == PASS) {
        respond("resign");
        return;
    }
    else if (move == -2) {
        respond("wrong color");
        return;
    }
    else if (move < 0) {
        valid = false;
        move = -1 * move;
    }

    std::string move_str = GoBoardUtil::point_to_string(move, nogo_engine.board.size);
    if (valid) {
        respond(move_str);
    }
    else {
        respond("Illegal move: " + move_str);
    }
}

void GtpConnection::undo_cmd(std::vector<std::string> &args)
{
    int code = nogo_engine.undo();
    assert(code == 0 || code == -1);
    if (code == -1) {
        respond("cannot undo: no move has been made");
        return;
    }
    respond();
}

void GtpConnection::solve_cmd(std::vector<std::string> &args)
{
    int value = nogo_engine.solve();
    std::string value_s = std::to_string(value);
    respond(value_s);
}

void GtpConnection::prove_cmd(std::vector<std::string> &args)
{
    bool value = nogo_engine.prove();
    std::string value_s = std::to_string(value);
    respond(value_s);
}

void GtpConnection::store_solution_cmd(std::vector<std::string> &args)
{
    std::string msg;
    if (args.size() != 1) {
        msg = "argument error!";
    }
    else {
        std::string f_name = args[0];
        nogo_engine.store_solution(f_name);
        msg = "solution stored to [" + f_name + "]";
    }
    respond(msg);
}

void GtpConnection::load_solution_cmd(std::vector<std::string> &args)
{
    std::string msg;
    if (args.size() != 1) {
        msg = "argument error!";
    }
    else {
        std::string f_name = args[0];
        nogo_engine.load_solution(f_name);
        msg = "solution loaded from [" + f_name + "]";
    }
    respond(msg);
}

void GtpConnection::search_size_cmd(std::vector<std::string> &args)
{
    uint64_t size = hash.size();
    std::string msg = "size of search: " + std::to_string(size) + " nodes";
    respond(msg);
}

void GtpConnection::proof_size_cmd(std::vector<std::string> &args)
{
    uint64_t proof_size = hash.proof_size();
    std::string msg = "size of proof: " + std::to_string(proof_size) + " nodes";
    respond(msg);
}

void GtpConnection::stats_cmd(std::vector<std::string> &args)
{
    // add code below for logging stats
    respond();
}

void GtpConnection::debug_cmd(std::vector<std::string> &args)
{
    // add code below for debugging
    respond();
}

void string_to_upper(std::string &s)
{
    int length = s.size();
    for (int i = 0; i < length; i++) {
        if (97 <= s[i] && s[i] <= 122) {
            s[i] -= 32;     // convert to uppercase
        }
    }
}

int color_to_int(std::string color)
{
    if (color.compare("b")==0 || color.compare("B")==0) {
        return BLACK;
    }
    if (color.compare("w")==0 || color.compare("W")==0) {
        return WHITE;
    }
    return -1;
}
