#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define PI 3.14152
namespace GA
{
	///<Summary> The chromosome class is a collection of floating point numbers </Summary>
	///<Features> Fixed length chromosome </Features>	
	
	class Chromosome
	{
		public:
			Chromosome(){ genes = NULL; fitness = 0.0; length = 0;};
			void Initialize(float* fA_genes, int i_length);
			~Chromosome(){delete[] genes;};		
			void MutateGene(int i_offset);
			double GetRand(double d_max);
			double GetFitness(float f_UserSmoothnessParam)
			{
				if(fitness == 0.0) 
				{ 
					return EvaluateFitness(f_UserSmoothnessParam);
				} 
				return fitness;
			};						
			void PrintDebugString()
			{
				for(int i = 0; i < length ;i++)
				{
					printf("%f ",genes[i]);
				}
				printf("\n");
			}
			void Copy(Chromosome* chrom_from);
			float* GetGenes();
			int length;
			double perfectsmoothline;
			float* genes;
		private:
			float AbsFloat(float f_value) { if(f_value < 0.0){f_value *= -1;} return f_value;};
			double EvaluateFitness(float f_UserSmoothnessParam);			
			double fitness;						
	};
	float* Chromosome::GetGenes()
	{
		float* outgenes= new float[length];
		for(int i = 0; i < length; i++)
		{
			outgenes[i] = genes[i];
		}
		return outgenes;
	}
	void Chromosome::MutateGene(int i_offset)
	{
		float PIdiv2 = (float)(PI/2.0);
		float original = genes[i_offset];		
		float current = (float)(GetRand(PIdiv2/2.0) * pow(-1,rand()));
		float result = original + current;
		while(!(result < PIdiv2 && result >= -PIdiv2))
		{
			result = result / 2.0f;
		}
		genes[i_offset] = result;
	}	
	double Chromosome::GetRand(double d_max)
	{
		double value=(double)rand() / (double)RAND_MAX; 
		return value * d_max;
	}
	void Chromosome::Copy(Chromosome* chrom_from)
	{
		if(genes != NULL)
			delete[] genes;
		length = chrom_from->length;
		genes = new float[length];
		for(int i= 0; i < length; i++)
			genes[i] = chrom_from->genes[i];
		perfectsmoothline = chrom_from->perfectsmoothline;
	}
	// f_genes are a set of floating point numbers for radians
	// NOTE: since one can measure an angle from two directions eg: clockwise and counter clockwise
	//       we use +values to meansure counte-clockwise angle and negative values for clockwise angles
	//       this retricts genes to have values between -PI to +PI
	void Chromosome::Initialize(float* fA_genes, int i_length)
	{
		length = i_length;
		genes = new float[length];
		fitness = 0.0;
		perfectsmoothline = length * PI; // a perfectly smooth line is a straight line
		for(int i = 0; i < length;i++)
		{
			genes[i] = fA_genes[i];
		}
	}
	// Fitness calculation of the chorosome is done by finding angle difference between line i, i-1
	// with the formula (PI - |theta(i)|)/perfectsmoothline
	// the smoother the line is, the bigger the value.
	// return value: The closer a line is to 1, the smoother it is. Range of the value: 0 to 1
	double Chromosome::EvaluateFitness(float f_UserSmoothnessParam)
	{
		double totalsmoothness = perfectsmoothline;		
		double averagedsmoothness = 0.0;
		for(int i = 0; i < length; i++)
		{
			totalsmoothness -= (PI - AbsFloat(genes[i]));
		}
		averagedsmoothness = totalsmoothness / perfectsmoothline;
		if(averagedsmoothness >= 0.75)
		{
			fitness = f_UserSmoothnessParam * averagedsmoothness;
		}
		else
		{
			fitness = f_UserSmoothnessParam * 1.0/averagedsmoothness;
		}
		if(fitness < 0)
			return 0.0;		
		return fitness;
	}
}