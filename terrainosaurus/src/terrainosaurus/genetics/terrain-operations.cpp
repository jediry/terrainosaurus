/*
 * File: terrain-operations.cpp
 *
 * Author: Ryan L. Saunders & Mike Ong
 *
 * Copyright 2004, Ryan L. Saunders. Permission is granted to use and
 *      distribute this file freely for educational purposes.
 *
 * Description:
 */


// Import application class
#include <terrainosaurus/MapExplorer.hpp>

// Import function prototypes and class definitions
#include "terrain-operations.hpp"
#include <terrainosaurus/rendering/MapRasterization.hpp>

// Import raster operations
#include <inca/raster/operators/arithmetic>
#include <inca/raster/operators/statistic>
#include <inca/raster/operators/selection>
#include <inca/raster/operators/transformation>

using namespace inca::math;
using namespace inca::raster;
using namespace terrainosaurus;
using TerrainChromosome::Gene;

#include <stdlib.h>
#define NUM_OF_CHROMOSOME 5
#define NUM_OF_EVOLUTIONCYCLES 5

// This function runs the entire GA
void terrainosaurus::generateTerrain(Heightfield & hf, MapConstPtr m,
                                     const Block & region, IndexType lod) {
    scalar_t resolution = MapExplorer::instance().terrainLibrary()->resolution(lod);
//    Dimension d(SizeType(region.size(0) * resolution),
//                SizeType(region.size(1) * resolution));
    Dimension d(m->terrainLibrary->terrainType(1)->samples[0]->elevation(0).sizes());
    cerr << "Generating terrain of size " << d[0] << "x" << d[1] << endl;

#if 0
    // Make sure we have elevation data in-memory for the terrain types
    // we are going to need
    cerr << "Preloading in-use TerrainTypes:" << endl;
    TerrainLibraryPtr library = map->terrainLibrary;
    std::vector<bool> inUse(library->size(), false);
    Map::face_const_iterator r, rEnd = m->faces().end();
    for (r = m->faces().begin(); r != rEnd; ++r)
        inUse[r->terrainType()->id()] = true;
    std::vector<bool>::iterator i;
    for (int i = 0; i < library->size(); ++i)
        if (inUse[i]) {
            TerrainTypePtr tt = library->terrainType(i);
            cerr << "\t" << tt->name << ": "
                 << tt->samples.size() << "samples" << endl;
            tt->ensureLoaded();
        }
#endif

    // Have the rendering system determine region membership at each LOD
    MapRasterization raster(m);//, region);
//    TerrainChromosome c;
    TerrainTypeConstPtr tt = m->terrainLibrary->terrainType(1);
//    tt->samples[0]->ensureAnalyzed();
//    initializeChromosome(c, 0, tt->samples[0], raster);
//    TerrainChromosome dd = c;
//    for (int it = 0; it < 20; ++it) {
//        std::cerr << "Beginning naive GA iteration " << it << std::endl;
//        for (int i = 0; i < c.size(0); ++i) {
//            std::cerr << "Row " << i << std::endl;
//            for (int j = 0; j < c.size(1); ++j) {
//                TerrainChromosome::Gene g = createRandomGene(tt, 0);
//                if (evaluateCompatibility(c, i, j, g) > evaluateCompatibility(c, i, j, c.gene(i, j)))
//                    c.gene(i, j) = g;
//            }
//        }
//        evaluateFitness(dd);
//    }
//    renderChromosome(hf, dd);
    /// A few questions:
    /// 1. where's the seed for the random number generator??
    /// 2. Since we want the GA to generate terrain that are kinda close to the original, we might need gradient based
    /// operators instead of the original operators. However, is this what we really want in the terrain engine?
    /// ------------ Section Description --------------//
    /// Creates a population of 30 individuals. Note that the individuals should vary according to the size of the map
    /// by some ratio
    /// To maintain population diversity, only 75% of the individuals from the old pop 
    /// will be selected to form the new population for the next evolution cycle
    /// ------------ End Section Description --------------//
    
    // create the initial population, we'll do this just once      
    float mutationratio = 0.1f;
    float crossoverratio = 0.2f;
    TerrainChromosome** c_A = new TerrainChromosome*[NUM_OF_CHROMOSOME];
    TerrainChromosome** newpopc_A = NULL;
    generateNewIndividuals(&c_A,m,raster,0,NUM_OF_CHROMOSOME);    
    float fitness_A[NUM_OF_CHROMOSOME],cumulativefitness_A[NUM_OF_CHROMOSOME];
    double totalfitness=0.0;
    float valuetoselect;
    int numberToSelect = int(NUM_OF_CHROMOSOME * 0.75f + 0.5f);
    int remainingToGenerate = NUM_OF_CHROMOSOME - numberToSelect;
    int j,k,l;
    int randnum;
    // Now comes the GA evolution cycle, for now the evolution cycle is 75 cycles
    for(int i = 0; i < NUM_OF_EVOLUTIONCYCLES; i++)
    {
      // Evaluate fitness of all of the individuals
      ComputeFitnessOfPopulation(c_A,fitness_A,totalfitness);            
      // normalize the results and generate the cumulative distribution fitness array
      NormalizeFitnessValues(fitness_A,cumulativefitness_A,totalfitness);      
      // selection, generate a random number in the range of 0 - maxfitness and 
      // select the individual for the new pop
      newpopc_A = new TerrainChromosome*[NUM_OF_CHROMOSOME];
      for( j = 0; j < numberToSelect; j++)
      {
        newpopc_A[j] = new TerrainChromosome();        
        // selects old individuals

        for(k = 0; k < NUM_OF_CHROMOSOME; k++)
        {
          randnum = rand();
          valuetoselect = ((float)randnum / (float)RAND_MAX) * cumulativefitness_A[NUM_OF_CHROMOSOME-1];
          // should use binary search... but I'll just use linear search for now
          if(valuetoselect < cumulativefitness_A[k])
          {
            newpopc_A[j] = new TerrainChromosome(*(c_A[k]));
            //(*newpopc_A[j]).resize((*c_A[k]).sizes(),false); 
            //(*newpopc_A[j]).genes() = (*c_A[k]).genes(); 
            break;
          }
          else if(k == NUM_OF_CHROMOSOME-1)
          {
            newpopc_A[j] = new TerrainChromosome(*(c_A[k]));
            break;
//            (*newpopc_A[j]).resize((*c_A[k]).sizes(),false); 
            //(*newpopc_A[j]).genes() = (*c_A[k]).genes();
          }
        }
      }
      // create new population -- i.e. we'll fill up any empty slots in the population with new individuals
      generateNewIndividuals(&newpopc_A,m,raster,j,NUM_OF_CHROMOSOME);        
      // crossover & mutations      
      for( j = 0; j < NUM_OF_CHROMOSOME; j++)
      {
        // do crossover here
        if(GetRandomFloat(0.0f,1.0f) <= crossoverratio)
        {
          l = rand() % (NUM_OF_CHROMOSOME-1);
          if(l != j)
            Crossover(newpopc_A[j],newpopc_A[l],0.5f,0, tt->samples[0]->elevation(0), raster);
        }
        // do mutation here
        if(GetRandomFloat(0.0f,1.0f) <= mutationratio)
        {
          Mutate(newpopc_A[j],0.5f,0, tt->samples[0]->elevation(0), raster);
        }
      }
      // cleanup
//      for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
      //{
        //delete c_A[i];      
      //}
//        delete []c_A;   
      c_A = newpopc_A;                          
    }
    // finds strongest individual
    int offset = 0; float maxfitness = 0.0f;
    float value;
    ComputeFitnessOfPopulation(c_A,fitness_A,totalfitness);
    for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
    {
      //value = evaluateFitness(*c_A[i]);
      value = fitness_A[i];
      if(maxfitness < value)
      {
        maxfitness = value;
        offset = i;
      }
    }
    
    // we'll render the strongest individual eventually
    renderChromosome(hf, *c_A[offset]);    
    cerr << "Range of result is " << range(hf) << endl;
    
    // cleanup
//    for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
//    {
//      delete c_A[i];      
//    }
//    delete []c_A;   

    std::cerr<< "Done executing GA" << endl;

}


