/*!*****************************************************************************
\file      functions.h
\author    Vadim Surov, Jie Le Jet Ang
\par       DP email: jielejet.ang@digipen.edu.sg
\par       Course: CS3183
\par       Section: A
\par       Programming Assignment 12
\date      24-07-2025

\brief
    This file declares template classes and utility structures for implementing 
    a genetic algorithm framework. Includes gene and chromosome classes, population 
    and individual management, fitness evaluations, and the core genetic
    algorithm class with selection, crossover, and mutation logic for evolving
    populations to solve optimization problems such as bit-counting and the
    8-Queens problem.
*******************************************************************************/
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>

#include "data.h"

// Returns result of addition of all genes in a chromosome
template<typename Gene>
struct Fitness_Accumulate
{
    /*!*****************************************************************************
    \brief
        Computes the sum of all gene values in the given gene vector.

    \param genes
        The vector of gene objects to be summed.

    \return
        The sum of all gene values.
    *******************************************************************************/
    int operator()(const std::vector<Gene>& genes) const
    {
        int sum = 0;
        for (auto& g : genes)
            sum += g.getValue();
        return sum;
    }
};

// Returns a measure in percentages that indicates the fitness of  
// a particular chromosome into a particular solution.
// Fittest chromosome has all genes equal 1.
template<typename Gene>
struct Fitness_Nbits
{
    /*!*****************************************************************************
    \brief
        Calculates the percentage of genes with value 1 in the given gene vector.

    \param genes
        The vector of gene objects.

    \return
        The fitness score as a percentage of genes equal to 1.
    *******************************************************************************/
    int operator()(const std::vector<Gene>& genes) const
    {
        int ones = 0;
        for (auto& g : genes)
            ones += (g.getValue() == 1 ? 1 : 0);
        return (ones * 100) / static_cast<int>(genes.size());
    }
};

// Returns a measure in percentages that indicates the fitness of  
// a particular chromosome into a particular solution of 8 queens 
// problem
template<typename Gene>
struct Fitness_8queens
{
    /*!*****************************************************************************
    \brief
        Evaluates if the given gene vector represents a valid 8-queens solution.

    \param genes
        The vector of gene objects, each gene value is a queen position.

    \return
        100 if valid solution, 0 otherwise.
    *******************************************************************************/
    int operator()(const std::vector<Gene>& genes) const
    {
        if (genes.empty()) 
            return 0;


        size_t N = genes.size();
        std::vector<int> pos;
        for (auto& g : genes) pos.push_back(g.getValue());
        // check unique
        std::vector<bool> used(N, false);
        for (int x : pos) {
            if (x < 0 || x >= (int)N || used[x]) return 0;
            used[x] = true;
        }
        // check diagonal conflict
        for (size_t i = 0; i < N; ++i)
            for (size_t j = i + 1; j < N; ++j)
                if (std::abs(pos[i] - pos[j]) == std::abs((int)i - (int)j)) return 0;
        return 100;
    }
};


namespace AI
{
    // Crossover methods for the genetic algorithm
    enum CrossoverMethod { Middle, Random  };
    
    // Simplest gene seeding class/function
    struct Seed
    {
        /*!*****************************************************************************
        \brief
            Returns the provided value for gene initialization (seeding).

        \param p
            Value to return (default 0).

        \return
            The same value as input.
        *******************************************************************************/
        int operator()(int p = 0) const
        {
            return p;
        }
    };

    // Gene seeding class/function with a fixed value
    template<int Val = 0>
    struct Seed_Value
    {
        /*!*****************************************************************************
        \brief
            Returns a fixed compile-time value for gene initialization.

        \return
            The template value Val.
        *******************************************************************************/
        int operator()(int /* p */ = 0) const
        {
            return Val;
        }
    };

    // Gene random seeding class/function
    template<int Max>
    struct Seed_Random
    {
        /*!*****************************************************************************
        \brief
            Returns a random integer in the range [0, Max-1] for gene initialization.

        \param p
            Ignored parameter.

        \return
            A random integer.
        *******************************************************************************/
        int operator()(int /* p */ = 0) const
        {
            // Your code ...
            return std::rand() % Max; // returns value in [0, Max-1]
        }
    };

