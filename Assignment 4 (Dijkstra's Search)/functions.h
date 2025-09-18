/*!*****************************************************************************
\file functions.h
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov\@digipen.edu, jielejet.ang\@digipen.edu.sg
\par Course: CS3183
\par Section: A
\par Programming Assignment 4
\date 05-27-2025
\brief
This file contains the setup for figuring out how a character or object
can move from one point to another in a grid, using the shortest path.
It includes a helper that checks which neighboring tiles can be moved to
GetMapAdjacents, and a class Dijkstras that calculates the best path.

The file includes:
- GetMapAdjacents: A functor to compute valid adjacent nodes on a 2D grid.
- Dijkstras: A class implementing Dijkstra's shortest-path algorithm using a
  min-heap and adjacency interface.
- KeyHasher: A hash function for using Key in unordered_map.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "data.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace AI
{
    /*!****************************************************************************
    \class GetMapAdjacents
    \brief A helper that finds all valid directions to move from a given tile.

    \details
    Given a position in a grid (a tile), this class checks in all four directions
    (up, down, left, right) and returns the tiles that can be walked to.
    It's used in pathfinding to explore neighboring tiles.
    *******************************************************************************/
    class GetMapAdjacents : public GetAdjacents
    {
    protected:
        int* map;
        int size;

    public:

        /*!****************************************************************************
        \brief Constructor for GetMapAdjacents.
        \param map Pointer to a 1D array representing the square map.
        \param size The length/width of one side of the square map.
        *******************************************************************************/
        GetMapAdjacents(int* map = nullptr, int size = 0)
            : GetAdjacents(), map{ map }, size{ size }
        {
        }

        /*!****************************************************************************
        \brief Function declaration for operator() that finds walkable neighboring tiles.
        \param key The tile position to check neighbors from.
        \return A list of pointers to Nodes representing valid directions to move.
        *******************************************************************************/
        std::vector<Node*> operator()(Key key) override;
    };

    /*!****************************************************************************
    \class Dijkstras
    \brief A class that finds the shortest path in a grid using Dijkstra's algorithm.

    \details
    Dijkstra's algorithm is used to find the shortest way from a starting point
    to a target point. It keeps track of explored paths and picks the least costly one.
    *******************************************************************************/
    class Dijkstras
    {
        GetAdjacents* pGetAdjacents; //!< Pointer to a class that returns valid moves.

    public:

        /*!****************************************************************************
        \brief Constructor for Dijkstras.
        \param pGetAdjacents Pointer to an adjacency checker.
        *******************************************************************************/
        Dijkstras(GetAdjacents* pGetAdjacents)
            : pGetAdjacents(pGetAdjacents)
        {
        }

        /*!****************************************************************************
        \brief Function declaration for run, which finds the shortest path from start to target.
        \param starting The starting point in the grid.
        \param target The goal point in the grid.
        \return A list of characters (like 'N', 'S', 'E', 'W') showing the path taken.
        *******************************************************************************/
        std::vector<char> run(Key starting, Key target);

    private:

        /*!****************************************************************************
        \brief Function declaration for getPath,Builds the final movement path by tracing parent nodes.
        \param pNode The goal node to trace back from.
        \return A list of movement directions from start to goal.
        *******************************************************************************/
        std::vector<char> getPath(Node* pNode);
    };

    /*!****************************************************************************
    \struct KeyHasher
    \brief A hash function object to allow Key to be used in unordered_map.
    \comments Based on the pseudocode given this is necessary. KeyHasher accurately 
              mirrors the behavior of pseodocode HashTable.
    *******************************************************************************/
    struct KeyHasher {

        /*!****************************************************************************
        \brief operator overload in KeyHasher class to generate a unique number from a Key.
        \param k The Key object to hash.
        \return A unique size_t value used by unordered_map.
        *******************************************************************************/
        std::size_t operator()(const Key& k) const {
            std::size_t h = 0;
            for (int val : k)
                h ^= std::hash<int>()(val) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

} // namespace AI

#endif
