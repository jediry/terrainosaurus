#include "SimilarityGA.hpp"
#include "terrain-operations.hpp"
using namespace terrainosaurus;

#include <inca/math/generator/RandomUniform>
using namespace inca::math;

#define MAX_WEIGHT 10.0f


// Operators and helper functions used by the similarity GA
namespace terrainosaurus {
    // Function returning a uniformly distributed, random integer
    int randomInt(int min, int max) {
        return RandomUniform<int>()(min, max);
    }

    // Function returning a uniformly distributed, random scalar
    scalar_t randomScalar(scalar_t min, scalar_t max) {
        return RandomUniform<scalar_t>()(min, max);
    }

    // Initialization operator assigning default variances and weights
    class DefaultInitializationOperator
            : public SimilarityGA::InitializationOperator {
    public:
        void operator()(Chromosome & c) {
            for (int i = 0; i < c.size(); ++i) {
                c[i].weight    = 1.0f;
                c[i].variance  = 1.0f;
                c[i].magnitude = 1;
            }
        }
    };

    // Initialization operator assigning random variances and weights
    class RandomInitializationOperator
            : public SimilarityGA::InitializationOperator {
    public:
        void operator()(Chromosome & c) {
            for (int i = 0; i < c.size(); ++i) {
                c[i].weight    = randomScalar(0.0f, 10.0f);
                c[i].variance  = randomScalar(0.1f, 10.0f);
                c[i].magnitude = randomInt(-10, 10);
            }
        }
    };


    // Crossover operator swapping parts on either side of a split point
    class StandardCrossoverOperator
            : public SimilarityGA::CrossoverOperator {
    public:
        void operator()(Chromosome & c1, Chromosome & c2) {
            int split = randomInt(0, c1.size() - 1);
            for (int i = split; i < c1.size(); ++i)
                swap(c1[i], c2[i]);
        }
    };

    // Crossover operator exchanging only the weights (not variances)
    class WeightCrossoverOperator
            : public SimilarityGA::CrossoverOperator {
    public:
        void operator()(Chromosome & c1, Chromosome & c2) {
            for (int i = 0; i < c1.size(); ++i) {
                scalar_t w = c1[i].weight;
                c1[i].weight = c2[i].weight;
                c2[i].weight = w;
            }
        }
    };


    // Mutation operator randomly decreasing or increasing the weight of a gene
    class WeightMutationOperator
            : public SimilarityGA::MutationOperator {
    public:
        void operator()(Gene & g) {
            g.weight += randomScalar(-0.5, 0.5);
            if (g.weight < 0)               g.weight = 0;
            else if (g.weight > MAX_WEIGHT) g.weight = MAX_WEIGHT;
        }
    };

    // Mutation operator randomly decreasing or increasing the variance
    // of a gene
    class VarianceMutationOperator
            : public SimilarityGA::MutationOperator {
    public:
        void operator()(Gene & g) {
            g.variance += randomScalar(-0.5f, 0.5f);
            if (g.variance < 0) {
                g.variance  = 10;
                g.magnitude -= 1;
            } else if (g.variance > 10) {
                g.variance /= 10;
                g.magnitude += 1;
            }
        }
    };

    // Mutation operator randomly decreasing or increasing the variance
    // magnitude of a gene
    class VarianceMagnitudeMutationOperator
            : public SimilarityGA::MutationOperator {
    public:
        void operator()(Gene & g) {
            g.magnitude += randomInt(-1, 1);
        }
    };


    // Fitness operator measuring the average fitness of the entire library,
    // using the current coefficients
    class AggregateLibraryFitnessOperator
            : public SimilarityGA::FitnessOperator {
    public:
        AggregateLibraryFitnessOperator(const TerrainLibrary::LOD & tl)
            : _terrainLibrary(tl) { }

        Scalar operator()(Chromosome & c) {
            c.setValues();
            return terrainLibraryFitness(_terrainLibrary);
        }
        
    protected:
        const TerrainLibrary::LOD & _terrainLibrary;
    };

#if 0
    // Fitness operator measuring ???
    class AggregateLibraryFitnessOperator
            : public SimilarityGA::FitnessOperator {
    public:

    };
#endif
}


// Chromosome constructor -- give each gene its index
SimilarityChromosome::SimilarityChromosome() {
    for (IndexType i = 0; i < size(); ++i)
        (*this)[i].index = i;
}

void SimilarityChromosome::setValues() const {
//    HACK_weights.resize(size());
//    HACK_variances.resize(size());
//    scalar_t sumW = 0;
//    for (int i = 0; i < size(); ++i)
//        sumW += (*this)[i].weight;
//    for (int i = 0; i < size(); ++i) {
//        HACK_weights[i]   = (*this)[i].weight / sumW;
//        HACK_variances[i] = (*this)[i].variance
//                            * std::pow(1.0f, (*this)[i].magnitude);
//    }
}            


// GA constructor -- set up the GA operators and parameters
SimilarityGA::SimilarityGA(const TerrainLibrary::LOD & tl) {
    // Make sure all the TerrainSamples are studied (and cached!)
    tl.ensureStudied();

    // Set up the initialization operators
    addInitializationOperator(new DefaultInitializationOperator(), 0.2f);
    addInitializationOperator(new RandomInitializationOperator(), 0.8f);

    // Set up the crossover operators
    addCrossoverOperator(new StandardCrossoverOperator());
    addCrossoverOperator(new WeightCrossoverOperator());

    // Set up the mutation operators (keeping track of the ones we care about)
    addMutationOperator(new WeightMutationOperator());
    addMutationOperator(new VarianceMutationOperator());
    addMutationOperator(new VarianceMagnitudeMutationOperator());

    // Set up the fitness calculation operators
    addFitnessOperator(new AggregateLibraryFitnessOperator(tl));

    // Set up the GA parameters
    setPopulationSize(20);
    setEvolutionCycles(200);
    setCrossoverProbability(0.2f);
    setMutationProbability(0.1f);
    setSelectionRatio(0.75f);
    setElitismRatio(0.1f);
    setCrossoverRatio(0.4f);
    setMutationRatio(0.2f);
}
