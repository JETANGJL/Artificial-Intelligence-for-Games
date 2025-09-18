/*!*****************************************************************************
\file functions.cpp
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov\@digipen.edu, jielejet.ang\@digipen.edu.sg
\par Course: CS3183
\par Section: A
\par Programming Assignment 4
\date 05-27-2025

\brief This file contains the implementation of the Dijkstra's algorithm.
The file includes:
- Definition of GetMapAdjacents::operator() to compute walkable adjacent tiles
  based on cardinal directions (N, S, E, W).
- Dijkstras::run which performs the shortest-path search using a min-heap
  and visited map.
- Dijkstras::getPath which reconstructs the path from the goal node back
  to the starting node using parent pointers.
*******************************************************************************/
#include "functions.h"

namespace AI
{
    /*!****************************************************************************
    \brief
    Finds valid adjacent tiles (North, South, East, West) from a given position
    on the grid and returns them as newly created nodes.

    \param key
    The current position in the grid, represented as a Key object (row, column).

    \return
    A vector of pointers to newly allocated Node objects representing all valid
    adjacent tiles that can be traversed to from the current position.
    *******************************************************************************/
    std::vector<AI::Node*> AI::GetMapAdjacents::operator()(Key key)
    {
        std::vector<Node*> list;

        int i = key[0]; // row (y)
        int j = key[1]; // column (x)

        auto is_valid = [&](int row, int col) -> bool {
            return row >= 0 && row < size && col >= 0 && col < size && map[row * size + col] == 0;
            };

        // WEST
        if (is_valid(i, j - 1))
            list.push_back(new Node{ Key{ i, j - 1 }, 10, 'W' });

        // EAST
        if (is_valid(i, j + 1))
            list.push_back(new Node{ Key{ i, j + 1 }, 10, 'E' });

        // NORTH
        if (is_valid(i - 1, j))
            list.push_back(new Node{ Key{ i - 1, j }, 10, 'N' });

        // SOUTH
        if (is_valid(i + 1, j))
            list.push_back(new Node{ Key{ i + 1, j }, 10, 'S' });

        return list;
    }

    /*!****************************************************************************
    \brief
    Runs Dijkstra's algorithm to find the shortest path from the start to the goal
    position using grid-based traversal.

    \param start
    The starting position on the map as a Key (row, column).

    \param goal
    The target/end position to reach on the map as a Key (row, column).

    \return
    A vector of direction characters ('N', 'S', 'E', 'W') representing the steps
    from the start to the goal. Returns an empty vector if no path exists or
    if start equals goal.
    *******************************************************************************/
    std::vector<char> Dijkstras::run(Key start, Key goal)
    {
        if (start == goal)
            return {};

        using P = std::pair<int, Node*>;
        std::priority_queue<P, std::vector<P>, std::greater<P>> open;
        std::unordered_map<Key, Node*, KeyHasher> visited;

        Node* startNode = new Node{ start, 0, ' ', nullptr };
        open.emplace(0, startNode);
        visited[start] = startNode;

        Node* pCurrent = nullptr;

        while (!open.empty())
        {
            Node* current = open.top().second;
            open.pop();

            if (current->key == goal)
            {
                pCurrent = current;
                break;
            }

            for (Node* neighbor : (*pGetAdjacents)(current->key))
            {
                int newCost = current->g + neighbor->g;

                auto it = visited.find(neighbor->key);
                if (it == visited.end() || newCost < it->second->g)
                {
                    neighbor->g = newCost;
                    neighbor->parent = current;
                    visited[neighbor->key] = neighbor;
                    open.emplace(newCost, neighbor);
                }
                else
                {
                    delete neighbor;
                }
            }
        }

        std::vector<char> result = getPath(pCurrent);

        // Free all Nodes
        for (auto it = visited.begin(); it != visited.end(); ++it)
            delete it->second;

        return result;
    }
    /*!****************************************************************************
    \brief
    Builds the path by walking back from the goal node to the start node using
    parent pointers and collecting movement directions along the way.

    \param pNode
    Pointer to the goal node reached during search.

    \return
    A vector of characters representing the movement directions ('N', 'S', 'E', 'W')
    from the start node to the goal node.
    *******************************************************************************/
    std::vector<char> Dijkstras::getPath(Node* pNode)
    {
        std::vector<char> path;
        while (pNode && pNode->parent)
        {
            path.push_back(pNode->info);
            pNode = pNode->parent;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

} // namespace AI