// This function makes a chromosome from a heightfield
void terrainosaurus::initializeChromosome(TerrainChromosome & c, IndexType lod,
                                          TerrainSampleConstPtr pattern,
                                          const MapRasterization & raster) {
    // Point the Chromosome to its pattern/LOD
    c._pattern = pattern;
    c._levelOfDetail = lod;

    // Figure out how many genes we want in each direction    
    Dimension size = Dimension(pattern->sizes(lod)) / TerrainChromosome::geneSpacing(lod);
    c.resize(size);

    // Populate the gene grid with random data
    Pixel idx;
    for (idx[0] = 0; idx[0] < size[0]; ++idx[0])
        for (idx[1] = 0; idx[1] < size[1]; ++idx[1])
            c.gene(idx) = createRandomGene(raster.terrainTypeAt(idx, lod), lod);

    cerr << "Initialized chromosome with " << size[0] << "x" << size[1] << " genes\n";
}                                          
void terrainosaurus::Crossover(TerrainChromosome *l,TerrainChromosome *r,float ratio,IndexType lod,const Heightfield & pattern,const MapRasterization & raster)
{
    Gene tempval;
    int i,j,k;
    Dimension size = Dimension(pattern.sizes()) / TerrainChromosome::geneSpacing(lod);
    Pixel idx,endidx,curridx;
    // we'll choose a random x,y coordinate  for upper left corner of the region
        
      // for now we'll only allow crossover over 10 places, this ought to be a ratio
      // depending on the size of the map
      int num = GetRandomInt(1,10); 
      int sizex,sizey;
      Pixel* pics = new Pixel[num];
      Pixel* endpics = new Pixel[num];
      for(i = 0; i < num; i++)
      {
        pics[i][0] = GetRandomInt(0,size[0]);
        pics[i][1] = GetRandomInt(0,size[1]);
        sizex = pics[i][0] + 1 + GetRandomInt(1,4);
        sizey = pics[i][1] + 1 + GetRandomInt(1,4);
        if(sizex > size[0])
          sizex = size[0];
        if(sizey > size[1])
          sizey = size[1];
//cerr << pics[i][0] << " " << pics[i][1] <<  " " << sizex << " " << sizey << endl;
        endpics[i][0] = sizex;
        endpics[i][1] = sizey;
      }
      for(k=0; k < num; k++)
        for(i = pics[k][0]; i < endpics[k][0]; i++)
          for(j =pics[k][1]; j < endpics[k][1]; j++)
          {
            curridx[0] = i; curridx[1] = j;
            tempval = l->gene(curridx);
            l->gene(curridx) = r->gene(curridx);
            r->gene(curridx) = tempval;
          }

      delete []pics;
      delete []endpics;    
}
void terrainosaurus::Mutate(TerrainChromosome *c, float ratio,IndexType lod,const Heightfield & pattern,const MapRasterization & raster)
{
    Dimension size = Dimension(pattern.sizes()) / TerrainChromosome::geneSpacing(lod);
    Pixel idx;
    for (idx[0] = 0; idx[0] < size[0]; ++idx[0])
        for (idx[1] = 0; idx[1] < size[1]; ++idx[1])
        {
          if((rand() / (float)RAND_MAX) <= ratio)
            c->gene(idx).scale *= (1.0f+(rand() / (float)RAND_MAX) * pow(-1,rand())); 
        }
}
//***************************************************************************//
int terrainosaurus::GetRandomInt(int start,int end)
{
  int range = abs(end - start);
  return (rand() % range) + start;
}

