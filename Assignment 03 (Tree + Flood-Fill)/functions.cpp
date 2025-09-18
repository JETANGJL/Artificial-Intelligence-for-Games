/*!*****************************************************************************
\file functions.cpp
\author Vadim Surov, Ang Jie Le Jet
\par DP email: vsurov@digipen.edu, jielejet.ang@digipen.edu.sg
\par Course: CSD3183
\par Section: A
\par Programming Assignment #3
\date 05-21-2025
\brief
This file contains the implementation of tree traversal logic used in
the flood-fill algorithm. It includes:
- Breadth-First Search (BFS) utility to find a TreeNode with a target value
- Any additional utility implementations needed for flood fill
*******************************************************************************/
#include "functions.h"

namespace AI 
{
    /*!*****************************************************************************
    \brief
    Performs a breadth-first search (BFS) on a tree to locate the first node
    with a specified value.

    \param root
    A reference to the root of the tree to begin the search from.

    \param value
    The target string value to search for in the tree. This should match the
    node value exactly.

    \return
    A pointer to the TreeNode containing the value, or nullptr if not found.
    ******************************************************************************/
    TreeNode* BFS(TreeNode& root, const std::string& value)
    {
        std::queue<TreeNode*> q;
        q.push(&root);

        while (!q.empty())
        {
            TreeNode* node = q.front();
            q.pop();

            if (node->value == value)
                return node;

            for (TreeNode* child : node->children)
                q.push(child);
        }

        return nullptr;
    }

} 