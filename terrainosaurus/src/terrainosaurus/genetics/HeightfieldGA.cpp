/**
 * \file    HeightfieldGA.cpp
 *
 * \author  Ryan L. Saunders & Mike Ong
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the heightfield-generation genetic algorithm
 *      using the Inca GA framework.
 */

// Import class definition
#include "HeightfieldGA.hpp"
using namespace terrainosaurus;


namespace terrainosaurus {
    // Forward declarations of operator classes
    class CreateRandomOperator;
    class CrossRectangularRegionsOperator;
    class MutateVerticalOffsetOperator;
    class MutateVerticalScaleOperator;
    class MutateVerticalRotationOperator;
    class MutateHorizontalTranslationOperator;
    class MutateHorizontalJitterOperator;
}



/**
 * The CrossRectangularRegionsOperator implements a simple crossover operator
 * that swaps small rectangular clusters of genes from one chromosome with the
 * matching genes (i.e., with the same indices) from the other chromosome.
 *
 * The percentage of genes that are swapped is random but, on average, will
 * approximate the value of the crossoverRatio() property, which is a scalar
 * in the range [0..1]. Note that the maximum amount of crossover occurs with
 * crossoverRatio() == 0.5, since values above this are symmetric with those
 * below 0.5 (e.g. 0.4 and 0.6 will both result in each chromosome having
 * 40% from one chromosome and 60% from the other).
 */
class terrainosaurus::CrossRectangularRegionsOperator
        : public HeightfieldGA::CrossoverOperator {
public:
    // Constructor
    explicit CrossRectangularsRegionsOperator(scalar_t r = 0.5f, int w = 5)
        : _crossoverRatio(r), _regionWidth(w) { }

    // Properties
    scalar_t crossoverRatio() const    { return _crossoverRatio; }
    void setCrossoverRatio(scalar_t r) { _crossoverRatio = r; }
    int regionWidth() const    { return _regionWidth; }
    void setRegionWidth(int w) { _regionWidth = w; }

    // Function call operator
    void operator()(Chromosome & c1, Chromosome & c2) {
        // Make sure the two chromosomes are the same size
        if (c1.sizes() != c2.sizes()) {
            GeneticAlgorithmException e;
            e << typeid(this).name() << ": Cannot cross differently-sized "
                 "chromosomes -- c1(" << c1.sizes() << "), c2(" << c2.sizes() << ')';
            throw e;
        }

        // The random number generator we'll use throughout
        RandomUniform<int> randomInt;

        // Decide how many chunks we'll swap (between 0 and all of them)
        int size = c1.size();
        int meanChunkSize = regionWidth() * regionWidth() / 4;
        int num = randomInt(0, int(size * ratio / meanChunkSize));

        // For each chunk that we intend to swap...
        Pixel start, end, idx;
        Dimension regionSize;
        int crossed = 0;
        for (int k = 0; k < num; ++k) {
            // Pick a random starting X,Y point...
            start[0] = randomInt(0, chromoSizes[0] - 1);
            start[1] = randomInt(0, chromoSizes[1] - 1);

            //...and rectangular region size...
            regionSize[0] = randomInt(0, regionWidth());
            regionSize[1] = randomInt(0, regionWidth());

            //...then calculate the ending X,Y point...
            end[0] = std::min(start[0] + regionSize[0], chromoSizes[0] - 1);
            end[1] = std::min(start[1] + regionSize[1], chromoSizes[1] - 1);

            //...and swap everything in between
            for (idx[0] = start[0]; idx[0] <= end[0]; ++idx[0])
                for (idx[1] = start[1]; idx[1] <= end[1]; ++idx[1]) {
                    swap(c1(idx), c2(idx));
                    ++crossed;
                }
        }
        std::cerr << "Crossed " << crossed << " of " << size << " genes\n"
                     "Target percentage: " << (crossoverRatio() * 100) << '\n'
                     "Actual percentage: " << (crossed * 100.0f / size) << '\n';
    }

protected:
    scalar_t _crossoverRatio;
    int      _regionWidth;
};


// Constructor
HeightfieldGA::HeightfieldGA() {
    // Set up the initialization operators
    addInitializationOperator(new CreateRandomChromosomeOperator(), 1.0f);

    // Set up the crossover operators
    addCrossoverOperator(new CrossRectangularRegionsOperator(), 1.0f);

    // Set up the mutation operators

    // Set up the fitness calculation operators
}

// Destructor
HeightfieldGA::~HeightfieldGA() { }
