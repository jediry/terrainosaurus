#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace GA
{	
	struct Point2D
	{
		float x, y;
	};
	struct Point
	{
		int x,y;
		int marked;	
		Point *prev, *upper, *lower, *mid;
	};
	struct Boundary
	{
		int upper, lower;  
	};
	class Pyramid
	{
		public:
			Point** ConnectPoints(Point* prevpt, Point* curpt);
			void DebugRandomWalk(Point* origin);
			Point* RandomWalk(Point* origin);
			int length;
		private:
			void CreatePydTree(Point* origin, Point* dest, int length1);
			void CreateRevPydTree(Point* origin, Point* dest, int length1, int length2);
			void CreateEvenTree(Point* origin, Point* dest, int length1);
			int GetLessThan(int number)
			{
				return rand() % number;
			};
			
	};
	Point* Pyramid::RandomWalk(Point* origin)
	{
		Point* line = new Point[length];
		Point* curpt = origin;
		int value, curroffset = 0;
		//printf(" Coord: x %d y %d \n", curpt->x, curpt->y);
		line[curroffset].x = curpt->x; line[curroffset++].y = curpt->y;
		bool flat;
		while(curpt->lower != NULL || curpt->mid != NULL || curpt->upper != NULL)
		{
			flat = true;
			while(flat)
			{
				value = GetLessThan(3);
				if(value == 0)
				{
					if(curpt->lower != NULL)
					{
						flat = false;
						curpt = curpt->lower;
					}
					else
						flat = true;
				}
				else if(value == 1)
				{
					if(curpt->mid != NULL)
					{
						flat = false;
						curpt = curpt->mid;
					}
					else
						flat = true;
				}
				else
				{
					if(curpt->upper != NULL)
					{
						flat = false;
						curpt = curpt->upper;
					}
					else
						flat = true;
				}
			}
			line[curroffset].x = curpt->x; line[curroffset++].y = curpt->y;			
			//printf(" Coord: x %d y %d \n", curpt->x, curpt->y);
		}
		return line;
	}
	void Pyramid::DebugRandomWalk(Point* origin)
	{
		Point* curpt = origin;
		int value;
		printf(" Coord: x %d y %d \n", curpt->x, curpt->y);
		bool flat;
		while(curpt->lower != NULL || curpt->mid != NULL || curpt->upper != NULL)
		{
			flat = true;
			while(flat)
			{
				value = GetLessThan(3);
				if(value == 0)
				{
					if(curpt->lower != NULL)
					{
						flat = false;
						curpt = curpt->lower;
					}
					else
						flat = true;
				}
				else if(value == 1)
				{
					if(curpt->mid != NULL)
					{
						flat = false;
						curpt = curpt->mid;
					}
					else
						flat = true;
				}
				else
				{
					if(curpt->upper != NULL)
					{
						flat = false;
						curpt = curpt->upper;
					}
					else
						flat = true;
				}
			}
			printf(" Coord: x %d y %d \n", curpt->x, curpt->y);
		}
	}
	Point** Pyramid::ConnectPoints(Point* prevpt, Point* curpt)
	{
		int i,j;
		// make them the same
		curpt->y = prevpt->y;
		// extra one for the two squares representing the end squares			
		int firstDimension = curpt->x - prevpt->x + 1;
		int secondDimension = (int)(firstDimension / 2.0f); 
	  length = firstDimension;
		// round down occurs in secondDimension, so we'll need to check that:
		// if value of firstDimension is even, the total length of the 2nd dimension is 2(X-1)+1;
		// however if value of firstDimension is odd, the total length of the 2nd dimension is 2(x)+1;
		
		// the & takes care of odd or even
		int len, remains, k,l;
		int number = 2 * (secondDimension - (firstDimension & 0x01)) + 1;			
		Point** data = new Point*[firstDimension];	
		int *length = new int[firstDimension];
		for(i = 0, j = firstDimension-1; i <= j; i++,j--)
		{
			len = 2*i+1;
			data[i] = new Point[len];
			data[j] = new Point[len];
			length[i] = len; length[j] = len;
			// initialize the rest of the points
			remains = (int)(len/2.0f);
			// inits top half
			for(k=0; k <remains; k++)
			{
				data[i][k].marked = 0; data[i][k].x = prevpt->x + i; data[i][k].y = curpt->y + remains - k;
				data[i][k].upper = NULL; data[i][k].mid = NULL;	data[i][k].lower= NULL;
				data[j][k].marked = 0; data[j][k].x = curpt->x - i;	data[j][k].y = curpt->y + remains - k;
				data[j][k].upper = NULL; data[j][k].mid = NULL; data[j][k].lower= NULL;
			}
			// the middle point
			data[i][k].marked = 0; data[i][k].x = prevpt->x + i; data[i][k].y = curpt->y;
			data[i][k].upper = NULL; data[i][k].mid = NULL;	data[i][k].lower= NULL;
			data[j][k].marked = 0; data[j][k].x = curpt->x - i;	data[j][k].y = curpt->y;
			data[j][k].upper = NULL; data[j][k].mid = NULL; data[j][k].lower= NULL;
			// the lower half
			l = k + remains;
			k++;
			for(;k <= l; k++)
			{
				data[i][k].marked = 0; data[i][k].x = prevpt->x + i; data[i][k].y = curpt->y -(k - curpt->y);
				data[i][k].upper = NULL; data[i][k].mid = NULL;	data[i][k].lower= NULL;
				data[j][k].marked = 0; data[j][k].x = curpt->x - i;	data[j][k].y = curpt->y - (k - curpt->y);
				data[j][k].upper = NULL; data[j][k].mid = NULL; data[j][k].lower= NULL;
			}
		}			
		// debug:
		/*
		for(i = 0; i < firstDimension; i++)
		{
			printf("Length at offset %d is %d and coord is %d %d\n",i, length[i],data[i][0].x,data[i][0].y);
		}
	  */
		// mark the first and end points	
		data[0][0].marked = 1;
		data[firstDimension-1][0].marked = 1;
		// use a tree like structure
		
		//first half with pyramidal structure

		// if odd, there's only one longest array of points
		if((firstDimension & 0x01) == 1)
		{		
			for(i = 0; i < remains; i++)
			{
				CreatePydTree(data[i],data[i+1],length[i]);
			}
			// starting from the ony longest array of points and connect them
			// since i+1 is |i|-2, we'll have to reverse this process
			for(i = remains; i < firstDimension-1; i++)
			{
				CreateRevPydTree(data[i],data[i+1],length[i],length[i+1]);
			}
		}
		// if even, there are two longest array of points
		else
		{
			for(i = 0; i < remains; i++)
			{
				CreatePydTree(data[i],data[i+1],length[i]);
			}
			// the previous for loop stops at the point where the two longest array are adjacent
			// namely at i and i+1
			CreateEvenTree(data[i],data[i+1],length[i]);
			i++;
			for(;i < firstDimension-1; i++)
			{
				CreateRevPydTree(data[i],data[i+1],length[i],length[i+1]);
			}
		}
		return data;
		// when we reached firstDimension-2, we'll just call the connectpoint functio again to connect to end
		// point
	}
	void Pyramid::CreateEvenTree(Point* origin, Point* dest, int length1)
	{
		for(int i = 0; i < length1; i++)
		{
			origin[i].mid = &dest[i];
			if(i-1 >= 0)
				origin[i].upper = &dest[i-1];		
			if(i+1 < length1)
				origin[i].lower = &dest[i+1];
		}
	}
	// the origin points to dest which is |origin| -2 in length
	void Pyramid::CreateRevPydTree(Point* origin, Point* dest, int length1, int length2)
	{
		for(int i = 0; i < length1; i++)
		{
			if(i-2 >= 0)
				origin[i].upper = &dest[i-2];
			if(i-1 >= 1 && i-1 < length2)
				origin[i].mid = &dest[i-1];
			if(i < length2)
				origin[i].lower = &dest[i];		
		}
	}
	// the origin points to dest which is |origin| + 2 in length
	void Pyramid::CreatePydTree(Point* origin, Point* dest, int length1)
	{	
		// the offset of 0 points to elements 0,1,2, 1 points to elements 1,2,3
		for(int i = 0; i < length1; i++)
		{
			origin[i].upper = &dest[i];
			origin[i].mid = &dest[i+1];
			origin[i].lower = &dest[i+2];
		}
	}
}