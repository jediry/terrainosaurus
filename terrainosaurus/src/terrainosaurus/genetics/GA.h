#pragma once
#include "Chromosome.h"
#include "Pyramid.h"
#include "RandomLine.h"
#include <limits.h>
#include <stdlib.h>
//#include <Pair>
#include <vector>
using namespace std;

namespace GA
{
	// progress:
	// operators are not implemented yet
	// generate random population isn't in palce yet
	// will have to link this class to the pyramid class
	class Population
	{
		public:
                    typedef std::vector<float> AngleList;
                    typedef std::pair<float,float> Range;
                    typedef std::vector<Range> RangeList;

			RangeList ranges;
			//float* MakeLine(Point fromPt, Point toPt,int i_cycles); /* old version that uses polygon shape to generate lines */
			float* MakeLine(Point fromPt, Point toPt,float* ubounds, float* lbounds,int i_cycles); /* new version */
                        AngleList MakeLine(const RangeList & envelope, float startAngle, float endAngle, int cycles);   // NEW, hacked version
			Population(int i_numberOfChromosomes, float f_smoothParam, float f_mutationRate, float f_crossoverRate, float f_selectionRatio, int i_randomseed);
			void InitializeChromosome(int i_offset, float* fA_genes, int i_length);
			//float* Evolve(int cycles);
			float* Evolve2(int cycles,RandomLine* randline);
			~Population(){delete[] chroms; /*delete pyramid;*/};
			float* MakeLine(Point fromPt, Point toPt,RangeList* bounds,int i_cycles);
		private:
			double GetAngle(Point* v_two, Point* v_three);
			double* ComputePopulationFitness(double& total);
			double* ComputePopulationCFitness();
			void AveragingOperator(int offset1, int offset2,int index);
			double GetFitness(int i_offset) { return chroms[i_offset].GetFitness(smoothParam);};
			double GetRand(double d_max);
			int FindOffset(double d_value,double* dA_cfitness);
			float* GetAngles(Point* fA_randomwalk, int i_length);
			double GetAngle(Point* P_one, Point* P_two, Point* P_three);
			void SinglePointCrossover(int offset1, int offset2,int index);
			//Chromosome* GenerateNewChromosome();
			Chromosome* GenerateNewChromosome2(RandomLine* randline);
			int numOfChromosomes;
			// selectionRatio determines the number of individuals being selected from the current population to form the new population
			// in each evolution cycle, 0 to 1
			float smoothParam, mutationRate, crossoverRate, selectionRatio;
			int numOfIndividuals;
			Chromosome* chroms;
			Pyramid* pyramid;
			Point fromPt, toPt;
			int seed;
	};
        Population::AngleList Population::MakeLine(const RangeList & envelope, float startAngle, float endAngle, int cycles) {
            Point start, end;
            start.x = 0;                    start.y = 0;
            end.x = envelope.size() - 1;    end.y = 0;
            float * up = new float[envelope.size()];
            float * down = new float[envelope.size()];
            for (IndexType i = 0; i < IndexType(envelope.size()); ++i) {
//                down[i] = envelope[i].second;
//                up[i] = envelope[i].first;
                up[i] = envelope[i].second;
                down[i] = envelope[i].first;
            }
            float * f = MakeLine(start, end, up, down, cycles);
//            float * f = MakeLine(start, end, const_cast<RangeList*>(&envelope), cycles);
            AngleList angles(envelope.size());
            for (int i = 0; i < int(angles.size()); ++i)
                angles[i] = f[i];
            delete f;
            return angles;
        }
	/* new version that doesn't use polygon to generate the line */
	float* Population::MakeLine(Point fromPt, Point toPt,RangeList* bounds,int i_cycles)
	{
		vector <Range>::iterator iter;
		float* angles;
		int segments = (int)(abs(toPt.x - fromPt.x)+0.5);
		Range* rpt; float* ubounds; float* lbounds;
		GA::Point2D prevpt, curpt;
		prevpt.x = (float)fromPt.x; prevpt.y = (float)fromPt.y;
		curpt.x = (float)toPt.x; curpt.y = (float)toPt.y;
		// generate the ubounds and lbounds arrays
		ubounds = new float[segments]; lbounds = new float[segments];
		iter = bounds->begin();
		for(int i = 0; i < segments; i++)
		{
			rpt = &(*iter);
			lbounds[i] = rpt->first;
			ubounds[i] = rpt->second;
			iter++;
		}
		GA::RandomLine randline(prevpt,curpt,ubounds,lbounds,smoothParam,segments);
		// generate the initial population
		for(int i = 0; i < numOfChromosomes; i++)
		{
			angles = randline.GetAnotherLine();
			InitializeChromosome(i,angles,toPt.x - fromPt.x);
			delete[] angles;
		}
		// gets into evolution cycles
		float* line=Evolve2(i_cycles,&randline);
		delete[] ubounds; delete[] lbounds;
		// return the angles of the lines
		return line;

	}

