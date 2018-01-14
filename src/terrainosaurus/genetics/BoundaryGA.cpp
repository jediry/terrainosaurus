/** -*- C++ -*-
 *
 * \file    BoundaryGA.cpp
 *
 * \author  Ryan L. Saunders & Mike Ong
 *
 * Copyright 2005, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file implements the boundary-generation genetic algorithm
 *      using the Inca GA framework.
 */

/* Possible reformulation of this:
 *      remove the max angle constraint
 *      introduce additional fitness components:
 *          endpoint-distance
 *      introduce new operations
 *          curl/uncurl (or compress/expand)
 *          mirror subsequence
 *      calculate number of segments based on expected curve length, not
 *        linear distance
 */

// Import class definition
#include "BoundaryGA.hpp"

using namespace terrainosaurus;
using namespace inca;
using namespace inca::math;


namespace terrainosaurus {
    // Forward declarations of operator classes

    class RandomAngleInitializationOperator;
    class StraightLineInitializationOperator;

    class SpliceSubsequencesCrossoverOperator;

    class RandomBendMutationOperator;

    class SmoothnessFitnessOperator;
}

// HACK: remove this once I track down the cause of the OOB angles
void findBadAngles(const BoundaryGA::Chromosome &c) {
    for (IndexType i = 0; i < IndexType(c.size()); ++i) {
        const BoundaryGA::Gene & g = c[i];
        if (std::abs(g.relativeAngle) > PI<scalar_t>())
            INCA_DEBUG("PI warning!")
        if (std::abs(g.absoluteAngle) > PI<scalar_t>())
            INCA_DEBUG("PI warning!")
    }
}

void fixOverrunAngle(BoundaryGA::Gene & g) {
    if (g.relativeAngle > PI<scalar_t>())
        g.relativeAngle = -2 * PI<scalar_t>() + g.relativeAngle;
    else if (g.relativeAngle < -PI<scalar_t>())
        g.relativeAngle = 2 * PI<scalar_t>() + g.relativeAngle;
}

#define GAUSSIAN 1
#define UNIFORM  2
#define DISTRIBUTION GAUSSIAN
class terrainosaurus::RandomAngleInitializationOperator
        : public BoundaryGA::InitializationOperator {
public:
    void operator()(Chromosome & c) {
        // XXX The GA should take a "Derived" parameter, so that this cast
        // need not be done in client code.

        // Make sure the chromosome has the right number of segments
        const BoundaryGA & ga = static_cast<const BoundaryGA &>(owner());
        c.resize(ga.segmentCount());

        // Randomly choose angles (respecting the absolute angle constraint)
#if DISTRIBUTION == GAUSSIAN
        RandomGaussian<scalar_t> randomAngle;
#elif DISTRIBUTION == UNIFORM
        RandomUniform<scalar_t> randomAngle;
#endif
        scalar_t maxAbs = ga.maxAbsoluteAngle();
        scalar_t cAbs = 0.0f;
        scalar_t tolerance = 0.01f;
        for (IndexType i = 0; i < IndexType(c.size()); ++i) {
            Gene & g = c[i];
            scalar_t minAngle = -(maxAbs - tolerance) - cAbs;
            scalar_t maxAngle =  (maxAbs - tolerance) - cAbs;
//            INCA_DEBUG("Angle between " << minAngle << " and " << maxAngle)
#if DISTRIBUTION == GAUSSIAN
            scalar_t mean = cAbs;
//            scalar_t mean = (maxAngle + minAngle) / scalar_t(2);
            scalar_t stddev = (maxAngle - minAngle) / 4;
//            INCA_DEBUG("Mean = " << mean << "  stddev = " << stddev);
            g.relativeAngle = randomAngle(mean, stddev);
#elif DISTRIBUTION == UNIFORM
            g.relativeAngle = randomAngle(minAngle, maxAngle);
#endif
            if (g.relativeAngle > maxAngle)      g.relativeAngle = maxAngle;
            else if (g.relativeAngle < minAngle) g.relativeAngle = minAngle;
//            INCA_DEBUG("Angle is " << g.relativeAngle);
            scalar_t oldAbs = cAbs;
            cAbs += g.relativeAngle;
            g.absoluteAngle = cAbs;
            fixOverrunAngle(g);

            if (std::abs(g.absoluteAngle) > maxAbs)
                INCA_DEBUG("Abs warning!");
            if (g.absoluteAngle > PI<scalar_t>() || g.absoluteAngle < -PI<scalar_t>())
                INCA_DEBUG("PI warning!");
            if (g.relativeAngle > PI<scalar_t>() || g.relativeAngle < -PI<scalar_t>())
                INCA_DEBUG("PI warning!");
        }
        findBadAngles(c);
    }            
};


