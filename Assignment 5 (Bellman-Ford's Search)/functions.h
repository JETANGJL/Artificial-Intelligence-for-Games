/*!*****************************************************************************
\file functions.h
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov\@digipen.edu, jielejet.ang\@digipen.edu.sg
\par Course: CS3183
\par Section: A
\par Programming Assignment 5
\date 06-07-2025
\brief
This file helps find the shortest way to get from one place to another in a grid or network.
It uses a method called Bellman-Ford, which works even when some paths have negative values.
It can also detect if there's a "bad loop" (negative cycle) that makes the path cost go lower forever.

It includes:
- BellmanFord: A class that calculates all shortest paths from a starting point
  using a cost matrix. You can also get the steps (with cost) to reach any point.
******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <array>
#include <climits>
#include <algorithm>

#include "data.h"



namespace AI
{
    const int null = -1;
    const int inf = INT_MAX;

    /*!****************************************************************************
    \class BellmanFord
    \brief A templated class implementing Bellman-Ford's shortest path algorithm.

    \templated param SIZE The size of the graph (number of nodes). Must be known at compile time.
    \details
    This class uses an adjacency cost matrix to compute the shortest path from a
    given starting node to all other nodes using the Bellman-Ford algorithm.
    It can also detect the presence of negative weight cycles.
    ******************************************************************************/
    template<int SIZE = 0>
    class BellmanFord
    {
        int* matrix;       //!< Pointer to adjacency matrix (cost)
        int* distance;     //!< Array storing minimum distance from source to each node
        int* predecessor;  //!< Array storing predecessor of each node on the shortest path

    public:

        /*!****************************************************************************
        \brief Constructor. Initializes the matrix and allocates memory.
        \param matrix Pointer to the adjacency matrix
        ******************************************************************************/
        BellmanFord(int* matrix = nullptr)
            : matrix{ matrix }
        {
            distance = new int[SIZE];
            predecessor = new int[SIZE];
        }

        /*!****************************************************************************
        \brief Destructor. Cleans up allocated memory.
        ******************************************************************************/
        ~BellmanFord()
        {
            delete[] distance;
            delete[] predecessor;
        }

        /*!****************************************************************************
        \brief Runs the Bellman-Ford algorithm from the given starting node.
        \param starting Index of the starting node
        \return True if no negative cycles are detected, false otherwise
        ******************************************************************************/
        bool run(int starting = 0)
        {

            // disable constant condition warning this is for my vs community tests, debug mode
            //#pragma warning(disable : 4127) 
            if (SIZE == 0)
                return true;


            for (int i = 0; i < SIZE; ++i)
            {
                distance[i] = inf;
                predecessor[i] = null;
            }
            distance[starting] = 0;

            for (int k = 0; k < SIZE - 1; ++k)
            {
                for (int u = 0; u < SIZE; ++u)
                {
                    for (int v = 0; v < SIZE; ++v)
                    {
                        int weight = matrix[u * SIZE + v];
                        if (weight != inf && distance[u] != inf && distance[u] + weight < distance[v])
                        {
                            distance[v] = distance[u] + weight;
                            predecessor[v] = u;
                        }
                    }
                }
            }

            for (int u = 0; u < SIZE; ++u)
            {
                for (int v = 0; v < SIZE; ++v)
                {
                    int weight = matrix[u * SIZE + v];
                    if (weight != inf && distance[u] != inf && distance[u] + weight < distance[v])
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        /*!****************************************************************************
        \brief Reconstructs the shortest path from the start to the given target.
        \param target Index of the target node
        \return A vector containing the sequence of nodes on the path
        ******************************************************************************/
        std::vector<int> getPath(int target)
        {
            std::vector<int> path;
            if (distance[target] == inf)
                return path;

            for (int at = target; at != null; at = predecessor[at])
                path.push_back(at);

            std::reverse(path.begin(), path.end());

            if (!path.empty() && path.front() == 0)
                path.erase(path.begin()); // remove the starting node

            return path;
        }

        /*!****************************************************************************
        \brief Reconstructs the full route from start to target with step costs.
        \param target Index of the target node
        \return A vector of arrays, each describing [from, to, cost] of one step
        ******************************************************************************/
        std::vector<std::array<int, 3>> getRoute(int target)
        {
            std::vector<std::array<int, 3>> route;

            // Reconstruct full path manually (including start)
            std::vector<int> path;
            if (distance[target] == inf)
                return route;

            for (int at = target; at != null; at = predecessor[at])
                path.push_back(at);

            std::reverse(path.begin(), path.end());

            for (size_t i = 1; i < path.size(); ++i)
            {
                int from = path[i - 1];
                int to = path[i];
                int cost = matrix[from * SIZE + to];
                route.push_back({ from, to, cost });
            }

            return route;
        }

        /*!****************************************************************************
        \brief Overloads the << operator for outputting distances and predecessors.
        \param os Output stream
        \param rhs BellmanFord instance
        \return The output stream with formatted result
        ******************************************************************************/
        friend std::ostream& operator<<(std::ostream& os, const BellmanFord& rhs)
        {
            os << "[";
            for (int i = 0; i < SIZE; ++i)
            {
                if (rhs.distance[i] == inf)
                    os << "inf";
                else
                    os << rhs.distance[i];

                if (i != SIZE - 1)
                    os << ",";
            }
            os << "] [";
            for (int i = 0; i < SIZE; ++i)
            {
                if (rhs.predecessor[i] == null)
                    os << "null";
                else
                    os << rhs.predecessor[i];

                if (i != SIZE - 1)
                    os << ",";
            }
            os << "]";
            return os;
        }
    };

} // end namespace

#endif