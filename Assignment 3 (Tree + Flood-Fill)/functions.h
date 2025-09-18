/*!****************************************************************************
\file functions.h
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov@digipen.edu, jielejet.ang@digipen.edu.sg
\par Course: CSD3183
\par Section: A
\par Programming Assignment #3
\date 05-21-2025
\brief
Contains the class definitions and function declarations used for
tree-based and flood-fill operations which were inclusive in assignment 1 and assignment 2. It includes:
- A templated Node structure for general tree construction
- Adjacent node retrievers (GetTreeAdjacents and GetTreeStochasticAdjacents)
- Flood fill algorithms using recursive and iterative approaches
- Interface abstractions for stack and queue-based traversals
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <random>
#include <functional>
#include <queue>

#include "data.h"



namespace AI
{
    /*!*****************************************************************************
    \brief
    A templated tree node class storing a value, pointer to its parent, and a list of children.

    \tparam T
    The type of value stored in the node.
    *****************************************************************************/
    template<typename T>
    struct Node
    {
        T value;
        Node* parent;
        std::list<Node*> children;

        /*!*************************************************************************
        \brief
        Constructs a new Node object.

        \param value
        The value to store in the node.
        \param parent
        Pointer to the parent node.
        \param children
        A list of child node pointers.
        *************************************************************************/
        Node(T value = {}, Node* parent = nullptr, const std::list<Node*>& children = {})
            : value{ value }, parent{ parent }, children{ children }
        {
        }

        /*!*************************************************************************
        \brief
        Destructor. Recursively deletes child nodes.
        *************************************************************************/
        ~Node()
        {
            for (auto child : children)
                delete child;
        }

        /*!*************************************************************************
        \brief
        Serializes a node to an output stream.

        \param os
        Output stream reference.
        \param rhs
        The node to serialize.
        \return
        Output stream reference after writing.
        *************************************************************************/
        friend std::ostream& operator<<(std::ostream& os, const Node<T>& rhs)
        {
            os << rhs.value << " {" << rhs.children.size() << " ";
            for (auto* child : rhs.children)
                os << *child;
            os << "} ";
            return os;
        }

        /*!*************************************************************************
        \brief
        Deserializes a node from an input stream.

        \param is
        Input stream reference.
        \param rhs
        The node to populate.
        \return
        Input stream reference after reading.
        *************************************************************************/
        friend std::istream& operator>>(std::istream& is, Node<T>& rhs)
        {
            rhs.parent = nullptr;
            rhs.children.clear();

            is >> rhs.value;

            char brace;
            is >> brace;
            if (brace != '{') return is;

            int count;
            is >> count;

            for (int i = 0; i < count; ++i)
            {
                Node<T>* child = new Node<T>;
                is >> *child;
                child->parent = &rhs;
                rhs.children.push_back(child);
            }

            is >> brace;
            return is;
        }

        /*!*************************************************************************
        \brief
        Returns the path from the root to this node as a vector of values.

        \return
        Vector of node values from the root to this node.
        *************************************************************************/
        std::vector<T> getPath() const
        {
            std::vector<T> path;
            const Node<T>* current = this;

            while (current)
            {
                path.push_back(current->value);
                current = current->parent;
            }

            std::reverse(path.begin(), path.end());
            return path;
        }
    };

    using TreeNode = Node<std::string>;

    /*!****************************************************************************
    \brief
    Abstract base class for getting adjacent nodes.
    ******************************************************************************/
    class GetAdjacents
    {
    public:
        virtual ~GetAdjacents() {}

        /*!*************************************************************************
        \brief
        Returns a list of adjacent TreeNode pointers.

        \param pNode
        Pointer to the current TreeNode.

        \return
        A vector of adjacent TreeNode pointers.
        *************************************************************************/
        virtual std::vector<TreeNode*> operator()(TreeNode* pNode) = 0;
    };

    /*!****************************************************************************
    \brief
    Retrieves direct child TreeNodes with value "x".
    ******************************************************************************/
    class GetTreeAdjacents : public GetAdjacents
    {
    public:
        /*!*************************************************************************
        \brief
        Default constructor for GetTreeAdjacents.

        This constructor initializes an instance of GetTreeAdjacents, which is used to
        retrieve adjacent child nodes whose value is "x".

        \details
        The base class GetAdjacents is also initialized using its default constructor.
        *************************************************************************/
        GetTreeAdjacents() : GetAdjacents() {}

        /*!*************************************************************************
        \brief
        Returns direct children with value "x".

        \param pNode
        Pointer to the TreeNode.

        \return
        Vector of TreeNode pointers.
        *************************************************************************/
        std::vector<TreeNode*> operator()(TreeNode* pNode) override
        {
            std::vector<TreeNode*> result;

            for (TreeNode* child : pNode->children)
            {
                if (child->value == "x")
                {
                    result.push_back(child);
                }
            }

            return result;
        }
    };

    /*!****************************************************************************
    \brief
    Retrieves and shuffles adjacent nodes with value "x" using fixed RNG.
    ******************************************************************************/
    class GetTreeStochasticAdjacents : public GetTreeAdjacents
    {
    public:
        /*!*************************************************************************
        \brief
        Default constructor for GetTreeStochasticAdjacents.

        This constructor initializes an instance of GetTreeStochasticAdjacents,
        which extends GetTreeAdjacents by returning adjacent nodes in a
        randomized order.

        \details
        The base class GetTreeAdjacents is also initialized using its default constructor.
        *************************************************************************/
        GetTreeStochasticAdjacents() : GetTreeAdjacents() {}

        /*!*************************************************************************
        \brief
        Shuffles result of GetTreeAdjacents.

        \param pNode
        Pointer to TreeNode.

        \return
        Vector of shuffled TreeNode pointers.
        *************************************************************************/
        std::vector<TreeNode*> operator()(TreeNode* pNode) override
        {
            std::vector<TreeNode*> list = GetTreeAdjacents::operator()(pNode);

            static std::minstd_rand rng(0); // Required for deterministic grading
            std::shuffle(list.begin(), list.end(), rng);

            return list;
        }
    };

    /*!****************************************************************************
    \brief
    Struct Interface for generic container to abstract Stack or Queue.
    ******************************************************************************/
    struct Interface
    {
        virtual void clear() = 0;
        virtual void push(TreeNode* pNode) = 0;
        virtual TreeNode* pop() = 0;
    };

    /*!****************************************************************************
    \brief
    Struct Queue wrapper class implementing Interface.
    ******************************************************************************/
    struct Queue : Interface
    {
        std::queue<TreeNode*> q;

        /*!**************************************************************************
        \brief
        Clears the internal queue by popping all elements.
        ***************************************************************************/
        void clear() override
        {
            while (!q.empty()) q.pop();
        }

        /*!**************************************************************************
        \brief
        Pushes a TreeNode pointer into the internal queue.

        \param pNode
        Pointer to the TreeNode to enqueue.
        ***************************************************************************/
        void push(TreeNode* pNode) override
        {
            q.push(pNode);
        }

        /*!**************************************************************************
        \brief
        Removes and returns the front TreeNode in the queue.

        \return
        Pointer to the front TreeNode, or nullptr if the queue is empty.
        ***************************************************************************/
        TreeNode* pop() override
        {
            if (q.empty()) return nullptr;
            TreeNode* node = q.front();
            q.pop();
            return node;
        }
    };

    /*!****************************************************************************
    \brief
    Struct Stack wrapper class implementing Interface.
    ******************************************************************************/
    struct Stack : Interface
    {
        std::stack<TreeNode*> s;

        /*!**************************************************************************
        \brief
        Clears the internal stack by popping all elements.
        ***************************************************************************/
        void clear() override
        {
            while (!s.empty()) s.pop();
        }

        /*!**************************************************************************
        \brief
        Pushes a TreeNode pointer onto the internal stack.

        \param pNode
        Pointer to the TreeNode to be pushed onto the stack.
        ***************************************************************************/
        void push(TreeNode* pNode) override
        {
            s.push(pNode);
        }

        /*!**************************************************************************
        \brief
        Removes and returns the top TreeNode from the stack.

        \return
        Pointer to the top TreeNode, or nullptr if the stack is empty.
        ***************************************************************************/
        TreeNode* pop() override
        {
            if (s.empty()) return nullptr;
            TreeNode* node = s.top();
            s.pop();
            return node;
        }
    };

    /*!****************************************************************************
    \brief
    Performs a breadth-first search for a node with matching value. It is a function 
    declaration

    \param root
    The root node to start searching from.
    \param value
    The string value to match.
    \return
    A pointer to the first node with matching value or nullptr if not found.
    ******************************************************************************/
    TreeNode* BFS(TreeNode& root, const std::string& value);  // Declaration only

    /*!****************************************************************************
    \brief
    Class for recursive flood fill from node with value "x".
    ******************************************************************************/
    class Flood_Fill_Recursive
    {
        GetAdjacents* pGetAdjacents;

    public:
        /*!*************************************************************************
        \brief
        Constructor that accepts a GetTreeAdjacents pointer and casts it to
        the base class GetAdjacents.

        \param adj
        Pointer to a GetTreeAdjacents object used to retrieve adjacent "x" nodes.
        *************************************************************************/
        Flood_Fill_Recursive(GetTreeAdjacents* adj)
            : pGetAdjacents{ static_cast<GetAdjacents*>(adj) } {}

        /*!*************************************************************************
        \brief
        Starts flood fill replacing "x" nodes with given value recursively.

        \param pNode
        The starting node.
        \param value
        Replacement string value.
        *************************************************************************/
        void run(TreeNode* pNode, std::string value)
        {
            if (!pNode)
                return;

            // If root is not "x", find the correct starting point
            if (pNode->value != "x")
            {
                pNode = BFS(*pNode, "x");
                if (!pNode) return;
            }

            if (pNode->value != "x")
                return;

            pNode->value = value;

            std::vector<TreeNode*> neighbors = (*pGetAdjacents)(pNode);

            for (TreeNode* neighbor : neighbors)
            {
                run(neighbor, value);
            }
        }
    };

    /*!****************************************************************************
    \brief
    Templated class for iterative flood fill using stack or queue.
    ******************************************************************************/
    template<typename T>
    class Flood_Fill_Iterative
    {
        GetAdjacents* pGetAdjacents;
        T openlist;

    public:
        /*!*************************************************************************
        brief
        Constructor that accepts a GetAdjacents pointer and initializes the open list.

        \param adj
        Pointer to an object used to retrieve adjacent "x" nodes.
        *************************************************************************/
        Flood_Fill_Iterative(GetAdjacents* adj)
            : pGetAdjacents{ adj }, openlist{} {}

        /*!*************************************************************************
        \brief
        Iteratively replaces "x" nodes using given strategy.

        \param pNode
        Start TreeNode pointer.
        \param value
        Replacement value string.
        *************************************************************************/
        void run(TreeNode* pNode, std::string value)
        {
            if (!pNode)
                return;

            // If root is not "x", find the correct starting point
            if (pNode->value != "x")
            {
                pNode = BFS(*pNode, "x");
                if (!pNode) return;
            }

            openlist.clear();
            openlist.push(pNode);

            while (TreeNode* current = openlist.pop())
            {
                if (current->value != "x")
                    continue;

                current->value = value;

                std::vector<TreeNode*> neighbors = (*pGetAdjacents)(current);
                for (TreeNode* neighbor : neighbors)
                {
                    openlist.push(neighbor);
                }
            }
        }
    };

} // namespace AI

#endif
