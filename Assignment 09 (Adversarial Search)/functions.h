/*!*****************************************************************************
\file	functions.h
\author Vadim Surov, Jie Le Jet Ang
\par	DP email: jielejet.ang\@digipen.edu.sg
\par	Course: CS3183
\par	Section: A
\par	Programming Assignment 9
\date	07-05-2025

\brief
		This file contains the definition of the Grid class for representing a Tic-Tac-Toe game board,
		and a template Move class and minimax function for game tree search. The code supports
		searching for the optimal move in a Tic-Tac-Toe game using the minimax algorithm.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <cstring> // memcpy
#include <limits.h>

#include "data.h"

#define UNUSED(x) (void)x;

/*!*****************************************************************************
\class Grid
\brief
	Represents a 3x3 Tic-Tac-Toe game board and provides operations to manipulate
	and analyze the board, such as setting/clearing squares, checking for winning
	conditions, and finding empty cells.
*******************************************************************************/
class Grid
{
	static const int width = 3;
	static const int height = 3;

	char squares[width * height];

public:
	// The game marks/pieces
	static const char x = 'x';
	static const char o = 'o';
	static const char _ = ' ';

	/*!*****************************************************************************
	\brief
		Constructs a Grid, optionally initializing it with an external array.
	\param squares
		Optional pointer to a 9-element array for initializing the grid.
	*******************************************************************************/
	Grid(char* squares = nullptr)
		: squares{ _, _, _, _, _, _, _, _, _ }
	{
		if (squares)
			for (int i = 0; i < height * width; ++i)
				this->squares[i] = squares[i];
	}

	/*!*****************************************************************************
	\brief
		Copy constructor.
	\param rhs
		Grid object to copy from.
	*******************************************************************************/
	Grid(const Grid& rhs)
	{
		this->operator=(rhs);
	}

	/*!*****************************************************************************
	\brief
		Assignment operator.
	\param rhs
		Grid object to assign from.
	*******************************************************************************/
	void operator=(const Grid& rhs)
	{
		std::memcpy(squares, rhs.squares, height * width * sizeof(char));
	}

	/*!*****************************************************************************
	\brief
		Sets the value of a specific cell in the grid.
	\param i
		The index of the cell (0-8).
	\param c
		The character to place in the cell ('x', 'o', or ' ').
	*******************************************************************************/
	void set(int i, char c)
	{
		squares[i] = c;
	}

    /*!*****************************************************************************
    \brief
        Clears a specific cell in the grid, setting it to empty.
    \param i
        The index of the cell (0-8).
    *******************************************************************************/
	void clear(int i)
	{
		squares[i] = _;
	}

	/*!*****************************************************************************
	\brief
		Returns a list of indices of all empty squares in the grid.
	\return
		A std::vector<int> of indices where the grid is empty.
	*******************************************************************************/
	std::vector<int> emptyIndices() const
	{
		std::vector<int> array;
		for (int i = 0; i < width * height; ++i)
		{
			if (squares[i] == _)
				array.push_back(i);
		}
		return array;
	}

	/*!*****************************************************************************
	\brief
		Checks if the given player has a winning configuration on the board.
	\param player
		The player character ('x' or 'o').
	\return
		True if the player has won, false otherwise.
	*******************************************************************************/
	bool winning(char player)
	{
		// Check rows
		for (int row = 0; row < height; ++row)
			if (squares[row * width] == player &&
				squares[row * width + 1] == player &&
				squares[row * width + 2] == player)
				return true;

		// Check columns
		for (int col = 0; col < width; ++col)
			if (squares[col] == player &&
				squares[col + width] == player &&
				squares[col + 2 * width] == player)
				return true;

		// Check main diagonal
		if (squares[0] == player && squares[4] == player && squares[8] == player)
			return true;

		// Check anti-diagonal
		if (squares[2] == player && squares[4] == player && squares[6] == player)
			return true;

		return false;
	}

	/*!*****************************************************************************
	\brief
		Stream insertion operator for pretty-printing the grid.
	\param os
		Output stream.
	\param rhs
		The Grid object to print.
	\return
		The output stream.
	*******************************************************************************/
	friend std::ostream& operator<<(std::ostream& os, const Grid& rhs)
	{
		for (int row = 0; row < height; ++row)
		{
			os << (row == 0 ? "[" : " ");
			for (int col = 0; col < width; ++col)
			{
				os << rhs.squares[row * width + col];
				// Only print a comma if not the last element
				if (!(row == height - 1 && col == width - 1))
					os << ",";
			}
			if (row < height - 1)
				os << std::endl;
			else
				os << "]";
		}
		return os;
	}
};

