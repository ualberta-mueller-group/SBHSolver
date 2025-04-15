#include "board.hpp"
#include "gtp_connection.hpp"
#include "hash.hpp"
#include "memory_manager.hpp"
#include "configs.hpp"
#include <iostream>


MemoryManager manager;
Hash hash(N_ROWS, N_COLS);


int main()
{
    srand(time(0));
    NoGoBoard board(N_ROWS, N_COLS);
    Search search(N_ROWS, N_COLS);
    NoGo nogo_engine(board, search);
    GtpConnection con(nogo_engine);
    con.start_connection();

    return 0;
}
