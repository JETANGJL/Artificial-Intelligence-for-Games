/*!*****************************************************************************
\file       functions.cpp
\author     Vadim Surov, Jie Le Jet Ang
\par        DP email: jielejet.ang@digipen.edu.sg
\par        Course: CS3183
\par        Section: A
\par        Programming Assignment 10
\date       07-10-2025

\brief
    Implements composite and decorator classes for behavior trees used in game AI.
    Contains logic for Selector, Sequence, RandomSelector, Inverter, Succeeder,
    Repeater, Repeat_until_fail, and CheckState nodes, each overriding operator()
    to define specific execution and logging behavior.
*******************************************************************************/
#include "functions.h"

namespace AI
{
    /*!*****************************************************************************
    \brief
        Executes the CheckState node, which compares the state of a target task to a specified value. Returns Success if
        the check passes and the task is in the desired state, or Failure otherwise. All actions and results are logged
        to the provided Log stream for debugging and hierarchical output.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.

    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& CheckState::operator()(Log* log, std::string level)
    {
        if (log)
            *log << level << "CheckState(" << checktask.getId() << "," << STATES[checkstate] << ")\n";
        // Compare the initial state of checktask to checkstate
        if (checktask.getState() == checkstate)
            state = State::Success;
        else
            state = State::Failure;
        log_result(log, level, state);
        return *this;
    }

    /*!*****************************************************************************
    \brief
        Executes the Selector node, which runs each child task in order and returns Success as soon as any child succeeds.
        If all child tasks fail, returns Failure. Each childs execution and the nodes final state are logged to the output
        stream using the specified indentation level for visual tree representation.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.

    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& Selector::operator()(Log* log, std::string level)
    {
        if (log) *log << level << "Selector()\n";

        state = State::Failure;
        for (auto& t : tasks)
        {
            (*t)(log, level + "| ");
            if (t->getState() == State::Success)
            {
                state = State::Success;
                break;
            }
        }

        log_result(log, level, state);
        return *this;
    }

    /*!*****************************************************************************
    \brief
        Executes the Sequence node, which runs each child task in order and returns Failure as soon as any child fails.
        Only returns Success if all children succeed. Execution details and the nodes final result are logged with proper
        indentation to represent tree structure in the output stream.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.

    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& Sequence::operator()(Log* log, std::string level)
    {
        if (log) *log << level << "Sequence()\n";

        state = State::Success;
        for (auto& t : tasks)
        {
            (*t)(log, level + "| ");
            if (t->getState() == State::Failure)
            {
                state = State::Failure;
                break;
            }
        }

        log_result(log, level, state);
        return *this;
    }

    /*!*****************************************************************************
    \brief
        Executes the RandomSelector node, which selects one child task at random, executes it, and returns the result
        of that child. Logs the node name, the execution of the chosen child, and the resulting state, using the given
        indentation level for hierarchical formatting in the log output.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.

    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& RandomSelector::operator()(Log* log, std::string level)
    {
        if (log)
            *log << level << "RandomSelector()\n";

        if (tasks.empty())
        {
            state = State::Failure;
            log_result(log, level, state);
            return *this;
        }

        // Pick a random child (same every run if you don't seed)
        int idx = std::rand() % tasks.size();

        auto it = tasks.begin();
        std::advance(it, idx);
        (*(*it))(log, level + "| ");
        state = (*it)->getState();

        log_result(log, level, state);
        return *this;
    }

    /*!*****************************************************************************
    \brief
        Executes the Inverter decorator node, which runs its child and inverts the result: if the child returns Success,
        the inverter returns Failure, and vice versa. The output and final state are logged with proper indentation for
        tree visualization.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.
    
    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& Inverter::operator()(Log* log, std::string level)
    {
        if (log) *log << level << "Inverter()\n";
        if (!task)
        {
            state = State::Failure;
            log_result(log, level, state);
            return *this;
        }
        (*task)(log, level + "| ");
        state = (task->getState() == State::Success) ? State::Failure
            : (task->getState() == State::Failure) ? State::Success
            : task->getState();
        log_result(log, level, state);
        return *this;
    }

    /*!*****************************************************************************
    \brief
        Executes the Succeeder decorator node, which runs its child but always returns Success, regardless of the childs
        actual result. All actions, including the childs execution and the nodes final state, are logged to the output
        stream with hierarchical indentation for tree structure.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.

    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& Succeeder::operator()(Log* log, std::string level)
    {
        if (log) *log << level << "Succeeder()\n";
        if (task) (*task)(log, level + "| ");
        state = State::Success;
        log_result(log, level, state);
        return *this;
    }

    /*!*****************************************************************************
    \brief
        Executes the Repeater decorator node, which runs its child node a specified number of times and always returns
        Success. Each repetition and the final state are logged to the output stream with proper indentation to reflect
        the tree structure.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.

    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& Repeater::operator()(Log* log, std::string level)
    {
        if (log) *log << level << "Repeater(" << counter << ")\n";
        state = State::Success;
        if (counter && task)
        {
            for (int i = 0; i < counter; ++i)
                (*task)(log, level + "| ");
        }
        log_result(log, level, state);
        return *this;
    }

    /*!*****************************************************************************
    \brief
        Executes the Repeat_until_fail decorator node, which runs its child repeatedly until the child returns Failure,
        at which point this node returns Success. Each execution and the nodes final state are logged with indentation
        to reflect the behavior trees hierarchy in the output stream.

    \param log
        Pointer to the Log stream for output.

    \param level
        String prefix used for indentation and formatting of the log output, representing the current depth in the tree.

    \return
        Reference to this task after execution.
    *******************************************************************************/
    Task& Repeat_until_fail::operator()(Log* log, std::string level)
    {
        if (log) *log << level << "Repeat_until_fail()\n";
        state = State::Success;
        if (task)
        {
            while (true)
            {
                (*task)(log, level + "| ");
                if (task->getState() == State::Failure)
                    break;
            }
        }
        log_result(log, level, state);
        return *this;
    }
} // end namespace