    // Gene class
    template<typename T = int, typename S = Seed>
    class Gene
    {
        T value;

    public:
        /*!*****************************************************************************
        \brief
            Constructs a Gene object using an optional parameter value and seed strategy.

        \param p
            The initial value for the gene (default 0).
        *******************************************************************************/
        Gene(int p = 0)
            : value{ S()(p) }
        {
        }

        /*!*****************************************************************************
        \brief
            Retrieves the value stored in the gene.

        \return
            The value of the gene.
        *******************************************************************************/
        T getValue() const
        {
            return value;
        }

        /*!*****************************************************************************
        \brief
            Sets the value of the gene.

        \param v
            The value to assign to the gene.
        *******************************************************************************/
        void setValue(T v)
        {
            value = v;
        }

        /*!*****************************************************************************
        \brief
            Stream insertion operator for outputting the value of a Gene.

        \param os
            The output stream to write to.

        \param rhs
            The Gene instance to output.

        \return
            Reference to the output stream.
        *******************************************************************************/
        friend std::ostream& operator<<(std::ostream& os, const Gene& rhs)
        {
            os << rhs.value;
            return os;
        }
    };

    // Chromosome class
    template<typename Gene, typename Fitness, size_t Size>
    class Chromosome
    {
        std::vector<Gene> genes;
        int fitness;

    public:

        using gene_type = Gene;

        static const size_t size = Size;

        /*!*****************************************************************************
        \brief
            Constructs a Chromosome object with all genes initialized and computes initial fitness.
        *******************************************************************************/
        Chromosome()
            : genes(Size), fitness{ Fitness()(genes) }
        {
        }

        /*!*****************************************************************************
        \brief
            Retrieves a reference to the chromosome gene vector.

        \return
            Reference to the vector of gene objects.
        *******************************************************************************/
        std::vector<Gene>& getGenes()
        {
            return genes;
        }

        /*!*****************************************************************************
        \brief
            Sets the chromosome gene vector and updates fitness.

        \param v
            Vector of genes to assign to this chromosome.
        *******************************************************************************/
        void setGenes(const std::vector<Gene>& v)
        {
            genes = v;
            fitness = Fitness()(genes);
        }

        /*!*****************************************************************************
        \brief
            Retrieves the gene at the specified index.

        \param i
            The index of the gene to retrieve.

        \return
            The gene object at the specified index.
        *******************************************************************************/
        Gene getGene(size_t i) const
        {
            return genes[i];
        }

        /*!*****************************************************************************
        \brief
            Sets the gene at the specified index and updates the chromosome fitness.

        \param i
            The index of the gene to set.
        \param v
            The new gene value.
        *******************************************************************************/
        void setGene(size_t i, const Gene& v)
        {
            genes[i] = v;
            fitness = Fitness()(genes);
        }

        /*!*****************************************************************************
        \brief
            Gets the fitness value of the chromosome.

        \return
            The current fitness score.
        *******************************************************************************/
        int getFitness() const
        {
            return fitness;
        }

        // Select a random mutation point and change 
        // gene at the mutation point
        /*!*****************************************************************************
        \brief
            Performs a random mutation by changing a randomly selected gene in the chromosome.
        *******************************************************************************/
        void randomMutation()
        {
            setGene(std::rand() % Chromosome::size, Gene());
        }

        // Copy genes from a source
        /*!*****************************************************************************
        \brief
            Copies gene values from another chromosome and updates fitness.

        \param src
            The source chromosome from which to copy genes.
        *******************************************************************************/
        void copyGenesFrom(Chromosome& src)
        {
            std::copy(src.genes.begin(), src.genes.end(), genes.begin());
            fitness = Fitness()(genes);
            //genes = src.genes;
            //fitness = Fitness()(genes);
        }

        /*!*****************************************************************************
        \brief
            Stream insertion operator for outputting chromosome gene values and fitness.

        \param os
            Output stream.

        \param rhs
            Chromosome to display.

        \return
            Reference to the output stream.
        *******************************************************************************/
        friend std::ostream& operator<<(std::ostream& os, 
            const Chromosome& rhs)
        {
            os << '[';
            for (auto it = rhs.genes.begin(); it != rhs.genes.end(); ++it)
                os << *it << (it + 1 != rhs.genes.end() ? "," : "");
            os << "]=" << rhs.fitness;
            return os;
        }
    };