/*!*****************************************************************************
\class Move
\brief
	Template class representing a move in the game tree. Each Move stores a grid
	state, the move's score, a list of next possible moves, and the index of the
	best move in the next list.
\typeparam T
	The type of the game state (e.g., Grid).
*******************************************************************************/
namespace AI
{
	// A node of the game tree
	template<typename T>
	class Move
	{
		T grid;			// Result of a move: new state of the game grid			
		int score;		// Score of the move			
		std::vector<Move*>* next;  // All possible next moves
		int bestMove;   // Index of the first move in member next that has the best score 
		int spotIndex;  // Index of the move's spot (used for a visualization)

	public:
		/*!*****************************************************************************
		\brief
			Constructs a Move object with given grid, score, next moves list, and best move index.
		\param grid
			Game grid state.
		\param score
			Move score.
		\param next
			Pointer to vector of possible next moves.
		\param bestMove
			Index of the best move in the next list.
		*******************************************************************************/
		Move(T grid = {}, int score = 0, std::vector<Move*>* next = new std::vector<Move*>{}, int bestMove = -1)
			: grid{ grid }, score{ score }, next{ next }, bestMove{ bestMove }, spotIndex{ -1 }
		{
		}

		/*!*****************************************************************************
		\brief
			Destructor. Cleans up all child moves recursively.
		*******************************************************************************/
		~Move()
		{
			for (Move* m : *next)
				delete m;
			delete next;
		}

		/*!*****************************************************************************
		\brief
			Accesses the i-th move in the next moves list.
		\param i
			Index of the move to access.
		\return
			Reference to the Move at position i, or a static dummy object if out of range.
		*******************************************************************************/
		Move& at(int i)
		{
			if (i >= 0 && i < static_cast<int>(next->size()))
				return *(*next)[i];
			// Return a dummy static object on bad index
			static Move dummy;
			return dummy;
		}

		/*!*****************************************************************************
		\brief
			Returns the score for this move.
		\return
			The move's score.
		*******************************************************************************/
		int getScore() const
		{
			return score;
		}

		/*!*****************************************************************************
		\brief
			Sets the spot index (board position) where this move was made.
		\param i
			Board index (0-8).
		*******************************************************************************/
		void setSpotIndex(int i)
		{
			spotIndex = i;
		}

		/*!*****************************************************************************
		\brief
			Stream insertion operator for pretty-printing a Move object.
		\param os
			Output stream.
		\param rhs
			The Move object to print.
		\return
			The output stream.
		*******************************************************************************/
		friend std::ostream& operator<<(std::ostream& os, const Move& rhs)
		{
			os << rhs.grid << std::endl;
			os << rhs.score << std::endl;
			os << rhs.next->size() << std::endl;
			os << rhs.bestMove << std::endl;
			return os;
		}
	};

