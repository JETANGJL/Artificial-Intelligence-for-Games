/*!****************************************************************************
\file functions.h
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov@digipen.edu, jielejet.ang@digipen.edu.sg
\par Course: CSD3183
\par Section: A
\par Programming Assignment #2
\date 05-13-2025
\brief
This file contains the core implementation for a 2D flood-fill simulation
used in AI exploration tasks.

The main purpose of the project is to simulate how an agent would traverse
a map and fill reachable tiles, avoiding obstacles. This is commonly used
in game AI, map exploration, and region-filling problems.

The file includes:
- Classes to retrieve valid neighboring tiles (GetMapAdjacents,
  GetMapStochasticAdjacents).
- Flood fill implementations using recursive and iterative techniques.
- Stack and queue-based wrappers to switch between depth-first and
  breadth-first traversal styles.
- Interface abstraction for consistent open list usage.
- Deterministic randomization for consistent test grading.

The flood fill algorithm modifies a 2D map in-place, changing tile values
from 0 to a specified color code to mark visited regions.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stack>
#include <queue>
#include <random>
#include <algorithm>

#include "data.h"



namespace AI 
{

    /*!****************************************************************************
    \class GetMapAdjacents
    \brief Domain-specific functor that returns all valid adjacent nodes
           (up/down/left/right) for a given grid cell.

    \details
    This class checks the surrounding tiles of a cell based on whether they
    are within bounds and walkable (i.e., value is 0). It returns a list of
    newly created Node pointers representing these valid neighbors.
    *******************************************************************************/
    class GetMapAdjacents : public GetAdjacents
    {
        protected:
            int* map;
            int size;

    public:

        /*!****************************************************************************
        \brief Default constructor for GetMapAdjacents.
        \param map Pointer to a map array.
        \param size The width/height of the square map.
        ******************************************************************************/
        GetMapAdjacents(int* map = nullptr, int size = 0)
            : GetAdjacents(), map{ map }, size{ size }
        {
        }

        /*!****************************************************************************
        \brief Destructor for GetMapAdjacents.
        \details Currently does nothing but defined for completeness.
        ******************************************************************************/
        virtual ~GetMapAdjacents()
        {
        }

        /*!****************************************************************************
        \brief Returns the internal map array.
        \return Pointer to the integer map data.
        ******************************************************************************/
        int* getMap() const { return map; }
        
        /*!****************************************************************************
        \brief Returns the size (width/height) of the map.
        \return Size of the square map.
        ******************************************************************************/
        int getSize() const { return size; }

        /*!****************************************************************************
        \brief Returns valid adjacent nodes for the given key.

        \param key
        The starting coordinate (row and column) to retrieve neighbors from.

        \return
        A vector of dynamically allocated Node pointers representing adjacent walkable tiles.
        *******************************************************************************/
        std::vector<AI::Node*> operator()(Key key)
        {
            std::vector<AI::Node*> list;

            int i = key.i; // row
            int j = key.j; // col

            auto is_valid = [&](int ni, int nj) -> bool {
                return ni >= 0 && ni < size && nj >= 0 && nj < size && map[ni * size + nj] == 0;
                };

            // Correct: Key{col, row} = Key{j, i}
            if (is_valid(i - 1, j))
                list.push_back(new Node{ Key{ j, i - 1 } }); // Up
            if (is_valid(i + 1, j))
                list.push_back(new Node{ Key{ j, i + 1 } }); // Down
            if (is_valid(i, j - 1))
                list.push_back(new Node{ Key{ j - 1, i } }); // Left
            if (is_valid(i, j + 1))
                list.push_back(new Node{ Key{ j + 1, i } }); // Right

            return list;
        }
    };

    /*!****************************************************************************
    \class GetMapStochasticAdjacents
    \brief A variant of GetMapAdjacents that shuffles the adjacent nodes randomly.

    \details
    This subclass uses a fixed-seed random number generator to shuffle the order
    of adjacent nodes returned. It ensures deterministic randomness for consistent
    grading and testing.
    *******************************************************************************/
    class GetMapStochasticAdjacents : public GetMapAdjacents
    {
    public:

        /*!****************************************************************************
        \brief Constructor for GetMapStochasticAdjacents.
        \param map Pointer to the map array.
        \param size Size of the map.
        ******************************************************************************/
        GetMapStochasticAdjacents(int* map, int size)
            : GetMapAdjacents{ map, size }
        {
        }

        /*!****************************************************************************
        \brief Returns a shuffled list of valid adjacent nodes for the given key.

        \param key
        The current position to retrieve neighbors from.

        \return
        A vector of randomly ordered Node pointers representing walkable neighbors.
        *******************************************************************************/
        std::vector<AI::Node*> operator()(Key key)
        {
            std::vector<AI::Node*> list = GetMapAdjacents::operator()(key);

            // Use minstd_rand with fixed seed for grader compatibility
            static std::minstd_rand rng(0);
            std::shuffle(list.begin(), list.end(), rng);

            return list;
        }

    };

    /*!****************************************************************************
    \struct Interface
    \brief Abstract interface for open list containers used in flood fill.

    \details
    Defines the virtual interface for container operations such as clear,
    push, and pop. This allows interchangeable usage of stacks and queues
    for traversal strategies.
    *******************************************************************************/
    struct Interface
    {
        virtual void clear() = 0;

        virtual void push(Node* pNode) = 0;

        virtual Node* pop() = 0;
    };

    /*!****************************************************************************
    \struct Queue
    \brief Implements a breadth-first queue-based container for node traversal.
    *******************************************************************************/
    struct Queue : Interface
    {
        std::queue<Node*> q;

        /*!****************************************************************************
        \brief Clears the queue by deleting all stored Node pointers.
        ******************************************************************************/
        void clear() override
        {
            while (!q.empty())
            {
                delete q.front();
                q.pop();
            }
        }

        /*!****************************************************************************
        \brief Pushes a node pointer into the queue.
        \param pNode Pointer to a Node to be added.
        ******************************************************************************/
        void push(Node* pNode) override
        {
            q.push(pNode);
        }

        /*!****************************************************************************
        \brief Pops and returns the front node pointer from the queue.
        \return Pointer to the front Node, or nullptr if queue is empty.
        ******************************************************************************/
        Node* pop() override
        {
            if (q.empty())
                return nullptr;

            Node* p = q.front();
            q.pop();
            return p;
        }
    };

    /*!****************************************************************************
    \struct Stack
    \brief Implements a depth-first stack-based container for node traversal.
    *******************************************************************************/
    struct Stack : Interface
    {
        std::stack<Node*> s;

        /*!****************************************************************************
        \brief Clears the stack by deleting all stored Node pointers.
        ******************************************************************************/
        void clear() override
        {
            while (!s.empty())
            {
                delete s.top();
                s.pop();
            }
        }

        /*!****************************************************************************
        \brief Pushes a node pointer into the stack.
        \param pNode Pointer to a Node to be added.
        ******************************************************************************/
        void push(Node* pNode) override
        {
            s.push(pNode);
        }

        /*!****************************************************************************
        \brief Pops and returns the top node pointer from the stack.
        \return Pointer to the top Node, or nullptr if stack is empty.
        ******************************************************************************/
        Node* pop() override
        {
            if (s.empty())
                return nullptr;

            Node* p = s.top();
            s.pop();
            return p;
        }
    };

    /*!****************************************************************************
    \class Flood_Fill_Recursive
    \brief Performs flood fill using recursive depth-first traversal.

    \details
    Given a starting cell, this class fills all connected walkable tiles using
    a recursive approach. It avoids revisiting or coloring blocked or out-of-bound
    cells.
    *******************************************************************************/
    class Flood_Fill_Recursive
    {
        GetAdjacents* pGetAdjacents;

    public:

        /*!****************************************************************************
        \brief Constructor for Flood_Fill_Recursive.
        \param pGetAdjacents Pointer to the adjacency functor.
        ******************************************************************************/
        Flood_Fill_Recursive(GetAdjacents* pGetAdjacents)
            : pGetAdjacents{ pGetAdjacents }
        {
        }
        /*!****************************************************************************
        \brief Executes recursive flood fill from a given starting key.

        \param key
        The starting coordinate for flood fill.

        \param color
        The integer value to use when coloring filled cells.
        *******************************************************************************/
        void run(Key key, int color)
        {
            int i = key.i;
            int j = key.j;

            GetMapAdjacents* mapAdj = dynamic_cast<GetMapAdjacents*>(pGetAdjacents);
            if (!mapAdj)
                return;

            int* map = mapAdj->getMap();     
            int size = mapAdj->getSize();     

            if (i < 0 || i >= size || j < 0 || j >= size)
                return;

            if (map[i * size + j] != 0)
                return;

            map[i * size + j] = color;

            std::vector<Node*> adjacents = (*pGetAdjacents)(key);
            for (Node* node : adjacents)
            {
                run(node->key, color);
                delete node;
            }
        }
    };

    /*!****************************************************************************
    \class Flood_Fill_Iterative
    \brief Performs flood fill using iterative traversal with stack or queue.

    \details
    This templated class uses either a stack (for DFS) or a queue (for BFS)
    to simulate flood fill without recursion. It is compatible with both
    GetMapAdjacents and GetMapStochasticAdjacents for determining neighbors.
    *******************************************************************************/
    template<typename T>
    class Flood_Fill_Iterative
    {
        GetAdjacents* pGetAdjacents;
        T openlist;

    public:

        /*!****************************************************************************
        \brief Constructor for Flood_Fill_Iterative.
        \param pGetAdjacents Pointer to the adjacency functor.
        ******************************************************************************/
        Flood_Fill_Iterative(GetAdjacents* pGetAdjacents)
            : pGetAdjacents{ pGetAdjacents }, openlist{}
        {
        }

        /*!****************************************************************************
        \brief Executes iterative flood fill from a given starting key.

        \param key
        The starting coordinate for flood fill.

        \param color
        The integer value used to mark visited tiles.
        *******************************************************************************/
        void run(Key key, int color)
        {
            GetMapAdjacents* mapAdj = dynamic_cast<GetMapAdjacents*>(pGetAdjacents);
            if (!mapAdj)
                return;

            int* map = mapAdj->getMap();
            int size = mapAdj->getSize();

            openlist.clear();
            openlist.push(new Node{ key });

            while (true)
            {
                Node* current = openlist.pop();
                if (!current)
                    break;

                int i = current->key.i;
                int j = current->key.j;

                if (i < 0 || i >= size || j < 0 || j >= size)
                {
                    delete current;
                    continue;
                }

                if (map[i * size + j] == 0)
                {
                    map[i * size + j] = color;
                }

                std::vector<Node*> adj = (*pGetAdjacents)(current->key);
                for (Node* n : adj)
                    openlist.push(n);

                delete current;
            }
        }
    };

} // end namespace

#endif