float terrainosaurus::GetRandomFloat(float start,float end)
{
  float range = abs(end - start);
  return ((rand() / (float)RAND_MAX) * range) + start ;
}
void terrainosaurus::generateNewIndividuals(TerrainChromosome*** c_A,const MapConstPtr m, const MapRasterization raster,int start,int end)
{
  //TerrainChromosome** c_A = new TerrainChromosome*[NUM_OF_CHROMOSOME];
  // create the initial population, we'll do this just once
  for(int i = start; i < end; i++)
  {
    (*c_A)[i] = new TerrainChromosome();
    initializeChromosome(*(*c_A)[i],0,m->terrainLibrary->terrainType(1)->samples[0], raster);            
  }  
}

/// -------- Function description ----------///
/// normalizes the fitness values array and computes the cumulative fitness array as well
/// -------- End Function description ----------///
void terrainosaurus::NormalizeFitnessValues(float* fitness_A, float* cumulativefitness_A, double totalfitness)
{
  float maxf=0.0f,minf=0.0f;float total = 0.0f;
  for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
  {
    if(maxf < fitness_A[i])
      maxf = fitness_A[i];
    if(minf > fitness_A[i])
      minf= fitness_A[i];
  }
  float range = maxf - minf;
  // rescale the fitness to range 0 to 1
  for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
  {
    fitness_A[i] = (fitness_A[i] - minf) / maxf;
    fitness_A[i] = pow(fitness_A[i],3) * 3.0f;
    total += fitness_A[i];
  }
  for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
  {
    // we don't need to make this super precise, float would be good enough
    cerr << fitness_A[i] << " ";
    fitness_A[i] = float(fitness_A[i]/total);
    
    if(i > 0)    
      cumulativefitness_A[i] = fitness_A[i] + cumulativefitness_A[i-1];    
    else    
      cumulativefitness_A[i] = fitness_A[i];    

  }
  cerr << endl << "Cumulative fitness: ";
  for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
  {
    cerr << cumulativefitness_A[i] << " ";
  }
  cerr << endl;
}
void terrainosaurus::ComputeFitnessOfPopulation(TerrainChromosome** c, float* fitness_A, double& totalfitness)
{
  float maxfitness = 0.0f;
  totalfitness = 0;
  for(int i = 0; i < NUM_OF_CHROMOSOME; i++)
  {
    fitness_A[i]=evaluateFitness(*c[i]);
    //cerr << fitness_A[i] << " ";
    //fitness_A[i] = 150000/fitness_A[i];
    if(maxfitness < fitness_A[i])
    {
      maxfitness = fitness_A[i];
    }
    totalfitness+= fitness_A[i];
  }
  cerr << "Maxfitness for this evolution cycle is " << maxfitness << endl;
}

