/*!*****************************************************************************
\file functions.h
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov\@digipen.edu, jielejet.ang\@digipen.edu.sg
\par Course: CS3183
\par Section: A
\par Programming Assignment 5
\date 06-27-2025
\brief
This file defines the domain-specific functor classes and the generic backtracking
template used to solve 1D and 2D Sudoku puzzles.

The backtracking algorithm works as a constraint satisfaction search and uses
functors to retrieve the next empty location and the next valid candidate at each
step of the search. The Sudoku solver uses this to fill in all empty cells with
valid values according to Sudoku rules.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <stack>
#include <algorithm>

#include "data.h"

#define UNUSED(x) (void)x;

/*!*****************************************************************************
\class NextLocation_Sudoku1D

\brief
    This is a domain specific functor class that is used to find a solution
    for a one-dimensional Sudoku by using Backtracking algorithm.
    Functor returns next location which is not occupied yet on the map.
    If no location found, returns Location<>{ nullptr, 0 }.
*******************************************************************************/
class NextLocation_Sudoku1D
{
    AI::MapInt1D* map;

public:
    /*!*****************************************************************************
    \brief
        Constructs the next location functor for 1D Sudoku.

    \param map
        Pointer to the 1D Sudoku map.
    *******************************************************************************/
    NextLocation_Sudoku1D(void* map)
        : map{ static_cast<AI::MapInt1D*>(map) }
    {
    }

    /*!*****************************************************************************
    \brief  
        Finds the next empty cell (value 0) in a 1D Sudoku grid.

    \return 
        A Location object with the base pointer and index of the next empty cell.
        If no empty cell exists, returns Location with nullptr base.
    *******************************************************************************/
    AI::Location<> operator()() const
    {
        for (int i = 0; i < map->size; ++i)
        {
            if (map->base[i] == 0)
                return AI::Location<int>{ map->base, i };
        }
        return AI::Location<int>{ nullptr, 0 };
    }
};

// This is a domain specific functor class that is used to find a solution 
// for a two-dimensional Sudoku by using Backtracking algorithm.
// Functor returns next location which is not occupied yet on the map.
// If no location found, returns Location<>{ nullptr, 0 }.
class NextLocation_Sudoku2D
{
    AI::MapInt2D* map;

public:
    /*!*****************************************************************************
    \brief
        Constructs the next location functor for 2D Sudoku.

    \param map
        Pointer to the 2D Sudoku map.
    *******************************************************************************/
    NextLocation_Sudoku2D(void* map)
        : map{ static_cast<AI::MapInt2D*>(map) }
    {
    }

    /*!*****************************************************************************
    \brief  
        Finds the next empty cell (value 0) in a 2D Sudoku grid using row-major order.

    \return 
        A Location object with the base pointer and index of the next empty cell.
        If no empty cell exists, returns Location with nullptr base.
    *******************************************************************************/
    AI::Location<> operator()() const
    {
        for (int j = 0; j < map->height; ++j)
        {
            for (int i = 0; i < map->width; ++i)
            {
                int index = j * map->width + i;
                if (map->base[index] == 0)
                    return AI::Location<>{ map->base, index };
            }
        }

        return AI::Location<>{ nullptr, 0 };
    }
};

/*!*****************************************************************************
\class NextCandidate_Sudoku1D

\brief
    This is a domain specific functor class that is used to find a solution
    for a one-dimensional Sudoku by using Backtracking algorithm.
    Functor returns next candidate for a specified location on the map.
    If no candidate found, returns 0.
*******************************************************************************/
class NextCandidate_Sudoku1D
{
    AI::MapInt1D* map;

public:
    /*!*****************************************************************************
    \brief
        Constructs the next candidate functor for 1D Sudoku.

    \param map
        Pointer to the 1D Sudoku map.
    *******************************************************************************/
    NextCandidate_Sudoku1D(void* map)
        : map{ static_cast<AI::MapInt1D*>(map) }
    {
    }

