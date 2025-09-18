/*!****************************************************************************
\file functions.h
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov@digipen.edu, jielejet.ang@digipen.edu.sg
\par Course: CSD3183
\par Section: A
\par Programming Assignment #1
\date 05-05-2025
\brief
This file contains the definition and implementation of the AI::Node class
template and tree functions. It defines a templated tree node structure used
for storing hierarchical data. The file also includes functionality for
serialization/deserialization, as well as tree search algorithms such as
Breadth-First Search (BFS) and Depth-First Search (DFS).
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

#include "data.h"

namespace AI 
{
    #define UNUSED(expr) (void)expr;

    // Forward declaration of templated struct and operator overloads
    template<typename T>
    struct Node; 
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Node<T>& rhs);
    template<typename T>
    std::istream& operator>>(std::istream& is, Node<T>& rhs);

    /*!****************************************************************************
    @brief
    Represents a generic tree node with a parent and a list of children.

    @tparam T
    The type of data stored in the node.
    *******************************************************************************/
    template<typename T>
    struct Node
    {
        // Member data
        T value;
        Node* parent;
        std::list<Node*> children;

        /*!****************************************************************************
        @brief
        Constructs a Node with optional value, parent, and children.

        @param value
        The value stored in the node.

        @param parent
        Pointer to the parent node (nullptr by default).

        @param children
        List of child nodes (defaults to empty).
        *******************************************************************************/
        Node(T value = {}, Node* parent = nullptr, 
                    const std::list<Node*>& children = {})
            : value{ value }, parent{ parent }, children{ children }
        {

        }

        /*!****************************************************************************
        @brief
        Destructor that recursively deletes all child nodes.
        *******************************************************************************/
        ~Node()
        {
            for (auto child : children)
                delete child;
        }

        /*!****************************************************************************
        \brief
        Returns the path from the root node to the current node.
        
        \details
        This function traces the parent pointers from the current node up to the root
        and builds a vector of values representing the path from root to this node.
        
        \return
        A vector containing the values from the root to the current node.
        *******************************************************************************/
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

        // Friend functions declaration
        friend std::ostream& operator<< <>(std::ostream& os, const Node<T>& rhs);
        friend std::istream& operator>> <>(std::istream& is, Node<T>& rhs);
    };

    /*!****************************************************************************
    \brief
    Performs a breadth-first search (BFS) to find a node with a specific value.
    
    \param node
    The root node to begin the search from.
    
    \param lookingfor
    The value to search for in the tree.
    
    \return
    A pointer to the first node found with the matching value, or nullptr if not found.
    *******************************************************************************/
    template<typename T>
    Node<T>* BFS(Node<T>& node, const T& lookingfor)
    {
        std::queue<Node<T>*> q;
        q.push(&node);

        while (!q.empty())
        {
            Node<T>* current = q.front();
            q.pop();

            if (current->value == lookingfor)
                return current;

            for (Node<T>* child : current->children)
                q.push(child);
        }

        return nullptr; // When not found
    }

    /*!****************************************************************************
    \brief
    Performs a depth-first search (DFS) to find a node with a specific value.
    
    \param node
    The root node to begin the search from.
    
    \param lookingfor
    The value to search for in the tree.
    
    \return
    A pointer to the first node found with the matching value, or nullptr if not found.
    *******************************************************************************/
    template<typename T>
    Node<T>* DFS(Node<T>& node, const T& lookingfor)
    {
        std::stack<Node<T>*> s;
        s.push(&node);

        while (!s.empty())
        {
            Node<T>* current = s.top();
            s.pop();

            if (current->value == lookingfor)
                return current;

            // Push children in order to search right-most paths first
            for (auto it = current->children.begin(); it != current->children.end(); ++it)
            {
                s.push(*it);
            }
        }

        return nullptr;
    }

    // --- Operator Overloads ---
    
    /*!****************************************************************************
    \brief
    Serializes a node and its subtree into a formatted string for output.
    
    \param os
    The output stream to write to.
    
    \param rhs
    The node to serialize.
    
    \return
    The output stream containing the serialized tree structure.
    *******************************************************************************/
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Node<T>& rhs)
    {
        os << rhs.value << " {" << rhs.children.size() << " ";
        for (auto child : rhs.children)
        {
            os << *child;
        }
        os << "} ";
        return os;
    }
    
    /*!****************************************************************************
    \brief
    Deserializes a node and its subtree from a formatted string.
    
    \param is
    The input stream to read from.
    
    \param rhs
    The node to populate with the parsed tree structure.
    
    \return
    The input stream after reading the tree structure.
    *******************************************************************************/
    template<typename T>
    std::istream& operator>>(std::istream& is, Node<T>& rhs)
    {
        rhs.parent = nullptr;
        rhs.children.clear();

        is >> rhs.value;

        char ch;
        is >> ch;
        if (ch != '{')
            return is;

        int numChildren = 0;
        is >> numChildren;

        for (int i = 0; i < numChildren; ++i)
        {
            Node<T>* child = new Node<T>();
            is >> *child;
            child->parent = &rhs;
            rhs.children.push_back(child);
        }

        is >> ch;
        if (ch != '}')
            return is;

        return is;
    }

} // end namespace

#endif