	float* Population::MakeLine(Point fromPt, Point toPt,float* ubounds, float* lbounds,int i_cycles)
	{
		float* angles;
		GA::Point2D prevpt, curpt;
		prevpt.x = (float)fromPt.x; prevpt.y = (float)fromPt.y;
		curpt.x = (float)toPt.x; curpt.y = (float)toPt.y;
		GA::RandomLine randline(prevpt,curpt,ubounds,lbounds,smoothParam,(int)(abs(toPt.x - fromPt.x)+0.5));
		// generate the initial population
		for(int i = 0; i < numOfChromosomes; i++)
		{
			angles = randline.GetAnotherLine();
			InitializeChromosome(i,angles,toPt.x - fromPt.x);
			delete[] angles;
		}
		// gets into evolution cycles
		float* line=Evolve2(i_cycles,&randline);

		// return the angles of the lines
		return line;

	}

	/*
	float* Population::MakeLine(Point in_fromPt, Point in_toPt, int i_cycles)
	{
		int length;
		Point *randomwalk, **data;
		float* angles;
		// setups the pyramidal structure
		pyramid = new GA::Pyramid();
		fromPt=in_fromPt; toPt = in_toPt;
		data = pyramid->ConnectPoints(&fromPt,&toPt);
		length = pyramid->length;
		// generate the initial population
		for(int i = 0; i < numOfChromosomes; i++)
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
	}*/
	float* Population::GetAngles(Point* fA_randomwalk, int i_length)
	{
		int arraylen = i_length;
		float* angles = new float[i_length];
		// dotproduct
		// cos(theta) = Dotproduct(v1,v2) / (length(v1)*length(v2))
		// all in doubles
		Point xaxis, origin, temp;
		xaxis.x = 1; xaxis.y = 0;
		origin.x = 0; origin.y = 0;
	  temp.x = fA_randomwalk[1].x - fA_randomwalk[0].x;
		temp.y = fA_randomwalk[1].y - fA_randomwalk[0].y;
		angles[0] = (float)GetAngle(&xaxis,&temp);
		//printf("point1 : %d %d point2: %d %d angle: %f\n",fA_randomwalk[0].x,fA_randomwalk[0].y,fA_randomwalk[1].x,fA_randomwalk[1].y,angles[0]);

		for(int i = 1; i < arraylen-1; i++)
		{
			temp.x = fA_randomwalk[i+1].x - fA_randomwalk[i].x;
			temp.y = fA_randomwalk[i+1].y - fA_randomwalk[i].y;
			angles[i] = (float)GetAngle(&xaxis,&temp);
			//printf("point1 : %d %d point2: %d %d angle: %f\n",fA_randomwalk[i].x,fA_randomwalk[i].y,fA_randomwalk[i+1].x,fA_randomwalk[i+1].y,angles[i]);
		}
		// the angle of the last line segment with an imaginary x axis, > PI/2
		//angles[i] = (float)GetAngle(&xaxis,&temp);
		return angles;
	}
	double Population::GetAngle(Point* vec1, Point* vec2)
	{
		double angle;
		angle = acos((vec1->x*vec2->x+vec1->y*vec2->y) / (sqrt((float)vec1->x*vec1->x+vec1->y*vec1->y) * sqrt((float)vec2->x*vec2->x+vec2->y*vec2->y)));
		if(vec1->y > vec2->y)
			angle *= -1.0;
		return angle;
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
		result = acos((vec1.x*vec2.x+vec1.y*vec2.y) / (sqrt((float)vec1.x*vec1.x+vec1.y*vec1.y) * sqrt((float)vec2.x*vec2.x+vec2.y*vec2.y)));
		// direction is being calculated here
		// P_one is ontop of point 3, so it's in positive direction
		if(P_one->y < P_three->y)
			result *= -1.0;
		return result;
	}
	Population::Population(int i_numberOfChromosomes, float f_smoothParam, float f_mutationRate, float f_crossoverRate, float f_selectionRatio, int i_randomseed)
	{
		seed=i_randomseed;
		srand(seed);
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
	float* Population::Evolve2(int i_cycles,RandomLine* randline)
	{
		int i = 0,j, chromlength, ii,jj;
		double *cfitness;
		int offset, curOffset = 0;
		int crossoveroffset1, crossoveroffset2,crossoverindex;
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
				Chromosome* tempchrom = GenerateNewChromosome2(randline);
				newpop[curOffset++].Copy(tempchrom);
				delete tempchrom;
			}
			delete[] cfitness;
			delete[] chroms;
			chroms = newpop;
			newpop = NULL;
			////******* apply GA operators on the individuals
			// chooses two individuals and perform single point crossover

			chromlength = chroms[0].length;
			if(GetRand(1.0) < crossoverRate)
			{
				crossoveroffset1 = rand() % numOfChromosomes;
				crossoveroffset2 = rand() % numOfChromosomes;
				crossoverindex = rand() % chromlength;
				if(crossoveroffset1 != crossoveroffset2)
					SinglePointCrossover(crossoveroffset1, crossoveroffset2,crossoverindex);
			}
			// averaging operator
			if(GetRand(1.0) < crossoverRate)
			{

				crossoveroffset1 = rand() % numOfChromosomes;
				crossoveroffset2 = rand() % numOfChromosomes;
				crossoverindex = rand() % chromlength;
				if(crossoveroffset1 != crossoveroffset2)
					AveragingOperator(crossoveroffset1, crossoveroffset2,crossoverindex);

			}
			// mutation section
			for(ii = 0; ii < numOfChromosomes; ii++)
			{
				for(jj = 0; jj < chromlength; jj++)
				{
					if(GetRand(1.0) < mutationRate)
					{
						chroms[ii].MutateGene(jj);
					}
				}
			}

			////^^^^^^^ End GA operators section
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
	Chromosome* Population::GenerateNewChromosome2(RandomLine* randline)
	{
		Chromosome* chrom = new Chromosome();
		float* angles =randline->GetAnotherLine();
		chrom->Initialize(angles,randline->segments);
		delete[] angles;
		return chrom;
	}
	/*
	float* Population::Evolve(int i_cycles)
	{
		int i = 0,j, chromlength, ii,jj;
		double *cfitness;
		int offset, curOffset = 0;
		int crossoveroffset1, crossoveroffset2,crossoverindex;
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
				Chromosome* tempchrom = GenerateNewChromosome();
				newpop[curOffset++].Copy(tempchrom);
				delete tempchrom;
			}
			delete[] cfitness;
			delete[] chroms;
			chroms = newpop;
			newpop = NULL;
			////******* apply GA operators on the individuals
			// chooses two individuals and perform single point crossover
			chromlength = chroms[0].length;
			if(GetRand(1.0) < crossoverRate)
			{
				crossoveroffset1 = rand() % numOfChromosomes;
				crossoveroffset2 = rand() % numOfChromosomes;
				crossoverindex = rand() % chromlength;
				if(crossoveroffset1 != crossoveroffset2)
					SinglePointCrossover(crossoveroffset1, crossoveroffset2,crossoverindex);
			}
			// averaging operator
			if(GetRand(1.0) < crossoverRate)
			{
				crossoveroffset1 = rand() % numOfChromosomes;
				crossoveroffset2 = rand() % numOfChromosomes;
				crossoverindex = rand() % chromlength;
				if(crossoveroffset1 != crossoveroffset2)
					AveragingOperator(crossoveroffset1, crossoveroffset2,crossoverindex);
			}
			// mutation section
			for(ii = 0; ii < numOfChromosomes; ii++)
			{
				for(jj = 0; jj < chromlength; jj++)
				{
					if(GetRand(1.0) < mutationRate)
					{
						chroms[ii].MutateGene(jj);
					}
				}
			}
			////^^^^^^^ End GA operators section
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
		Chromosome* chrom = new Chromosome();
	  Point *randomwalk, **data;
		data = pyramid->ConnectPoints(&fromPt,&toPt);
		int length = pyramid->length;;
		randomwalk = pyramid->RandomWalk(&data[0][0]);
		randomwalk[length-1].x = toPt.x;
		randomwalk[length-1].y = toPt.y;
		float* angles = GetAngles(randomwalk,length);
		chrom->Initialize(angles,length-2);
		delete[] angles;
		delete[] randomwalk;
		return chrom;
	}
	*/
	void Population::SinglePointCrossover(int offset1, int offset2,int index)
	{
		int length = chroms[offset1].length;
		float tempf;
		for(int i = index; i < length; i++)
		{
			tempf = chroms[offset1].genes[i];
			chroms[offset1].genes[i] = chroms[offset2].genes[i];
			chroms[offset2].genes[i] = tempf;
		}
	}
	// this function influences the chromosome in offset1
	void Population::AveragingOperator(int offset1, int offset2,int index)
	{
		int length = chroms[offset1].length;
		float tempf;
		for(int i = index; i < length; i++)
		{
			tempf = chroms[offset1].genes[i] + chroms[offset2].genes[i] / 2;
			chroms[offset1].genes[i] = tempf;
		}
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
		return numOfChromosomes-1;
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