class terrainosaurus::StraightLineInitializationOperator
        : public BoundaryGA::InitializationOperator {
public:
    void operator()(Chromosome & c) {
        // Make sure the chromosome has the right number of segments
        const BoundaryGA & ga = static_cast<const BoundaryGA &>(owner());
        c.resize(ga.segmentCount());

        // Set all angles to zero
        for (IndexType i = 0; i < IndexType(c.size()); ++i) {
            Gene & g = c[i];
            g.relativeAngle = 0.0f;
            g.absoluteAngle = 0.0f;
        }
    }
};


class terrainosaurus::SpliceSubsequencesCrossoverOperator
        : public BoundaryGA::CrossoverOperator {
public:
    void operator()(Chromosome & c1, Chromosome & c2) {
        // Ensure that the chromosomes are the same size (sanity check)
        if (c1.size() != c2.size()) {
            INCA_ERROR("Mismatched chromosome size in boundary GA "
                       << __FUNCTION__ << ": "
                       << c1.size() << " != " << c2.size())
            return;
        }

        findBadAngles(c1);
        findBadAngles(c2);

        // Pick a random (non-end) cut point
        IndexType cutPoint = RandomUniform<IndexType>(1, c1.size() - 2)();
        
        // Exchange all genes from that point on
        for (IndexType i = cutPoint; i < IndexType(c1.size()); ++i) {
            // Get the genes we're swapping
            Gene & g1 = c1[i];
            Gene & g2 = c2[i];
      
            // Exchange the relative angles
            scalar_t tmp = g2.relativeAngle;
            g2.relativeAngle = g1.relativeAngle;
            g1.relativeAngle = tmp;
                     
            // Recalculate the absolute anlges
//            g1.absoluteAngle = c1[i-1].absoluteAngle + g1.relativeAngle;
//            g2.absoluteAngle = c2[i-1].absoluteAngle + g2.relativeAngle;
        }
        
        // Clean up any max angle violations
        const BoundaryGA & ga = static_cast<const BoundaryGA &>(owner());
        ga.enforceMaxAbsoluteAngle(c1, cutPoint);
        ga.enforceMaxAbsoluteAngle(c2, cutPoint);
    }            
};


class terrainosaurus::RandomBendMutationOperator
        : public BoundaryGA::MutationOperator {
public:
    void operator()(Gene & g) {
        // Find the absolute angle up to (not including) this gene
        scalar_t prevAbs = g.absoluteAngle - g.relativeAngle;   

        // Calculate a random mutation w/in the legal range
        const BoundaryGA & ga = static_cast<const BoundaryGA &>(owner());
        scalar_t maxAbs = ga.maxAbsoluteAngle();
        scalar_t da = RandomUniform<scalar_t>()(maxAbs - g.absoluteAngle,
                                               -maxAbs - g.absoluteAngle);

        // Calculate the new relative & absolute angles
        g.absoluteAngle += da;
        g.relativeAngle += da;
        fixOverrunAngle(g);

        if (std::abs(g.absoluteAngle) > maxAbs)
            INCA_DEBUG("Max warning");
        if (std::abs(g.relativeAngle) > PI<scalar_t>())
            INCA_DEBUG("PI warning!");
        if (std::abs(g.absoluteAngle) > PI<scalar_t>())
            INCA_DEBUG("PI warning!");
    }            
};


#if 0
class terrainosaurus::CoilUncoilMutationOperator
        : public BoundaryGA::MutationOperator {
public:
    void operator()(Gene & g) {
    }            
};
#endif


