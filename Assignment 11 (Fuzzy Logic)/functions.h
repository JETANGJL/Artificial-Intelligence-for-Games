/*!*****************************************************************************
\file      functions.h
\author    Vadim Surov, Jie Le Jet Ang
\par       DP email: jielejet.ang@digipen.edu.sg
\par       Course: CS3183
\par       Section: A
\par       Programming Assignment 11
\date      07-19-2025

\brief
	Declares and defines classes and helpers for implementing fuzzy logic systems in AI.
	Includes FuzzySet, FuzzyOperator, FuzzyVariable,
	FuzzyRule, and FuzzyModule for variable fuzzification, fuzzy rule evaluation,
	and defuzzification methods (max average, centroid).
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <list>
#include <map>
#include <limits>
#include <cfloat> // FLT_EPSILON, ...
#include <memory> // shared_ptr


#include "data.h"

#define UNUSED(x) (void)x;

namespace AI
{
	/*!*****************************************************************************
	\brief
		Compares two floating-point values for approximate equality using an epsilon threshold.
	\param a
		First floating-point value to compare.
	\param b
		Second floating-point value to compare.
	\return
		True if the values are considered equal within floating-point precision, false otherwise.
	*******************************************************************************/
	inline bool isEqual(float a, float b)
	{
		float epsilon = 128 * FLT_EPSILON;
		float abs_th = FLT_MIN;

		if (a == b) return true;

		float diff = std::abs(a - b);
		float norm = std::min((std::abs(a) + std::abs(b)), std::numeric_limits<float>::max());

		return diff < std::max(abs_th, epsilon* norm);
	}


	//  Definition of the base fuzzy set class
	class FuzzySet
	{
	protected:
		// Members that define the shape
		float peakPoint;
		float leftOffset;
		float rightOffset;

		// representativeValue - the maximum of the set's membership function. For instance, if
		// the set is triangular then this will be the peak point of the triangular.
		// if the set has a plateau then this value will be the mid point of the 
		// plateau. This value is set in creation to avoid run-time
		// calculation of mid-point values.
		float representativeValue;

		// DOM - holds the degree of membership of a given value in this set
		float DOM;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a FuzzySet base class with shape-defining parameters and a representative value.
		\param peakPoint
			The peak (max membership) of the fuzzy set.
		\param leftOffset
			The distance from peakPoint to the left edge.
		\param rightOffset
			The distance from peakPoint to the right edge.
		\param representativeValue
			The characteristic value of the set (for centroid defuzzification).
		*******************************************************************************/
		FuzzySet(float peakPoint, float leftOffset, float rightOffset, float representativeValue)
			: peakPoint{ peakPoint }, leftOffset{ leftOffset }, rightOffset{ rightOffset },
				representativeValue{ representativeValue }, DOM{ 0.0f }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual destructor for FuzzySet; allows safe deletion of derived fuzzy sets.
		*******************************************************************************/
		virtual ~FuzzySet()
		{
		}

		// calculateDOM - calculates and returns the degree of membership for a particular value
		// NOTE, this does not set DOM to the value passed as the parameter.
		// This is because the centroid defuzzification method also uses this method
		// to determine the degree of memberships of the values it uses as its sample points.

		/*!*****************************************************************************
		\brief
			Calculates and returns the degree of membership (DOM) for a value in this set.
			Does not change the stored DOM.
		\param val
			The value for which to compute membership.
		\return
			Degree of membership in [0, 1].
		*******************************************************************************/
		virtual float calculateDOM(float val) const
		{
			UNUSED(val);

			return 0.0f;
		}

		/*!*****************************************************************************
		\brief
			Clears the stored degree of membership (DOM) for this set (sets it to 0).
		*******************************************************************************/
		void clearDOM() 
		{ 
			DOM = 0.0f; 
		}

		/*!*****************************************************************************
		\brief
			Returns the current degree of membership (DOM) for this set.
		\return
			The current DOM value.
		*******************************************************************************/
		float getDOM() const 
		{ 
			return DOM; 
		}

		/*!*****************************************************************************
		\brief
			Sets the degree of membership (DOM) for this set.
		\param val
			New DOM value to store.
		*******************************************************************************/
		void setDOM(float val)
		{ 
			DOM = val; 
		}

		/*!*****************************************************************************
		\brief
			Returns the representative (characteristic) value of the set, used for defuzzification.
		\return
			The representative value.
		*******************************************************************************/
		float getRepresentativeValue() const
		{
			return representativeValue;
		}

		// ORwithDOM - If this fuzzy set is part of a consequent floatiable,  
		// and it is fired by a rule then this method sets the DOM (in this 
		// context, the DOM represents a confidence level) to the maximum  
		// of the parameter value or the set's existing member DOM value

		/*!*****************************************************************************
		\brief
			Updates DOM to the maximum of the argument and the current DOM.
		\param val
			Value to compare with the current DOM.
		*******************************************************************************/
		void ORwithDOM(float val)
		{
			if (val > DOM)
				DOM = val;
		}

		/*!*****************************************************************************
		\brief
			Calculates and sets the DOM for the given value, and returns this set pointer.
		\param val
			Value to fuzzify.
		\return
			Pointer to this set after updating DOM.
		*******************************************************************************/
		FuzzySet* fuzzify(float val)
		{
			DOM = calculateDOM(val);
			return this;
		}
	};

	//  Definition of a fuzzy set that has a left shoulder shape.
	class FuzzySet_LeftShoulder : public FuzzySet
	{
	public:
		/*!*****************************************************************************
		\brief
			Constructs a left shoulder fuzzy set with specified peak and width parameters.
		\param peakPoint
			The top of the shoulder.
		\param leftOffset
			Distance to the left base.
		\param rightOffset
			Distance to the right base.
		*******************************************************************************/
		FuzzySet_LeftShoulder(float peakPoint, float leftOffset, float rightOffset)
			: FuzzySet(peakPoint, leftOffset, rightOffset, peakPoint - leftOffset / 2)
		{
		}

		/*!*****************************************************************************
		\brief
			Calculates the DOM for the left shoulder fuzzy set shape.
		\param val
			Value to evaluate.
		\return
			Degree of membership for the value.
		*******************************************************************************/
		float calculateDOM(float val) const
		{
			// Prevent divide by zero
			if (isEqual(peakPoint, val) &&
				(isEqual(leftOffset, 0.0f) || isEqual(rightOffset, 0.0f)))
				return 1.0f;

			// Left of peak: full membership
			if (val < peakPoint && val >= peakPoint - leftOffset)
				return 1.0f;

			// Right of peak: linear decrease
			if (val >= peakPoint && val < peakPoint + rightOffset)
				return (1.0f / -rightOffset) * (val - peakPoint) + 1.0f;

			// Out of range
			return 0.0f;
		}

	};

	// Definition of a fuzzy set that has a right shoulder shape.
	class FuzzySet_RightShoulder : public FuzzySet
	{
	public:
		/*!*****************************************************************************
		\brief
			Constructs a right shoulder fuzzy set with specified peak and width parameters.
		\param peakPoint
			The top of the shoulder.
		\param leftOffset
			Distance to the left base.
		\param rightOffset
			Distance to the right base.
		*******************************************************************************/
		FuzzySet_RightShoulder(float peakPoint, float leftOffset, float rightOffset)
			: FuzzySet(peakPoint, leftOffset, rightOffset, peakPoint + rightOffset / 2)
		{
		}

		/*!*****************************************************************************
		\brief
			Calculates the DOM for the right shoulder fuzzy set shape.
		\param val
			Value to evaluate.
		\return
			Degree of membership for the value.
		*******************************************************************************/
		float calculateDOM(float val) const
		{
			// Prevent divide by zero
			if (isEqual(peakPoint, val) &&
				(isEqual(leftOffset, 0.0f) || isEqual(rightOffset, 0.0f)))
				return 1.0f;

			// Left of peak: linear increase
			if (val <= peakPoint && val > peakPoint - leftOffset)
				return (1.0f / leftOffset) * (val - (peakPoint - leftOffset));

			// Right of peak: full membership
			if (val > peakPoint && val <= peakPoint + rightOffset)
				return 1.0f;

			// Out of range
			return 0.0f;
		}

	};

	// This defines a fuzzy set that is a singleton (a range
	// over which the DOM is always 1.0f)
	class FuzzySet_Singleton : public FuzzySet
	{
	public:
		/*!*****************************************************************************
		\brief
			Constructs a singleton fuzzy set, where membership is 1.0 in a narrow range.
		\param peakPoint
			The center of the singleton.
		\param leftOffset
			Distance to the left edge.
		\param rightOffset
			Distance to the right edge.
		*******************************************************************************/
		FuzzySet_Singleton(float peakPoint, float leftOffset, float rightOffset)
			: FuzzySet(peakPoint, leftOffset, rightOffset, peakPoint)
		{
		}

		/*!*****************************************************************************
		\brief
			Calculates the DOM for a singleton fuzzy set (always 1.0 within the range).
		\param val
			Value to evaluate.
		\return
			Degree of membership for the value.
		*******************************************************************************/
		float calculateDOM(float val) const
		{
			if (val >= peakPoint - leftOffset && val <= peakPoint + rightOffset)
				return 1.0f;
			return 0.0f;
		}
	};

	// This is a simple class to define fuzzy sets that have a triangular 
	// shape and can be defined by a mid point, a left displacement and a
	// right displacement. 
	class FuzzySet_Triangle : public FuzzySet
	{
	public:
		/*!*****************************************************************************
		\brief
			Constructs a triangular fuzzy set with given peak and width parameters.
		\param peakPoint
			The top of the triangle.
		\param leftOffset
			Distance from peak to the left base.
		\param rightOffset
			Distance from peak to the right base.
		*******************************************************************************/
		FuzzySet_Triangle(float peakPoint, float leftOffset, float rightOffset)
			: FuzzySet(peakPoint, leftOffset, rightOffset, peakPoint)
		{
		}

		/*!*****************************************************************************
		\brief
			Calculates the DOM for a triangular fuzzy set.
		\param val
			Value to evaluate.
		\return
			Degree of membership for the value.
		*******************************************************************************/
		float calculateDOM(float val) const
		{
			// Prevent divide by zero
			if (isEqual(peakPoint, val) &&
				(isEqual(leftOffset, 0.0f) || isEqual(rightOffset, 0.0f)))
				return 1.0f;

			// Left of peak: linear increase
			if (val <= peakPoint && val >= peakPoint - leftOffset)
				return (1.0f / leftOffset) * (val - (peakPoint - leftOffset));

			// Right of peak: linear decrease
			if (val > peakPoint && val < peakPoint + rightOffset)
				return (1.0f / -rightOffset) * (val - peakPoint) + 1.0f;

			// Out of range
			return 0.0f;
		}
	};


	// Fuzzy logic works with membership values in a way that mimics Boolean logic. 
	// Replacements for basic logic operators AND and OR are defined here.

	// Definition of the base operator class
	class FuzzyOperator
	{
	protected:
		std::list<std::shared_ptr<FuzzySet>> sets;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a FuzzyOperator (base class) from a list of FuzzySet pointers.
		\param sets
			Initializer list of shared pointers to FuzzySets (default is empty).
		*******************************************************************************/
		FuzzyOperator(std::initializer_list<std::shared_ptr<FuzzySet>> sets = {})
			: sets{ sets }
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual destructor for FuzzyOperator.
		*******************************************************************************/
		virtual ~FuzzyOperator()
		{
		}

		/*!*****************************************************************************
		\brief
			Virtual method to get the result DOM from the operator (AND/OR).
		\return
			The combined DOM value.
		*******************************************************************************/
		virtual float getDOM()
		{
			return 0;
		}

		/*!*****************************************************************************
		\brief
			Clears the DOM values of all fuzzy sets in the operator.
		*******************************************************************************/
		void clearDOM()
		{
			for (std::shared_ptr<FuzzySet> set : sets)
				set->clearDOM();
		}

		/*!*****************************************************************************
		\brief
			Updates the DOM of all fuzzy sets to the maximum of val and their current DOM.
		\param val
			Value to OR with each set's DOM.
		*******************************************************************************/
		void ORwithDOM(float val)
		{
			for (std::shared_ptr<FuzzySet> set : sets)
				set->ORwithDOM(val);
		}
	};

	//  Definition of the AND operator class
	class FuzzyAND : public FuzzyOperator
	{
	public:
		/*!*****************************************************************************
		\brief
			Constructs a FuzzyAND operator from a list of FuzzySet pointers.
		\param sets
			Initializer list of shared pointers to FuzzySets (default is empty).
		*******************************************************************************/
		FuzzyAND(std::initializer_list<std::shared_ptr<FuzzySet>> sets = {})
			: FuzzyOperator{ sets }
		{
		}

		/*!*****************************************************************************
		\brief
			Returns the minimum DOM among all sets for AND logic.
		\return
			The minimum DOM value, or 0 if there are no sets.
		*******************************************************************************/
		float getDOM()
		{
			float smallest = std::numeric_limits<float>::max();
			for (auto& set : sets)
			{
				float cur = set->getDOM();
				if (cur < smallest)
					smallest = cur;
			}
			return (smallest == std::numeric_limits<float>::max()) ? 0.0f : smallest;
		}
	};

	// Definition of the OR operator class
	class FuzzyOR : public FuzzyOperator
	{
	public:
		/*!*****************************************************************************
		\brief
			Constructs a FuzzyOR operator from a list of FuzzySet pointers.
		\param sets
			Initializer list of shared pointers to FuzzySets (default is empty).
		*******************************************************************************/
		FuzzyOR(std::initializer_list<std::shared_ptr<FuzzySet>> sets = {})
			: FuzzyOperator{ sets }
		{
		}

		/*!*****************************************************************************
		\brief
			Returns the maximum DOM among all sets for OR logic.
		\return
			The maximum DOM value, or 0 if there are no sets.
		*******************************************************************************/
		float getDOM()
		{
			float largest = std::numeric_limits<float>::lowest();
			for (auto& set : sets)
			{
				float cur = set->getDOM();
				if (cur > largest)
					largest = cur;
			}
			return (largest == std::numeric_limits<float>::lowest()) ? 0.0f : largest;
		}
	};

	// Definition of the fuzzy variable class
	class FuzzyVariable
	{
	protected:
		// A map of the fuzzy sets that comprise this variable
		std::map<std::string, std::shared_ptr<FuzzySet>> sets;

		// The minimum and maximum value of the range of this variable
		float minRange;
		float maxRange;

	public:
		/*!*****************************************************************************
		\brief
			Constructs an empty FuzzyVariable with default range.
		*******************************************************************************/
		FuzzyVariable()
			: sets{ }, minRange{ 0.0f }, maxRange{ 0.0f }
		{
		}

		/*!*****************************************************************************
		\brief
			Destructs a FuzzyVariable.
		*******************************************************************************/
		virtual ~FuzzyVariable()
		{
		}

		/*!*****************************************************************************
		\brief
			Gets a pointer to the named fuzzy set within this variable.
		\param name
			Name of the set.
		\return
			Shared pointer to the fuzzy set.
		*******************************************************************************/
		std::shared_ptr<FuzzySet> getSet(const std::string& name)
		{
			return sets[name];
		}

		/*!*****************************************************************************
		\brief
			Adjusts the variable's min and max range to fit new fuzzy set bounds.
		\param minBound
			Lower bound of the set.
		\param maxBound
			Upper bound of the set.
		*******************************************************************************/
		void adjustRangeToFit(float minBound, float maxBound)
		{
			if (minBound < minRange)
				minRange = minBound;
			if (maxBound > maxRange)
				maxRange = maxBound;
		}

		// The following methods create instances of the sets named in the method
		// name and add them to the member set map. Each time a set of any type is
		// added the minRange and maxRange are adjusted accordingly.

		/*!*****************************************************************************
		\brief
			Adds a left shoulder set with specified bounds and peak.
		\param name
			Name for the new set.
		\param minBound
			Minimum bound of the set.
		\param peak
			Peak value.
		\param maxBound
			Maximum bound of the set.
		\return
			Reference to this FuzzyVariable for chaining.
		*******************************************************************************/
		FuzzyVariable& addLeftShoulderSet(const std::string& name, float minBound, float peak, float maxBound)
		{
			sets.insert(std::pair<std::string, std::shared_ptr<FuzzySet>>(name, 
				std::shared_ptr<FuzzySet>(new FuzzySet_LeftShoulder(peak, peak - minBound, maxBound - peak))));
			adjustRangeToFit(minBound, maxBound);
			return *this;
		}

		/*!*****************************************************************************
		\brief
			Adds a right shoulder set with specified bounds and peak.
		\param name
			Name for the new set.
		\param minBound
			Minimum bound of the set.
		\param peak
			Peak value.
		\param maxBound
			Maximum bound of the set.
		\return
			Reference to this FuzzyVariable for chaining.
		*******************************************************************************/
		FuzzyVariable& addRightShoulderSet(const std::string& name, float minBound, float peak, float maxBound)
		{
			sets.insert(std::pair<std::string, std::shared_ptr<FuzzySet>>(name, 
				std::shared_ptr<FuzzySet>(new FuzzySet_RightShoulder(peak, peak - minBound, maxBound - peak))));
			adjustRangeToFit(minBound, maxBound);
			return *this;
		}

		/*!*****************************************************************************
		\brief
			Adds a triangular set with specified bounds and peak.
		\param name
			Name for the new set.
		\param minBound
			Minimum bound of the set.
		\param peak
			Peak value.
		\param maxBound
			Maximum bound of the set.
		\return
			Reference to this FuzzyVariable for chaining.
		*******************************************************************************/
		FuzzyVariable& addTriangularSet(const std::string& name, float minBound, float peak, float maxBound)
		{
			sets.insert(std::pair<std::string, std::shared_ptr<FuzzySet>>(name, 
				std::shared_ptr<FuzzySet>(new FuzzySet_Triangle(peak, peak - minBound, maxBound - peak))));
			adjustRangeToFit(minBound, maxBound);
			return *this;
		}

		/*!*****************************************************************************
		\brief
			Adds a singleton set with specified bounds and peak.
		\param name
			Name for the new set.
		\param minBound
			Minimum bound of the set.
		\param peak
			Peak value.
		\param maxBound
			Maximum bound of the set.
		\return
			Reference to this FuzzyVariable for chaining.
		*******************************************************************************/
		FuzzyVariable& addSingletonSet(const std::string& name, float minBound, float peak, float maxBound)
		{
			sets.insert(std::pair<std::string, std::shared_ptr<FuzzySet>>(name, 
				std::shared_ptr<FuzzySet>(new FuzzySet_Singleton(peak, peak - minBound, maxBound - peak))));
			adjustRangeToFit(minBound, maxBound);
			return *this;
		}

		/*!*****************************************************************************
		\brief
			Calculates DOMs for all sets in this variable for the given crisp value.
		\param val
			Value to fuzzify.
		\return
			Pointer to this variable for chaining.
		*******************************************************************************/
		FuzzyVariable* fuzzify(float val)
		{
			//for each set in the flv calculate the degree of membership for the given value
			for (std::pair<std::string, std::shared_ptr<FuzzySet>> set : sets)
				set.second->setDOM(set.second->calculateDOM(val));
			return this;
		}

		/*!*****************************************************************************
		\brief
			Updates DOMs of all sets using the maximum of the argument and their current value.
		\param val
			Value to OR with each set's DOM.
		*******************************************************************************/
		void ORwithDOM(float val)
		{
			for (std::pair<std::string, std::shared_ptr<FuzzySet>> set : sets)
				set.second->ORwithDOM(val);
		}

		/*!*****************************************************************************
		\brief
			Defuzzifies using maximum average method: sum(maxima*DOM) / sum(DOM).
		\return
			Defuzzified crisp value.
		*******************************************************************************/
		float deFuzzifyMaxAv()
		{
			float numerator = 0.0f, denominator = 0.0f;
			for (auto it = sets.begin(); it != sets.end(); ++it)
			{
				auto set = it->second;
				numerator += set->getRepresentativeValue() * set->getDOM();
				denominator += set->getDOM();
			}
			if (isEqual(0.0f, denominator))
				return 0.0f;
			return numerator / denominator;
		}

		/*!*****************************************************************************
		\brief
			Defuzzifies using the centroid method with specified sample count.
		\param numSamples
			Number of samples for centroid calculation.
		\return
			Defuzzified crisp value.
		*******************************************************************************/
		float deFuzzifyCentroid(int numSamples)
		{
			float stepSize = (maxRange - minRange) / numSamples;
			float totalArea = 0.0f;
			float sumOfMoments = 0.0f;

			for (int samp = 1; samp <= numSamples; ++samp)
			{
				float x = minRange + samp * stepSize;
				for (auto it = sets.begin(); it != sets.end(); ++it)
				{
					auto set = it->second;
					float contribution = std::min(
						set->calculateDOM(x),
						set->getDOM()
					);
					totalArea += contribution;
					sumOfMoments += x * contribution;
				}
			}
			if (isEqual(0.0f, totalArea))
				return 0.0f;
			return sumOfMoments / totalArea;
		}
	};


	// Definition of the fuzzy rule class of form
	// IF antecedent THEN consequence
	class FuzzyRule
	{
	protected:
		// Antecedent (usually a composite of several fuzzy sets and operators)
		std::shared_ptr<FuzzyOperator> antecedent;

		// Consequence (usually a single fuzzy set, but can be several ANDed together)
		std::shared_ptr<FuzzySet> consequence;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a fuzzy rule of the form IF antecedent THEN consequence.
		\param antecedent
			Shared pointer to the antecedent fuzzy operator.
		\param consequence
			Shared pointer to the consequence fuzzy set.
		*******************************************************************************/
		FuzzyRule(std::shared_ptr<FuzzyOperator> antecedent, std::shared_ptr<FuzzySet> consequence)
			: antecedent{ antecedent }, consequence{ consequence }
		{
		}

		// Updates the DOM (the confidence) of the consequent set with
		// the DOM of the antecedent set. 

		/*!*****************************************************************************
		\brief
			Calculates the consequence set's DOM by firing the rule.
		\return
			Shared pointer to the consequence fuzzy set.
		*******************************************************************************/
		std::shared_ptr<FuzzySet> calculate()
		{
			consequence->ORwithDOM(antecedent->getDOM());
			return consequence;
		}

		/*!*****************************************************************************
		\brief
			Sets the consequence's DOM to zero (clears confidence).
		*******************************************************************************/
		void setConfidenceOfConsequentToZero()
		{
			consequence->clearDOM();
		}
	};

	// Definition of the fuzzy module class
	class FuzzyModule
	{
	public:
		// Defuzzify methods supported by this module.
		enum DefuzzifyMethod { max_av = 0, centroid = 1 };

	protected:

		// When calculating the centroid of the fuzzy manifold this value is used
		// to determine how many cross-sections should be sampled
		int numSamples;

		// A map of all the fuzzy variables this module uses
		std::map<std::string, FuzzyVariable> variables;

		// An array containing all fuzzy rules
		std::list<FuzzyRule> rules;

	public:
		/*!*****************************************************************************
		\brief
			Constructs a FuzzyModule, initializing variables, rules, and number of centroid samples.
		*******************************************************************************/
		FuzzyModule()
			: numSamples{ 15 }, variables{ }, rules{ }
		{
		}

		/*!*****************************************************************************
		\brief
			Gets a reference to a named FuzzyVariable in the module.
		\param name
			Name of the fuzzy variable.
		\return
			Reference to the FuzzyVariable.
		*******************************************************************************/
		FuzzyVariable& getVariable(const std::string& name)
		{
			return variables[name];
		}

		/*!*****************************************************************************
		\brief
			Clears the DOMs of all rule consequents in the module.
		*******************************************************************************/
		void setConfidencesOfConsequentsToZero()
		{
			for (FuzzyRule rule : rules)
				rule.setConfidenceOfConsequentToZero();
		}

		/*!*****************************************************************************
		\brief
			Creates and returns a new FuzzyVariable with the given name.
		\param varName
			Name for the new variable.
		\return
			Reference to the new FuzzyVariable.
		*******************************************************************************/
		FuzzyVariable& createVariable(std::string varName)
		{
			variables.insert(std::pair<std::string, FuzzyVariable>(varName, FuzzyVariable()));
			return variables[varName];
		}

		/*!*****************************************************************************
		\brief
			Adds a fuzzy rule (antecedent consequence) to the module.
		\param antecedent
			Shared pointer to the antecedent operator.
		\param consequence
			Shared pointer to the consequence fuzzy set.
		*******************************************************************************/
		void addRule(std::shared_ptr<FuzzyOperator> antecedent, std::shared_ptr<FuzzySet> consequence)
		{
			rules.push_back(FuzzyRule(antecedent, consequence));
		}

		/*!*****************************************************************************
		\brief
			Fuzzifies the specified variable using the given crisp value.
		\param varName
			Name of the fuzzy variable.
		\param val
			Value to fuzzify.
		*******************************************************************************/
		void fuzzify(const std::string& varName, float val)
		{
			variables[varName].fuzzify(val);
		}

		/*!*****************************************************************************
		\brief
			Defuzzifies the named variable using the specified method (max_av or centroid).
		\param varName
			Name of the variable to defuzzify.
		\param method
			Defuzzification method to use.
		\return
			Defuzzified crisp value.
		*******************************************************************************/
		float deFuzzify(const std::string& varName, DefuzzifyMethod method)
		{
			setConfidencesOfConsequentsToZero();
			for (auto& rule : rules)
				rule.calculate();
			switch (method)
			{
			case centroid:
				return variables[varName].deFuzzifyCentroid(numSamples);
			case max_av:
				return variables[varName].deFuzzifyMaxAv();
			default:
				return 0.0f;
			}
		}
	};


} // end namespace

#endif