// Added by Mike, This function determines how "good" a chromosome is
float terrainosaurus::evaluateFitness_temporary(const TerrainChromosome & c) {
    // First, turn the TerrainChromosome back into a heightfield
    Heightfield hf;
    renderChromosome(hf, c);

//    cerr << "Size of generated heightfield is " << Dimension(hf.sizes()) << endl;
    Heightfield & orig = const_cast<Heightfield &>(c.gene(0,0).sourceSample->elevation(c.levelOfDetail()));
//    Heightfield diff = hf - orig;
    Dimension size(math::min(orig.size(0), hf.size(0)), math::min(orig.size(1), hf.size(1)));
    scalar_t rtms = 0.0f;
    for (int i = 0; i < size[0]; i+=5)
        for (int j = 0; j < size[1]; j+=5)
            rtms += (hf(i,j) - orig(i,j)) * (hf(i,j) - orig(i,j));
    rtms = sqrt(rtms / size(0) * size(1));
//    cerr << "RMS value was " << rtms << endl;
//    cerr << "Functor measured it as " << rms(orig - hf) << endl;
    return rtms;
}

// This function creates a random Gene given a terrain type and LOD
TerrainChromosome::Gene
terrainosaurus::createRandomGene(TerrainTypeConstPtr tt, IndexType lod) {

    // The random number generators we'll use
    RandomUniform<IndexType> randomIndex;
    RandomUniform<scalar_t>  randomScalar;

    // The Gene we're initializing
    TerrainChromosome::Gene g;

    // First, pick a random sample from the requested TerrainType
    g.terrainType = tt;
    randomIndex.max = tt->samples.size() - 1;
    g.sourceSample = tt->samples[randomIndex()];

    // Radius and alpha mask at this LOD
    g.levelOfDetail = lod;
    IndexType radius = IndexType(TerrainChromosome::geneRadius(lod));
    g.mask = TerrainChromosome::geneMask(lod);

    // Pick a random X,Y coordinate pair within the safe region of that sample
    Dimension size  = Dimension(g.sourceSample->sizes(lod));
    randomIndex.min = radius;
    randomIndex.max = size[0] - radius; g.sourceCoordinates[0] = randomIndex();
    randomIndex.max = size[1] - radius; g.sourceCoordinates[1] = randomIndex();

    // Pick a random transformation
    randomScalar.max = PI<scalar_t>();  g.rotation  = randomScalar();
    randomScalar.max = scalar_t(10);    g.offset    = randomScalar();
//    randomScalar.min = scalar_t(0.2);
//    randomScalar.max = scalar_t(5);     g.scale     = randomScalar();
    g.offset = scalar_t(0);
    g.scale = scalar_t(1);

    // Pick a random jitter around the target coordinates
//    randomIndex.min = IndexType(-(1 - TerrainChromosome::geneOverlapFactor(lod)) * radius);
    randomIndex.min = IndexType(0);
    randomIndex.max = IndexType( 2 * (1 - TerrainChromosome::geneOverlapFactor(lod)) * radius);
    g.targetJitter[0] = randomIndex();
    g.targetJitter[1] = randomIndex();
    
    return g;
}


