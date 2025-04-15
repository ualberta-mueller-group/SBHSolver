#ifndef GTP_CONNECTION_H
#define GTP_CONNECTION_H

#include "nogo_solver.hpp"


class GtpConnection
{
public:
    NoGo nogo_engine;
    bool m_quit = false;
    std::vector<std::string> command_names = {
        "play",
        "genmove",
        "undo",
        "protocol_version",
        "quit",
        "name",
        "version",
        "boardsize",
        "clear_board",
        "showboard",
        "komi",
        "list_commands",
        "solve",
        "prove",
        "store_solution",
        "load_solution",
        "search_size",
        "proof_size",
        "stats",
        "debug"
    };
    std::vector<std::string> gogui_commands = {
        "play",
        "genmove",
        "undo",
        "protocol_version",
        "quit",
        "name",
        "version",
        "boardsize",
        "clear_board",
        "showboard",
        "komi",
        "list_commands"
    };
    std::vector<void (GtpConnection::*)(std::vector<std::string> &args)> commands = {
        &GtpConnection::play_cmd,
        &GtpConnection::genmove_cmd,
        &GtpConnection::undo_cmd,
        &GtpConnection::protocol_version_cmd,
        &GtpConnection::quit_cmd,
        &GtpConnection::name_cmd,
        &GtpConnection::version_cmd,
        &GtpConnection::boardsize_cmd,
        &GtpConnection::clear_board_cmd,
        &GtpConnection::showboard_cmd,
        &GtpConnection::komi_cmd,
        &GtpConnection::list_commands_cmd,
        &GtpConnection::solve_cmd,
        &GtpConnection::prove_cmd,
        &GtpConnection::store_solution_cmd,
        &GtpConnection::load_solution_cmd,
        &GtpConnection::search_size_cmd,
        &GtpConnection::proof_size_cmd,
        &GtpConnection::stats_cmd,
        &GtpConnection::debug_cmd
    };

    // GtpConnection(NoGo &nogo_engine, NoGoBoard &board, bool debug_mode=false);
    GtpConnection(NoGo &nogo_engine, bool debug_mode=false);
    ~GtpConnection() {};

    void start_connection();

    void register_cmd();

    void get_cmd(std::string &line);

    void execute_cmd(std::string command_name, std::vector<std::string> &args);

    void respond(std::string response="");

    void protocol_version_cmd(std::vector<std::string> &args);

    void quit_cmd(std::vector<std::string> &args);

    void name_cmd(std::vector<std::string> &args);

    void version_cmd(std::vector<std::string> &args);

    void boardsize_cmd(std::vector<std::string> &args);

    void clear_board_cmd(std::vector<std::string> &args);

    void showboard_cmd(std::vector<std::string> &args);

    void komi_cmd(std::vector<std::string> &args);

    void list_commands_cmd(std::vector<std::string> &args);

    void play_cmd(std::vector<std::string> &args);

    void genmove_cmd(std::vector<std::string> &args);

    void undo_cmd(std::vector<std::string> &args);

    void solve_cmd(std::vector<std::string> &args);

    void prove_cmd(std::vector<std::string> &args);

    void store_solution_cmd(std::vector<std::string> &args);

    void load_solution_cmd(std::vector<std::string> &args);

    void search_size_cmd(std::vector<std::string> &args);

    void proof_size_cmd(std::vector<std::string> &args);

    void stats_cmd(std::vector<std::string> &args);

    void debug_cmd(std::vector<std::string> &args);

private:
    bool m_debug_mode;
    
};

void string_to_upper(std::string &s);

int color_to_int(std::string color);

#endif
