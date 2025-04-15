# SBHSolver for NoGo

SBHSolver is a game-solving program for the game of NoGo. It is a minimax solver with Sorted Bucket Hash (SBH) transposition table.

This repo contains the extended source code of our [paper](https://link.springer.com/chapter/10.1007/978-3-031-54968-7_4):  
Haoyu Du, Ting Han Wei, and Martin Müller. Solving NoGo on Small Rectangular Boards. In *Advances in Computer Games*, 2023.

SBHSolver features:
* SBH hashing. A newly proposed perfect hashing method for weakly solving games.
* Enhanced transposition cutoff.
* History heuristic.

## How to Use

Compile the source code with `make` to get the executable `solver_main`. SBHSolver loosely supports Go Text Protocol (GTP). Run `solver_main` interactively through command line.

Specify the board size and configurations in `configs.hpp`. Do not change the board size through GTP at run time.

Useful commands in addition to GTP standards:
* `solve` Solve the current board with implied next player.
* `prove` Extract the solution (principal variations) from the transposition table. Only run after `solve`.
* `search_size` Number of nodes of a search DAG. Equivalently, number of nodes in transposition table.
* `proof_size` Number of nodes of a solution.
* `store_solution [file_name]` Store the solution to a file.
* `load_solution [file_name]` Load the solution from a file.

## Extended Features

Two extended features are implemented in this version of SBHSolver.

### Linear Congruential Generator

The linear congruential generator (LCG) is implemented as a bijective function that distributes the hashes more randomly across all buckets while preserving perfect hashing at the same time. It improves the performance of SBH.

### Custom Memory Manager

Custom memory manager tries to reduce the overhead of requesting memory from the system at each hash insertion. Instead, it requests a large pool of memory in one lump sum from the system and then manages allocations internally.