    // Individual class
    template<typename Chromosome>
    class Individual
    {
        Chromosome chromosome;
 
    public:

        using chromosome_type = Chromosome;
        using gene_type = typename Chromosome::gene_type;

        /*!*****************************************************************************
        \brief
            Constructs an Individual object, initializing the contained chromosome.
        *******************************************************************************/
        Individual()
            : chromosome{ }
        {
        }

        /*!*****************************************************************************
        \brief
            Gets a reference to the individual's chromosome.

        \return
            Reference to the chromosome object.
        *******************************************************************************/
        Chromosome& getChromosome()
        {
            return chromosome;
        }

        /*!*****************************************************************************
        \brief
            Gets a reference to the chromosome's gene vector.

        \return
            Reference to the vector of gene objects.
        *******************************************************************************/
        std::vector<gene_type>& getGenes()
        {
            return chromosome.getGenes();
        }

        /*!*****************************************************************************
        \brief
            Sets the individual's chromosome's gene vector.

        \param v
            Vector of gene objects.
        *******************************************************************************/
        void setGenes(const std::vector<gene_type>& v)
        {
            chromosome.setGenes(v);
        }

        /*!*****************************************************************************
        \brief
            Retrieves the gene at a specific index in the individual's chromosome.

        \param i
            Index of the gene to retrieve.

        \return
            The gene object at the specified index.
        *******************************************************************************/
        gene_type getGene(size_t i) const
        {
            return chromosome.getGene(i);
        }

        /*!*****************************************************************************
        \brief
            Copies all genes from another individual.

        \param individual
            The individual from which to copy genes.
        *******************************************************************************/
        void copyGenesFrom(Individual& individual)
        {
            chromosome.copyGenesFrom(individual.chromosome);
        }

        /*!*****************************************************************************
        \brief
            Sets the gene at a specific index in the individual's chromosome.

        \param i

            The index at which to set the gene.

        \param gene
            The gene value to assign.
        *******************************************************************************/
        void setGene(size_t i, gene_type gene)
        {
            chromosome.setGene(i, gene);
        }

        /*!*****************************************************************************
        \brief
            Gets the individual's fitness score (chromosome's fitness).

        \return
            The individual's fitness score.
        *******************************************************************************/
        int getFitness() const
        {
            return chromosome.getFitness();
        }

        /*!*****************************************************************************
        \brief
            Stream insertion operator for outputting individual chromosome.

        \param os
            Output stream.

        \param rhs
            Individual to display.

        \return
            Reference to the output stream.
        *******************************************************************************/
        friend std::ostream& operator<<(std::ostream& os, Individual& rhs)
        {
            os << rhs.chromosome;
            return os;
        }
    };

    // Population class
    template<typename Individual>
    class Population
    {
        std::vector<Individual> individuals;
        Individual* fittest;

    public:
        /*!*****************************************************************************
        \brief
            Constructs a Population with the specified number of individuals and determines the fittest.

        \param size
            Number of individuals in the population.
        *******************************************************************************/
        Population(size_t size = 0)
            : individuals{ }, fittest{ nullptr }
        {
            if (size)
            {
                individuals.resize(size);
                updateFittest();
            }
        }

        /*!*****************************************************************************
        \brief
            Returns the number of individuals in the population.

        \return
            Number of individuals.
        *******************************************************************************/
        size_t getSize() const
        {
            return individuals.size();
        }

        /*!*****************************************************************************
        \brief
            Gets a reference to the individual at the specified index.

        \param i
            Index of the individual.

        \return
            Reference to the individual.
        *******************************************************************************/
        Individual& getIndividual(size_t i)
        {
            return individuals[i];
        }

        /*!*****************************************************************************
        \brief
            Gets a pointer to the fittest individual in the population.

        \return
            Pointer to the fittest individual.
        *******************************************************************************/
        Individual* getFittest() const
        {
            return fittest;
        }