class terrainosaurus::SmoothnessFitnessOperator
        : public BoundaryGA::FitnessOperator {
public:
    Scalar operator()(Chromosome & c) {
        // First, make sure this chromosome is legal
        const BoundaryGA & ga = static_cast<const BoundaryGA &>(owner());
        ga.enforceMaxAbsoluteAngle(c);

        Scalar smoothness = ga.smoothness();
        Scalar fitness = 0;

#if 0
        // HACK--this needs a home    
        const scalar_t SS = 1.1f;

        // Next, calculate the fitness, using two LOD levels
        int totalCount = c.size() + c.size() / 2 + c.size() % 2;
        for (int genes = 1; genes <= 2; ++genes) {   // For different LODs...
            int count = c.size() / genes + (c.size() % genes > 0);
            Scalar lodFitness = 0;
            for (IndexType i = 0; i < IndexType(c.size()); i += genes) {  // For each set of points
                Scalar s = std::sin((PI<scalar_t>() - std::abs(c[i].relativeAngle)) / 2
                                        * (SS - smoothness));
                lodFitness += s;
                if (s < 0) {
                    INCA_DEBUG("Ack! angle " << c[i].relativeAngle << " has fitness of " << s)
                    int x = 1;
                }
//                lodFitness += std::sin((PI<scalar_t>() - std::abs(c[i].relativeAngle)) / 2
//                                        * (1.1f - smoothness));
            }
//            INCA_DEBUG("LOD " << genes << " fitness is " << (lodFitness / count))
            fitness += lodFitness;  // Actually, lodFitness / count * count
        }
        Scalar maxFitness = (SS - smoothness > 1)
                    ? 1.0f
                    : std::sin(PI<scalar_t>() / 2 * (SS - smoothness));
#else
        int totalCount = c.size();
        for (IndexType i = 0; i < IndexType(c.size()); i += 1) {  // For each set of points
            Scalar s = std::sin((PI<scalar_t>() - std::abs(c[i].relativeAngle)) - smoothness * PI<scalar_t>() / Scalar(2));
            fitness += std::max(scalar_t(0), s);
        }
        Scalar maxFitness = 1.0f;
#endif
        
        // Finish the weighted average and normalize fitness to [0,1]
        INCA_DEBUG("Sum " << fitness << " aggregate " << (fitness / totalCount) << " normalized " << (fitness / totalCount / maxFitness))
        fitness /= totalCount * maxFitness;

        if (fitness < 0)
            fitness = 0.0f;

        INCA_DEBUG("Boundary fitness was " << fitness)
        return fitness;
    }
};


BoundaryGA::BoundaryGA() {
    // Set up the operators
    addInitializationOperator(new RandomAngleInitializationOperator());
//    addInitializationOperator(new StraightLineInitializationOperator());
    
    addCrossoverOperator(new SpliceSubsequencesCrossoverOperator());

    addMutationOperator(new RandomBendMutationOperator());
//    addMutationOperator(new CoilUncoilMutationOperator());

    addFitnessOperator(new SmoothnessFitnessOperator());

}

// Additional properties
scalar_t BoundaryGA::maxAbsoluteAngle() const { return _maxAbsoluteAngle; }
void BoundaryGA::setMaxAbsoluteAngle(scalar_arg_t a) { _maxAbsoluteAngle = a; }
scalar_t BoundaryGA::smoothness() const { return _smoothness; }
void BoundaryGA::setSmoothness(scalar_arg_t s) { _smoothness = s; }
int BoundaryGA::segmentCount() const { return _segmentCount; }
void BoundaryGA::setSegmentCount(int c) { _segmentCount = c; }


void BoundaryGA::enforceMaxAbsoluteAngle(Chromosome & c, IndexType start) const {
    scalar_t absoluteAngle = start == 0 ? scalar_t(0) : c[start-1].absoluteAngle;
    for (IndexType i = start; i < IndexType(c.size()); ++i) {
        Gene & g = c[i];
        absoluteAngle += g.relativeAngle;
#if 1
        if (g.absoluteAngle > maxAbsoluteAngle()) {
            INCA_DEBUG("Corrected positive angle [" << i << "] " << g.absoluteAngle)
            g.relativeAngle -= g.absoluteAngle - maxAbsoluteAngle();
            fixOverrunAngle(g);
            g.absoluteAngle = maxAbsoluteAngle();
            INCA_DEBUG("                       to     " << g.absoluteAngle)
        } else if (g.absoluteAngle < -maxAbsoluteAngle()) {
            INCA_DEBUG("Corrected negative angle [" << i << "] " << g.absoluteAngle)
            g.relativeAngle -= g.absoluteAngle + maxAbsoluteAngle();
            fixOverrunAngle(g);
            g.absoluteAngle = -maxAbsoluteAngle();
            INCA_DEBUG("                       to     " << g.absoluteAngle)
//        } else {
//            INCA_DEBUG("Left OK angle [" << i << "] " << g.absoluteAngle << " alone")
        }
#else
        if (g.absoluteAngle > maxAbsoluteAngle()) {
            scalar_t da = g.absoluteAngle - maxAbsoluteAngle();
            g.relativeAngle -= 2 * da;
            g.absoluteAngle = maxAbsoluteAngle() - da;
        } else if (g.absoluteAngle < -maxAbsoluteAngle()) {
            scalar_t da = g.absoluteAngle + maxAbsoluteAngle();
            g.relativeAngle -= 2 * da;
            g.absoluteAngle = -maxAbsoluteAngle() - da;
        }
#endif
        absoluteAngle = g.absoluteAngle;
    }
    findBadAngles(c);
}            


void BoundaryGA::run(int segments, scalar_t smoothness) {
    INCA_INFO("Generating boundary of " << segments << " segments, with "
              "smoothness " << smoothness)
    setSegmentCount(segments);
    setSmoothness(0.0f);
    Superclass::run();
}