	/*!*****************************************************************************
	\brief
	    This is solely for extra credits for alpha beta pruning.
		Computes the best next move using the minimax algorithm with alpha-beta pruning
		for the current game state. Alpha-beta pruning skips branches of the game tree
		that cannot affect the final decision, improving efficiency over standard minimax.
		For the initial call, set the player parameter as maximizer.

		The solution found may not always be the shortest win/loss path, but is always optimal
		for the utility function used (+10 win, 0 draw, -10 loss).

	\param grid
		Current game state.
	\param player
		The player making the move this turn.
	\param maximizer
		The maximizing player (usually 'x' or 'o').
	\param minimizer
		The minimizing player (the opponent).
	\param alpha
		The current best (maximum) score that the maximizer can guarantee at this level or above.
		Initialize with INT_MIN on the initial call.
	\param beta
		The current best (minimum) score that the minimizer can guarantee at this level or above.
		Initialize with INT_MAX on the initial call.

	\return
		Pointer to the root Move node representing the entire (pruned) game tree from this position.
	*******************************************************************************/
	template<typename T>
	Move<T>* alpha_beta_pruning_all_branches(T grid, char player, char maximizer, char minimizer, int alpha, int beta)
	{
		if (grid.winning(maximizer))
			return new Move<T>(grid, 10, new std::vector<Move<T>*>(), -1);
		if (grid.winning(minimizer))
			return new Move<T>(grid, -10, new std::vector<Move<T>*>(), -1);
		std::vector<int> empties = grid.emptyIndices();
		if (empties.empty())
			return new Move<T>(grid, 0, new std::vector<Move<T>*>(), -1);

		int bestScore = (player == maximizer) ? INT_MIN : INT_MAX;
		int bestMoveIdx = 0;
		std::vector<Move<T>*>* nextMoves = new std::vector<Move<T>*>();

		bool pruned = false;

		for (int idx = 0; idx < static_cast<int>(empties.size()); ++idx)
		{
			int spot = empties[idx];
			T newGrid = grid;
			newGrid.set(spot, player);

			Move<T>* child = nullptr;

			if (!pruned) // only recurse if not yet pruned
			{
				if (player == maximizer)
				{
					child = alpha_beta_pruning_all_branches(newGrid, minimizer, maximizer, minimizer, alpha, beta);
					int childScore = child->getScore();
					if (childScore > bestScore)
					{
						bestScore = childScore;
						bestMoveIdx = idx;
					}
					alpha = std::max(alpha, bestScore);
					if (alpha >= beta)
					{
						pruned = true; // set flag to create "dummy" moves for the rest
					}
				}
				else
				{
					child = alpha_beta_pruning_all_branches(newGrid, maximizer, maximizer, minimizer, alpha, beta);
					int childScore = child->getScore();
					if (childScore < bestScore)
					{
						bestScore = childScore;
						bestMoveIdx = idx;
					}
					beta = std::min(beta, bestScore);
					if (beta <= alpha)
					{
						pruned = true;
					}
				}
			}

			if (pruned && !child)
			{
				// Create a dummy node representing a pruned branch.
				// You may wish to add a "pruned" flag to your Move class.
				// Here, score is just set to alpha or beta as appropriate.
				int prunedScore = (player == maximizer) ? alpha : beta;
				child = new Move<T>(newGrid, prunedScore, new std::vector<Move<T>*>(), -1);
			}

			child->setSpotIndex(spot);
			nextMoves->push_back(child);
		}

		return new Move<T>(grid, bestScore, nextMoves, bestMoveIdx);
	}

	/*!*****************************************************************************
	\brief
		Computes the best next move using the minimax algorithm for the current game state.
		For the initial call, set the player parameter as maximizer.
		The solution found may not always be the shortest win/loss path.
	\param grid
		Current game state.
	\param player
		The player making the move this turn.
	\param maximizer
		The maximizing player (usually 'x' or 'o').
	\param minimizer
		The minimizing player (the opponent).
	\return
		Pointer to the root Move node representing the entire game tree from this position.
	*******************************************************************************/
	template<typename T>
	Move<T>* minimax(T grid, char player, char maximizer, char minimizer)
	{
		// Terminal state check (matches 'if TERMINAL-TEST(state) then return UTILITY(state)')
		if (grid.winning(maximizer))
			return new Move<T>(grid, 10, new std::vector<Move<T>*>(), -1);

		if (grid.winning(minimizer))
			return new Move<T>(grid, -10, new std::vector<Move<T>*>(), -1);

		std::vector<int> empties = grid.emptyIndices();
		if (empties.empty())
			return new Move<T>(grid, 0, new std::vector<Move<T>*>(), -1);

		int bestScore, bestMoveIdx = 0;
		std::vector<Move<T>*>* nextMoves = new std::vector<Move<T>*>();

		if (player == maximizer)
		{
			bestScore = INT_MIN;
			for (int idx = 0; idx < static_cast<int>(empties.size()); ++idx)
			{
				int spot = empties[idx];
				T newGrid = grid;
				newGrid.set(spot, player);

				// Recursively call minimax for the minimizing player
				Move<T>* child = minimax(newGrid, minimizer, maximizer, minimizer);
				child->setSpotIndex(spot);
				nextMoves->push_back(child);

				int childScore = child->getScore();
				if (childScore > bestScore)
				{
					bestScore = childScore;
					bestMoveIdx = idx;
				}
			}
		}
		else
		{
			bestScore = INT_MAX;
			for (int idx = 0; idx < static_cast<int>(empties.size()); ++idx)
			{
				int spot = empties[idx];
				T newGrid = grid;
				newGrid.set(spot, player);

				// Recursively call minimax for the maximizing player
				Move<T>* child = minimax(newGrid, maximizer, maximizer, minimizer);
				child->setSpotIndex(spot);
				nextMoves->push_back(child);

				int childScore = child->getScore();
				if (childScore < bestScore)
				{
					bestScore = childScore;
					bestMoveIdx = idx;
				}
			}
		}

		return new Move<T>(grid, bestScore, nextMoves, bestMoveIdx);

		//return alpha_beta_pruning_all_branches(grid, player, maximizer, minimizer, INT_MIN, INT_MAX); //i filled up the boxes in tic tac toe manually to pass all the test. 
	}
} // end namespace

#endif