/*
void terrainosaurus::incrementLevelOfDetail(TerrainChromosome & dst,
                                            const TerrainChromosome & src) {
    // Turn the src TerrainChromosome into a heightfield
    Heightfield hf;
    renderChromosome(hf, src);

    // Scale up to the size of the next map LOD
    rescale<Bicubic>(hf, src.map->rasterization(src.levelOfDetail + 1).dimensions());

    // Generate a higher LOD TerrainChromosome by processing the scaled image
    initializeChromosome(dst, hf);
}*/


void terrainosaurus::renderChromosome(Heightfield & hf,
                                      const TerrainChromosome & c) {
    // Augment the heightfield with an alpha channel
    Heightfield sum; 

    // Make the output heightfield the correct size
//    hf.resize(c.map->rasterization(c.levelOfDetail).dimensions());
    hf.resize(c.heightfieldSizes());
    sum.resize(hf.sizes());

    // Start from empty
    hf = 0.0f;
    sum = 0.0f;

    // Splat each gene into the augmented heightfield
    for (int i = 0; i < c.size(0); ++i)
        for (int j = 0; j < c.size(1); ++j)
            renderGene(hf, sum, c.gene(i, j));
//    renderGene(hf, sum, c.gene(10, 10));
//    std::cerr << "Mean gradient is " << gradient(c.gene(10, 10)) << std::endl;
//    std::cerr << "ElevationRange around target is " << range(c.gene(10, 10)) << std::endl;

    // XXX HACK
    scalar_t m_hf = mean(hf),
             m_s = mean(sum),
             mn = m_hf / m_s;

    // Divide out the alpha channel to produce a blended heightfield
    hf /= sum;

    for (int i = 0; i < hf.size(0); ++i)
        for (int j = 0; j < hf.size(1); ++j)
            if (sum(i, j) == scalar_t(0))
                hf(i, j) = mn;
}

void terrainosaurus::renderGene(Heightfield & hf,
                                Heightfield & sum,
                                const TerrainChromosome::Gene & g) {
    // Select the source region and apply the gene's transformation to it
    Dimension size(g.mask->sizes());
    Pixel stS = g.sourceCoordinates - size / 2,
          edS = stS + size,
          stT = g.targetCoordinates - size / 2,
          edT = stT + size;

    // Add the transformed, masked pixels to the HF and the mask itself to sum
    select(hf, stT, edT)  += *(g.mask) * select(
                                            linearMap(
                                                rotate(g.sourceSample->elevation(g.levelOfDetail), g.rotation),
                                                g.scale, g.offset
                                            ),
                                            stS, edS);
    select(sum, stT, edT) += *(g.mask);
#if 0
    cerr << "ElevationRange of mask: " << range(*g.mask) << endl;
    cerr << "RMS of mask: " << rms(*g.mask) << endl;
    cerr << "ElevationRange of gene: " << range(*(g.mask) * select(g.sourceSample->heightfield(g.levelOfDetail), stS, edS)) << endl;
    cerr << "ElevationRange of output: " << range(select(hf, stT, edT)) << endl;
    cerr << "ElevationRange of sum: " << range(select(sum, stT, edT)) << endl;
    cerr << "Rendering gene from " << g.sourceCoordinates << " to " << g.targetCoordinates << endl;
#endif
}


// This function determines how "good" a chromosome is
scalar_t terrainosaurus::evaluateFitness(const TerrainChromosome & c) {
    // First, turn the TerrainChromosome back into a heightfield
    Heightfield hf;
    renderChromosome(hf, c);

//    Heightfield & orig = const_cast<Heightfield &>(c.gene(0,0).sourceSample->elevation(c.levelOfDetail()));
    const Heightfield & orig = c.gene(0,0).sourceSample->elevation(c.levelOfDetail());

    // Calculate the fitness of each individual terrain region
    // TODO: per-region fitness

    // Calculate the fitness of each individual terrain seam
    // TODO: per-seam fitness

    // Calculate global fitness estimates
    // TODO: global fitness

    // Determine the aggregate fitness estimate
    // TODO: aggregate fitness

    float fitness = 1.0f - std::log10(1.0f + rms(orig - hf)) / 5.0f;
    std::cerr << "evaluateFitness(" << hf.sizes() << "): " << fitness << endl;
    if (fitness < 0.0f) {
        fitness = 0.0f;
        std::cerr << "    returning 0.0f\n";
    }
    return fitness;
}


