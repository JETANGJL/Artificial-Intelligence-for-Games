/*!*****************************************************************************
\file       functions.h
\author     Vadim Surov, Jie Le Jet Ang
\par        DP email: jielejet.ang@digipen.edu.sg
\par        Course: CS3183
\par        Section: A
\par        Programming Assignment 10
\date       07-10-2025

\brief
	Declares composite and decorator classes for behavior trees used in game AI.
	Includes Selector, Sequence, RandomSelector, Inverter, Succeeder, Repeater,
	Repeat_until_fail, and CheckState, each inheriting from Task and overriding
	operator() for custom node logic and logging.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sstream>
#include <string>
#include <list>
#include <cstdlib>
#include "data.h"

#define UNUSED(x) (void)x;

namespace AI
{
	/*!*****************************************************************************
	\brief
		Logs the result state with the specified indentation level to the provided log stream.

	\param log
		Pointer to the Log stream for output (can be nullptr).

	\param level
		Indentation string for formatting log output.

	\param s
		State value to log.
	*******************************************************************************/
	inline void log_result(Log* log, const std::string& level, State s)
	{
		if (log) *log << level << "L " << STATES[s] << "\n";
	}

	// Check the state of a task comparing it with given by parameter 
	class CheckState : public Task
	{
		Task checktask;
		State checkstate;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a CheckState node to compare a given task's state to a specified value.

		\param checktask
			The task whose state will be checked.

		\param checkstate
			The desired state to compare against (default is State::Success).
		*******************************************************************************/
		CheckState(Task checktask = {}, State checkstate = State::Success)
			: Task{ "CheckState" }, checktask{ checktask }, checkstate{ checkstate }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing the CheckState node; compares target task state to a desired value and logs the result.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.

		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;
		
	};

	// Selector composite
	//     Returns immediately with a success status code 
	//     when one of its children runs successfully.
	class Selector : public Task
	{
		std::list<SMART> tasks;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a Selector node with an optional list of child tasks.

		\param tasks
			Initializer list of child SMART tasks to be added (default is empty).
		*******************************************************************************/
		Selector(std::initializer_list<SMART> tasks = {})
			: Task{ "Selector" }, tasks{ tasks }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing the Selector node; returns Success on first child Success, otherwise Failure.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.

		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;
	};

	// Sequence composite
	//     Returns immediately with a failure status code 
	//     when one of its children fails.
	class Sequence : public Task
	{
		std::list<SMART> tasks;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a Sequence node with an optional list of child tasks.

		\param tasks
			Initializer list of child SMART tasks to be added (default is empty).
		*******************************************************************************/
		Sequence(std::initializer_list<SMART> tasks = {})
			: Task{ "Sequence" }, tasks{ tasks }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing the Sequence node; returns Failure on first child Failure, otherwise Success.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.

		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;
	};

	// Random selector composite
	//     Tries a single child at random.
	class RandomSelector : public Task
	{
		std::list<SMART> tasks;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a RandomSelector node with an optional list of child tasks.

		\param tasks
			Initializer list of child SMART tasks to be added (default is empty).
		*******************************************************************************/
		RandomSelector(std::initializer_list<SMART> tasks = {})
			: Task{ "RandomSelector" }, tasks{ tasks }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing the RandomSelector node; selects and runs one child at random and logs the result.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.

		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;
		
	};

	// Inverter
	//     Invert the value returned by a task
	class Inverter : public Task
	{
		SMART task;

	public:
		/*!*****************************************************************************
		\brief
			Constructs an Inverter decorator node for the given child task.

		\param task
			SMART pointer to the child task (default is empty).
		*******************************************************************************/
		Inverter(SMART task = {})
			: Task{ "Inverter" }, task{ task }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing the Inverter decorator; inverts the child's result and logs the execution.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.

		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;
		
	};

	// Succeeder
	//     Always return success, irrespective of what the child node actually returned.
	//     These are useful in cases where you want to process a branch of a tree 
	//     where a failure is expected or anticipated, but you don’t want to 
	//     abandon processing of a sequence that branch sits on.
	class Succeeder : public Task
	{
		SMART task;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a Succeeder decorator node for the given child task.

		\param task
			SMART pointer to the child task (default is empty).
		*******************************************************************************/
		Succeeder(SMART task = {})
			: Task{ "Succeeder" }, task{ task }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing the Succeeder decorator; always returns Success, logging execution.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.
		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;

	};

	// Repeater
	//     A repeater will reprocess its child node each time its 
	//     child returns a result. These are often used at the very 
	//     base of the tree, to make the tree to run continuously. 
	//     Repeaters may optionally run their children a set number of 
	//     times before returning to their parent.
	class Repeater : public Task
	{
		SMART task;
		int counter;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a Repeater decorator node for the given child task and repetition count.

		\param task
			SMART pointer to the child task (default is empty).

		\param counter
			Number of times to repeat the child task (default is 0).
		*******************************************************************************/
		Repeater(SMART task = {}, int counter = 0)
			: Task{ "Repeater" }, task{ task }, counter{ counter }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing the Repeater decorator; runs child multiple times, always returns Success.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.

		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;
	};

	// Repeat_until_fail
	//      Like a repeater, these decorators will continue to 
	//      reprocess their child. That is until the child finally 
	//      returns a failure, at which point the repeater will 
	//      return success to its parent.
	class Repeat_until_fail : public Task
	{
		SMART task;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a Repeat_until_fail decorator node for the given child task.

		\param task
			SMART pointer to the child task (default is empty).
		*******************************************************************************/
		Repeat_until_fail(SMART task = {})
			: Task{ "Repeat_until_fail" }, task{ task }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual override for executing Repeat_until_fail; runs child until Failure, then returns Success.

		\param log
			Pointer to the Log stream for output (can be nullptr).

		\param level
			Indentation string for formatting log output.

		\return
			Reference to this task after execution.
		*******************************************************************************/
		virtual Task& operator()(Log* log = nullptr, std::string level = "") override;
	};

} // end namespace

#endif