    /*!*****************************************************************************
    \brief  
        Finds the next valid candidate value for a given location in 1D Sudoku.

    \param  location 
        The location of the cell to assign a candidate.

    \return 
        The next valid number that doesn't exist in the array.
        Returns 0 if no valid candidate is available.
    *******************************************************************************/
    int operator()(AI::Location<> location)
    {
        int index = location.getIndex();

        // Try the next value greater than current value
        for (int val = map->base[index] + 1; val <= 9; ++val)
        {
            bool exists = false;

            for (int i = 0; i < map->size; ++i)
            {
                if (map->base[i] == val)
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
            {
                map->base[index] = val; // assign the candidate
                return val;
            }
        }

        map->base[index] = 0; // reset
        return 0;
    }
};

/*!*****************************************************************************
\class NextCandidate_Sudoku2D

\brief
    This is a domain specific functor class that is used to find a solution
    for a two-dimensional Sudoku by using Backtracking algorithm.
    Functor returns next candidate for a specified location on the map.
    If no candidate found, returns 0.
*******************************************************************************/
class NextCandidate_Sudoku2D
{
    AI::MapInt2D* map;

public:

    /*!*****************************************************************************
    \brief
        Constructs the next candidate functor for 2D Sudoku.

    \param map
        Pointer to the 2D Sudoku map.
    *******************************************************************************/
    NextCandidate_Sudoku2D(void* map)
        : map{ static_cast<AI::MapInt2D*>(map) }
    {
    }

    /*!*****************************************************************************
    \brief  
        Finds the next valid candidate value for a given location in 2D Sudoku.

    \param  location 
        The location of the cell to assign a candidate.

    \return     
        The next valid number that doesn't appear in the same row, column, or 3x3 subgrid.
        Returns 0 if no valid candidate is available.
    *******************************************************************************/
    int operator()(AI::Location<> location)
    {
        int index = location.getIndex();
        int row = index / map->width;
        int col = index % map->width;

        for (int val = map->base[index] + 1; val <= 9; ++val)
        {
            bool exists = false;

            // Check row
            for (int i = 0; i < map->width; ++i)
            {
                if (map->base[row * map->width + i] == val)
                {
                    exists = true;
                    break;
                }
            }

            // Check column
            if (!exists)
            {
                for (int j = 0; j < map->height; ++j)
                {
                    if (map->base[j * map->width + col] == val)
                    {
                        exists = true;
                        break;
                    }
                }
            }

            // Check 3x3 box
            if (!exists)
            {
                int box_row = row / 3 * 3;
                int box_col = col / 3 * 3;

                for (int j = 0; j < 3; ++j)
                {
                    for (int i = 0; i < 3; ++i)
                    {
                        int check_idx = (box_row + j) * map->width + (box_col + i);
                        if (map->base[check_idx] == val)
                        {
                            exists = true;
                            break;
                        }
                    }
                    if (exists) break;
                }
            }

            // If not found anywhere, it's valid
            if (!exists)
            {
                map->base[index] = val;
                return val;
            }
        }

        map->base[index] = 0; // reset if nothing valid
        return 0;
    }
};


namespace AI
{ 

/*!*****************************************************************************
\class Backtracking
\brief
    Template class of the Backtracking algorithm.

\templated NL
    NL defines domain-specific NextLocation functor.

\templated NC
    Parameter NC defines domain-specific NextCandidate functor. 
*******************************************************************************/
    template<typename NL, typename NC>
    class Backtracking
    {
        std::stack<Location<>> stack;
        NL next_location;
        NC next_candidate;
 
    public:
        /*!*****************************************************************************
        \brief
            Constructs the backtracking solver with domain-specific functors.

        \param map
            Pointer to the Sudoku map.
        *******************************************************************************/
        Backtracking(void* map = nullptr)
            : next_location{ map }, next_candidate{ map }
        {
        }

        /*!*****************************************************************************
        \brief 
            Runs the backtracking solver in a blocking mode until completion or failure.
        *******************************************************************************/
        void run()
        {
            while (solve());
        }

        /*!*****************************************************************************
        \brief 
            Performs a single step of the backtracking algorithm.
            If the current location has a valid candidate, pushes the next location onto the stack.
            If no candidate is found, it backtracks by popping the current location.

        \return 
            True if the step was performed. False if the puzzle is solved or unsolvable.
        *******************************************************************************/
        bool solve()
        {
            if (stack.empty())
            {
                Location<> loc = next_location();
                if (loc.notFound())
                    return false; // nothing to solve

                stack.push(loc);
            }

            Location<>& current = stack.top();
            int val = next_candidate(current);

            if (val == 0)
            {
                current.clearValue(); // undo current
                stack.pop();          // backtrack
            }
            else
            {
                Location<> next = next_location();
                if (next.notFound())
                    return false; // puzzle complete

                stack.push(next); // explore next
            }

            return true;
        }
    };

} // end namespace

#endif