/*
 * File: terrain-operations.hpp
 *
 * Author: Ryan L. Saunders & Mike Ong
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 *      This file declares the functions that support the 3D terrain generation
 *      genetic algorithm.
 *
 *      The generateTerrain(...) function is the entry point of the GA. It
 *      sets up the GA, starts it running, and returns the result as a
 *      heightfield of the appropriate dimensions.
 *
 *      The initializeChromosome(...) function scans a heightfield and
 *      populates a chromosome with genes that are "compatible" with the
 *      corresponding locations in the input heightfield. This is important
 *      because the input heightfield may be the result of up-scaling a
 *      lower-resolution terrain, and may have insufficient detail.
 *
 *      The renderChromosome(...) and renderGene(...) functions turn a
 *      chromosome and a gene (respectively) back into a set of elevation
 *      values (pixels) in a heightfield. This must be done after each
 *      iteration of the GA in order to evaluate the fitness of a chromosome.
 *
 *      The evaluateFitness(...) function examines a chromosome to determine
 *      its fitness according to a variety of metrics, and returns an overall
 *      fitness value as a floating-point number between 0 and 1 (inclusive).
 *
 *      The evaluateCompatibility(...) function examines a pair of genes to
 *      determine how well they match up (i.e., could one replace the other
 *      without damaging the terrain?). This is used initially to populate
 *      a higher-LOD chromosome from a lower-LOD chromosome, and also within
 *      an iteration of the GA to do mutation and crossover of chromosome
 *      data.
 */

#ifndef TERRAINOSAURUS_GENETICS_TERRAIN_OPERATIONS
#define TERRAINOSAURUS_GENETICS_TERRAIN_OPERATIONS

// Import library configuration
#include <terrainosaurus/terrainosaurus-common.h>


// Import Map, TerrainLibrary, and terrain GA data structures
#include <terrainosaurus/data/Map.hpp>
#include <terrainosaurus/rendering/MapRasterization.hpp>
#include <terrainosaurus/data/TerrainLibrary.hpp>
#include "TerrainChromosome.hpp"


// Interface functions for generating and manipulating heightfield data
// via a 2D genetic algorithm
namespace terrainosaurus {

    // Generate a heightfield by repeatedly applying the GA at successively
    // finer LODs and returning the best candidate as 'hf'.
    void generateTerrain(Heightfield & hf, MapConstPtr m,
                         const Block & bounds, IndexType levelOfDetail);


    // Fill a Chromosome by finding a set of Genes that are compatible with
    // the data in pattern.
    void initializeChromosome(TerrainChromosome & c, IndexType lod,
                              TerrainSampleConstPtr pattern,
                              const MapRasterization & raster);

    // Create a random Gene by chosing a random location within a terrain
    // sample of the given terrain type and LOD
    TerrainChromosome::Gene createRandomGene(TerrainTypeConstPtr terrainType,
                                             IndexType levelOfDetail);

    // Scale up a Chromosome to the next level of detail
//    void incrementLevelOfDetail(TerrainChromosome & dst,
//                                const TerrainChromosome & src);

    // Render a singe gene to an image (note -- not especially efficient!)
    Heightfield renderSoloGene(const TerrainChromosome::Gene & g);

    // Generate a heightfield by splatting together the Gene data in c
    void renderChromosome(Heightfield & hf,
                          const TerrainChromosome & c);
    void renderGene(Heightfield & hf, Heightfield & sum,
                    const TerrainChromosome::Gene & g);


    // Calculate a value in [0, 1] representing the fitness of a Chromosome
    scalar_t evaluateFitness(const TerrainChromosome & c);

    // Calculate a value in [0, 1] representing the compatibility of a Gene
    // with a slot in a Chromosome
    scalar_t evaluateCompatibility(const TerrainChromosome & c,
                                   IndexType i, IndexType j,
                                   const TerrainChromosome::Gene & g2);

    // Heightfield measurement operations for a particular slot in a Chromosome.
    // These operations return average values across the region of the pattern
    // heightfield covered by the gene at (i, j).
    scalar_t elevationMean(const TerrainChromosome & c, IndexType i, IndexType j);
    const Vector2D & gradientMean(const TerrainChromosome & c, IndexType i, IndexType j);
    const Vector2D & elevationRange(const TerrainChromosome & c, IndexType i, IndexType j);
    const Vector2D & slopeRange(const TerrainChromosome & c, IndexType i, IndexType j);

    // Heightfield measurement operations for a Gene.
    // These operations return average values of the data represented by the
    // Gene, and take into account any transformation.
    scalar_t elevationMean(const TerrainChromosome::Gene & g);
    Vector2D gradientMean(const TerrainChromosome::Gene & g);
    Vector2D elevationRange(const TerrainChromosome::Gene & g);
    Vector2D slopeRange(const TerrainChromosome::Gene & g);

/*---------------------------------------------------------------------------*
 | Mike's chromosome methods: crossover and mutation
 *---------------------------------------------------------------------------*/
	  void Crossover(TerrainChromosome* l,TerrainChromosome* r,float ratio,IndexType lod,const Heightfield & pattern,const MapRasterization & raster);	
		void Mutate(TerrainChromosome* c,float ratio, IndexType lod,const Heightfield & pattern,const MapRasterization & raster);        
		void generateNewIndividuals(TerrainChromosome*** c,const MapConstPtr m, const MapRasterization raster,int start,int end);
		float evaluateFitness_temporary(const TerrainChromosome & c);
		void ComputeFitnessOfPopulation(TerrainChromosome** c, float* fitness_A,double& totalfitness);
		void NormalizeFitnessValues(float* fitness_A, float* cumulativefitness_A, double totalfitness);		
		float GetRandomFloat(float start,float end);
		int GetRandomInt(int start,int end);
};

#endif