        /*!*****************************************************************************
        \brief
            Updates the pointer to the fittest individual based on current fitness values.
        *******************************************************************************/
        void updateFittest()
        {
            // Your code ...
            if (individuals.empty()) {
                fittest = nullptr;
                return;
            }
            fittest = &individuals[0];
            for (size_t i = 1; i < individuals.size(); ++i) {
                if (individuals[i].getFitness() > fittest->getFitness())
                    fittest = &individuals[i];
            }
        }

        /*!*****************************************************************************
        \brief
            Stream insertion operator for outputting the population fitness and individuals.

        \param os
            Output stream.

        \param rhs
            Population to display.

        \return
            Reference to the output stream.
        *******************************************************************************/
        friend std::ostream& operator<<(std::ostream& os, Population& rhs)
        {
            os << " = " << rhs.getFittest()->getFitness() << std::endl;
            for (size_t i = 0; i < rhs.getSize(); ++i)
                os << "  " << i << ':' << rhs.getIndividual(i) << std::endl;
            return os;
        }
    };
 
    // Genetic Algorithm class
    template<typename Individual>
    class GeneticAlgorithm
    {
        Population<Individual>* population;
        int generation;
 
    public:
        /*!*****************************************************************************
        \brief
            Constructs a GeneticAlgorithm object, initializing population and generation count.
        *******************************************************************************/
        GeneticAlgorithm()
            : population{ nullptr }, generation{ 0 }
        {
        }

        /*!*****************************************************************************
        \brief
            Destructor. Cleans up dynamically allocated population.
        *******************************************************************************/
        ~GeneticAlgorithm()
        {
            // Your code ...
            delete population;
        }

        /*!*****************************************************************************
        \brief
            Retrieves a pointer to the fittest individual in the current population.

        \return
            Pointer to the fittest individual or nullptr if not available.
        *******************************************************************************/
        Individual* getFittest() const
        {

            // Your code ...
            return population ? population->getFittest() : nullptr;
        }

        // Implementation of the Roulette Wheel Selection. The probability of an 
        // individual to be selected is directly proportional to its fitness.
        /*!*****************************************************************************
        \brief
            Performs roulette wheel selection to generate a new population.

        \param sizeOfPopulation
            Number of individuals in the new population.

        \return
            Pointer to a newly created Population object.
        *******************************************************************************/
        Population<Individual>* selection(size_t sizeOfPopulation)
        {
            if (!population)
            {
                population = new Population<Individual>(sizeOfPopulation);
                population->updateFittest();
            }

            // Compute total fitness
            int total_fitness = 0;
            for (size_t i = 0; i < population->getSize(); ++i)
                total_fitness += population->getIndividual(i).getFitness();

            auto* newGeneration = new Population<Individual>(sizeOfPopulation);

            for (size_t i = 0; i < sizeOfPopulation; ++i)
            {
                size_t selected = 0;
                if (total_fitness > 0)
                {
                    int rnd = std::rand() % total_fitness;
                    int accum = 0;
                    for (size_t j = 0; j < population->getSize(); ++j)
                    {
                        accum += population->getIndividual(j).getFitness();
                        if (accum > rnd)
                        {
                            selected = j;
                            break;
                        }
                    }
                }
                else
                {
                    selected = std::rand() % population->getSize();
                }
                newGeneration->getIndividual(i).copyGenesFrom(population->getIndividual(selected));
            }
            newGeneration->updateFittest();
            return newGeneration;
        }

