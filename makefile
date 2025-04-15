CXX = g++
CPPFLAGS = -Wall -std=c++17 -O3

default: main_solver.o gtp_connection.o nogo_solver.o search.o hash.o memory_manager.o board.o board_util.o
	$(CXX) $(CPPFLAGS) main_solver.o gtp_connection.o nogo_solver.o search.o hash.o memory_manager.o board.o board_util.o -o solver_main

main_solver.o: main_solver.cpp gtp_connection.hpp nogo_solver.hpp search.hpp hash.hpp board.hpp memory_manager.hpp configs.hpp board_util.hpp
	$(CXX) $(CPPFLAGS) -c main_solver.cpp

gtp_connection.o: gtp_connection.hpp gtp_connection.cpp nogo_solver.hpp search.hpp hash.hpp board.hpp memory_manager.hpp configs.hpp board_util.hpp
	$(CXX) $(CPPFLAGS) -c gtp_connection.cpp

nogo_solver.o: nogo_solver.hpp nogo_solver.cpp search.hpp hash.hpp board.hpp memory_manager.hpp configs.hpp board_util.hpp
	$(CXX) $(CPPFLAGS) -c nogo_solver.cpp

search.o: search.hpp search.cpp hash.hpp board.hpp memory_manager.hpp configs.hpp board_util.hpp
	$(CXX) $(CPPFLAGS) -c search.cpp

hash.o: hash.hpp hash.cpp memory_manager.hpp configs.hpp board_util.hpp
	$(CXX) $(CPPFLAGS) -c hash.cpp

memory_manager.o: memory_manager.hpp memory_manager.cpp configs.hpp
	$(CXX) $(CPPFLAGS) -c memory_manager.cpp

board.o: board.hpp board.cpp board_util.hpp
	$(CXX) $(CPPFLAGS) -c board.cpp

board_util.o: board_util.hpp board_util.cpp
	$(CXX) $(CPPFLAGS) -c board_util.cpp

clean:
	rm -f *.o solver_main