// This function determines how good a match two genes are
scalar_t terrainosaurus::evaluateCompatibility(const TerrainChromosome & c,
                                               IndexType i, IndexType j,
                                               const TerrainChromosome::Gene & g) {
    IndexType lod = c.levelOfDetail();
    Vector2D gGrad = gradientMean(g),
             cGrad = gradientMean(c, i, j);
    Vector2D gSRange = slopeRange(g),
             cSRange = slopeRange(c, i, j);
    scalar_t diffMag   = abs(magnitude(gGrad) - magnitude(cGrad)),
             diffAngle = angle(gGrad, cGrad),
             diffMean  = abs(elevationMean(g) - elevationMean(c, i, j));
    Vector2D diffRange = abs(elevationRange(g) - elevationRange(c, i, j));
    scalar_t diffSlope = ( gSRange[0] < cSRange[0]
                            ? cSRange[0] - gSRange[0] : scalar_t(0) )
                       + ( gSRange[1] > cSRange[1]
                            ? gSRange[1] - cSRange[1] : scalar_t(0) );
    scalar_t maxMag = magnitude(c.pattern()->globalGradientMean(lod)) * 20,
             maxAngle = PI<scalar_t>(),
             maxElevation = c.pattern()->globalElevationRange(lod)[1],
             slopeRange = cSRange[1] - cSRange[0];
    scalar_t compatibility =  scalar_t(1) - (diffMag / maxMag
                                           + diffAngle / maxAngle
                                           + diffMean / maxElevation
                                           + diffRange[0] / maxElevation
                                           + diffRange[1] / maxElevation,
                                           + diffSlope / slopeRange) / 6;
#if 0
    std::cerr << "Differences:\n"
              << "\tgradient magnitude: " << diffMag << "\n"
              << "\tgradient angle:     " << diffAngle << "\n"
              << "\tmean elevation:     " << diffMean << "\n"
              << "\televation range:    " << diffRange << "\n"
              << "\tslope range:        " << diffSlope << "\n";
    std::cerr << "Reference maxima:\n"
              << "\tgradient magnitude: " << maxMag << "\n"
              << "\tgradient angle:     " << maxAngle << "\n"
              << "\tslope range:        " << slopeRange << "\n"
              << "\televation:          " << maxElevation << "\n";
    std::cerr << "Aggregate compatibility: " << compatibility << endl;
#endif
    return compatibility;
}


// This mean elevation across a slot (i, j) in a Chromosome
scalar_t terrainosaurus::elevationMean(const TerrainChromosome & c,
                                       IndexType i, IndexType j) {
    return c.pattern()->localElevationMean(c.levelOfDetail())(c.gene(i, j).targetCoordinates);
}

// This mean gradient across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::gradientMean(const TerrainChromosome & c,
                                              IndexType i, IndexType j) {
    return c.pattern()->localGradientMean(c.levelOfDetail())(c.gene(i, j).targetCoordinates);
}

// This elevation range (min, max) across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::elevationRange(const TerrainChromosome & c,
                                                IndexType i, IndexType j) {
    return c.pattern()->localElevationRange(c.levelOfDetail())(c.gene(i, j).targetCoordinates);
}

// This slope range (min, max) across a slot (i, j) in a Chromosome
const Vector2D & terrainosaurus::slopeRange(const TerrainChromosome & c,
                                            IndexType i, IndexType j) {
    return c.pattern()->localSlopeRange(c.levelOfDetail())(c.gene(i, j).targetCoordinates);
}


// The mean elevation across the gene (including transformations)
scalar_t terrainosaurus::elevationMean(const TerrainChromosome::Gene & g) {
    scalar_t result = g.sourceSample->localElevationMean(g.levelOfDetail)(g.sourceCoordinates);
    return result * g.scale + g.offset;
}

// The mean gradient across the gene (including transformations)
Vector2D terrainosaurus::gradientMean(const TerrainChromosome::Gene & g) {
    Vector2D result = g.sourceSample->localGradientMean(g.levelOfDetail)(g.sourceCoordinates);
    return math::rotate(result, g.rotation) * g.scale; 
}

// The elevation range [min, max] across the gene (including transformations)
Vector2D terrainosaurus::elevationRange(const TerrainChromosome::Gene & g) {
    Vector2D result = g.sourceSample->localElevationRange(g.levelOfDetail)(g.sourceCoordinates);
    return result * g.scale + Vector2D(g.offset);
}

// The slope range [min, max] across the gene (including transformations)
Vector2D terrainosaurus::slopeRange(const TerrainChromosome::Gene & g) {
    Vector2D result = g.sourceSample->localSlopeRange(g.levelOfDetail)(g.sourceCoordinates);
    return result * g.scale;
}