        // Crossover parents genes
        /*!*****************************************************************************
        \brief
            Performs crossover on the population to create new gene combinations.

        \param newGeneration
            Pointer to the new generation's population.

        \param crossoverMethod
            Method to use for crossover (e.g., Middle or Random).
        *******************************************************************************/
        void crossover(Population<Individual>* newGeneration, 
            CrossoverMethod crossoverMethod)
        {
            // Your code ...
            if (!newGeneration)
                return;

            size_t sizeOfChromosome = newGeneration->getIndividual(0).getGenes().size();
            size_t sizeOfPopulation = newGeneration->getSize();

            size_t crossOverPoint = 0;
            if (crossoverMethod == CrossoverMethod::Middle)
                crossOverPoint = sizeOfChromosome / 2;
            else if (crossoverMethod == CrossoverMethod::Random)
                crossOverPoint = std::rand() % sizeOfChromosome;

            // Go in pairs
            for (size_t j = 0; j + 1 < sizeOfPopulation; j += 2)
            {
                // Copy genes to temp for swapping
                std::vector<typename Individual::gene_type> genesA = newGeneration->getIndividual(j).getGenes();
                std::vector<typename Individual::gene_type> genesB = newGeneration->getIndividual(j + 1).getGenes();
                // Swap up to crossOverPoint
                for (size_t i = 0; i < crossOverPoint; ++i)
                    std::swap(genesA[i], genesB[i]);
                // Assign back
                newGeneration->getIndividual(j).setGenes(genesA);
                newGeneration->getIndividual(j + 1).setGenes(genesB);
            }
            newGeneration->updateFittest();
        }

        // Do mutation of genes under a random probability
        /*!*****************************************************************************
        \brief
            Performs random mutation on the genes of the population.

        \param newGeneration
            Pointer to the population to mutate.

        \param mutationProbability
            Probability of mutating each individual (0-100).
        *******************************************************************************/
        void mutation(Population<Individual>* newGeneration, 
            int mutationProbability)
        {
            // Your code ...
            if (!newGeneration)
                return;

            size_t sizeOfPopulation = newGeneration->getSize();

            for (size_t j = 0; j < sizeOfPopulation; ++j)
            {
                if ((std::rand() % 100) < mutationProbability)
                {
                    newGeneration->getIndividual(j).getChromosome().randomMutation();
                }
            }
            newGeneration->updateFittest();
        }

        // Replace existing population if any with a new generation
        /*!*****************************************************************************
        \brief
            Replaces the current population with a new generation.

        \param newGeneration
            Pointer to the new population.
        *******************************************************************************/
        void setPopulation(Population<Individual>* newGeneration)
        {
            // Your code ...
            if (population != newGeneration)
            {
                delete population;
                population = newGeneration;
            }
        }

        // Start the search
        /*!*****************************************************************************
        \brief
            Runs the genetic algorithm, evolving the population until an optimal solution is found or a generation limit is reached.

        \param sizeOfPopulation
            Number of individuals in the population.

        \param mutationProbability
            Probability (0-100) for mutation to occur.

        \param crossoverMethod
            Crossover strategy to use.

        \param os
            Optional pointer to an output string stream for logging.
        *******************************************************************************/
        void run(size_t sizeOfPopulation = 100, int mutationProbability = 70, 
                    CrossoverMethod crossoverMethod = CrossoverMethod::Middle, 
                    std::ostringstream* os = nullptr)
        {
            if (population) {
                delete population;
                population = nullptr;
            }
            population = new Population<Individual>(sizeOfPopulation);
            generation = 0;

            // Loop until the solution is found
            while (next(mutationProbability, crossoverMethod, os)) {}
        }

        // Continue the search
        /*!*****************************************************************************
        \brief
            Advances the genetic algorithm by one generation. Performs selection, 
            crossover, mutation, and updates the population.

        \param mutationProbability
            Probability (0-100) for mutation.

        \param crossoverMethod
            Crossover strategy to use.

        \param os
            Optional pointer to an output string stream for logging.

        \return
            True if another generation should be processed, false if finished.
        *******************************************************************************/
        bool next(int mutationProbability, CrossoverMethod crossoverMethod, 
                    std::ostringstream* os)
        {
            if (!population)
                return false;

            if (os)
                *os << "\nGeneration: " << generation << *population << "\n";

            // Stop the search when either max fitness of solution or max limit for generations achieved
            if (population->getFittest() && population->getFittest()->getFitness() == 100)
                return false;
            if (generation > 10000)
                return false;

            size_t sizeOfPopulation = population->getSize();

            Population<Individual>* newGeneration = selection(sizeOfPopulation);

            crossover(newGeneration, crossoverMethod);
            mutation(newGeneration, mutationProbability);

            setPopulation(newGeneration);

            generation++;

            return true;
        }
    };

} // end namespace

#endif