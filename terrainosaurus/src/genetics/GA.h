#pragma once
#include "Chromosome.h"
#include "Pyramid.h"
#include <limits.h>
#include <stdlib.h>

namespace GA
{
	// progress:
	// operators are not implemented yet
	// generate random population isn't in palce yet
	// will have to link this class to the pyramid class
	class Population
	{
		public:
			float* MakeLine(Point fromPt, Point toPt,int i_cycles);
			Population(int i_numberOfChromosomes, float f_smoothParam, float f_mutationRate, float f_crossoverRate, float f_selectionRatio);
			void InitializeChromosome(int i_offset, float* fA_genes, int i_length);			
			float* Evolve(int cycles);
			~Population(){delete[] chroms; delete pyramid;};
		private:
			double* ComputePopulationFitness(double& total);
			double* ComputePopulationCFitness();
			double GetFitness(int i_offset) { return chroms[i_offset].GetFitness(smoothParam);};
			double GetRand(double d_max);
			int FindOffset(double d_value,double* dA_cfitness);
			float* GetAngles(Point* fA_randomwalk, int i_length);
			double GetAngle(Point* P_one, Point* P_two, Point* P_three);
			Chromosome* GenerateNewChromosome();
			int numOfChromosomes;
			// selectionRatio determines the number of individuals being selected from the current population to form the new population
			// in each evolution cycle, 0 to 1
			float smoothParam, mutationRate, crossoverRate, selectionRatio; 
			int numOfIndividuals;
			Chromosome* chroms;	
			Pyramid* pyramid;
	};
	float* Population::MakeLine(Point fromPt, Point toPt, int i_cycles)
	{
		int length;
		Point *randomwalk, **data;					
		float* angles;
		// setups the pyramidal structure
		pyramid = new GA::Pyramid();
		data = pyramid->ConnectPoints(&fromPt,&toPt);		
		length = pyramid->length;
		// generate the initial population
		for(int i = 0; i < numOfIndividuals; i++)
		{
			randomwalk = pyramid->RandomWalk(&data[0][0]);			
			randomwalk[length-1].x = toPt.x;
			randomwalk[length-1].y = toPt.y;
			angles = GetAngles(randomwalk,length);			
			InitializeChromosome(i,angles,length-2);
			delete[] angles;
			delete[] randomwalk;
		}		
		// gets into evolution cycles
		float* line=Evolve(i_cycles);
		// cleanup
		for(int i =0; i < length; i++)
		{
			delete[] data[i];
		}
		delete[] data;
		// return the angles of the lines
		return line;
	}
	float* Population::GetAngles(Point* fA_randomwalk, int i_length)
	{
		int arraylen = i_length;
		float* angles = new float[i_length];
		// dotproduct
		// cos(theta) = Dotproduct(v1,v2) / (length(v1)*length(v2))
		// all in doubles		
		Point xaxis, origin;
		xaxis.x = 1; xaxis.y = 0;
		origin.x = 0; origin.y = 0;
		angles[0] = (float)GetAngle(&xaxis,&origin,&fA_randomwalk[0]);
                int i;
		for(i = 1; i < arraylen-1; i++)
		{
			angles[i] = (float)GetAngle(&fA_randomwalk[i-1],&fA_randomwalk[i],&fA_randomwalk[i+1]);
		}
		// the angle of the last line segment with an imaginary x axis
		xaxis.x = fA_randomwalk[i_length-1].x + 1;
		xaxis.y = fA_randomwalk[i_length-1].y;
		angles[i] = (float)GetAngle(&xaxis,&fA_randomwalk[i],&fA_randomwalk[i-1]);
		return angles;
	}
	double Population::GetAngle(Point* P_one, Point* P_two, Point* P_three)
	{
		Point vec1,vec2;
		double result;
		// make two vectors
		vec1.x = P_two->x - P_one->x;
		vec1.y = P_two->y - P_one->y;
		vec2.x = P_three->x - P_two->x;
		vec2.y = P_three->y - P_two->y;
		// angle is always 180-theta in degrees
		result = acos((vec1.x*vec2.x+vec1.y*vec2.y) / (sqrt((double)vec1.x*vec1.x+vec1.y*vec1.y) * sqrt((double)vec2.x*vec2.x+vec2.y*vec2.y)));
		// direction is being calculated here
		// P_one is ontop of point 3, so it's in positive direction
		if(P_one->y < P_three->y)
			result *= -1.0;
		return result;
	}
	Population::Population(int i_numberOfChromosomes, float f_smoothParam, float f_mutationRate, float f_crossoverRate, float f_selectionRatio)
	{		
		selectionRatio = f_selectionRatio;
		numOfChromosomes=i_numberOfChromosomes;
		// numOfindivduals to be chosen from current population to form the new pop
		numOfIndividuals = (int)(selectionRatio * numOfChromosomes + 0.5);
		chroms = new Chromosome[numOfChromosomes];
		smoothParam = f_smoothParam;
		mutationRate = f_mutationRate; crossoverRate = f_crossoverRate;
	}
	void Population::InitializeChromosome(int i_offset,float* fA_genes, int i_length)
	{
		chroms[i_offset].Initialize(fA_genes,i_length);
	}
	float* Population::Evolve(int i_cycles)
	{		
		int i = 0,j;
		double *cfitness;			
		int offset, curOffset = 0;
		// roulette wheel
		Chromosome* newpop = NULL;
		while(i< i_cycles)
		{					
			newpop = new Chromosome[numOfChromosomes];
			curOffset = 0;
			// construct a cumulative frequency function
			cfitness=ComputePopulationCFitness();
			// choose individuals from the current pop
			double randoffset;
			for(j=0; j < numOfIndividuals; j++)
			{
				randoffset = GetRand(cfitness[numOfChromosomes-1]);
				offset=FindOffset(randoffset,cfitness);
				// copies the genes from the old chromosome to the new pop
				newpop[curOffset++].Copy(&chroms[offset]);
			}
			// generate new individuals for the new pop
			for(j = numOfIndividuals; j < numOfChromosomes; j++)
			{
				newpop[curOffset++].Copy(GenerateNewChromosome());
			}
			// apply operators on the individuals
			delete[] cfitness;
			delete[] chroms;
			chroms = newpop;
			newpop = NULL;
			i++;
		}
		double maxfitness = 0.0;
		double currfitness;
		offset=0;
		for(i = 0; i < numOfChromosomes; i++)
		{
			currfitness = chroms[i].GetFitness(smoothParam);
			if(maxfitness < currfitness)
			{
				maxfitness = currfitness;
				offset = i;
			}
		}
		return chroms[offset].GetGenes();
	}
	Chromosome* Population::GenerateNewChromosome()
	{
		return NULL;
	}
	// Find the offset of an individual to form the new pop
	int Population::FindOffset(double d_value,double* dA_cfitness)
	{		
		// should use a modified binary search here... but....
		for(int i =0; i < numOfChromosomes; i++)			
		{
			if(d_value < dA_cfitness[i])
			{
				return i;
			}
		}		
		return numOfChromosomes;
	}
	double Population::GetRand(double d_max)
	{
		double value=(double)rand() / (double)RAND_MAX; 
		return value * d_max;
	}
	double* Population::ComputePopulationCFitness()
	{
		double totalfitness = 0.0;
		double* cfitness = new double[numOfChromosomes];
		// get total fitness of the population
		double *fitness=ComputePopulationFitness(totalfitness);			
		// cumulative frequency function
		cfitness[0] = fitness[0]/totalfitness;
		for(int i = 1; i < numOfChromosomes; i++)
		{			
			cfitness[i] = (fitness[i] / totalfitness) + cfitness[i-1];			
		}
		delete[] fitness;
		return cfitness;
	}
	double* Population::ComputePopulationFitness(double& total)
	{
		int j;
		double* result = new double[numOfChromosomes];
		for(j = 0; j < numOfChromosomes; j++)
		{
			result[j] = GetFitness(j);
			total += result[j];
		}
		return result;
	}
}
