#pragma once
//#define DEBUG
namespace GA
{
	class RandomLine
	{
		public:
			/* generates a line that has y <= ubounds, and -y >= lbounds, the smoothness parameter affects the angle
				between the lines -> returns a bunch of angles
			*/
			float* GetLine(GA::Point2D PT_frompt, GA::Point2D PT_topt, float* f_ubounds, float* f_lbounds, float f_smoothness, int i_segments);
			float* GetAnotherLine(float* f_ubounds, float* f_lbounds, float f_smoothness, int i_segments);
			float* GetAnotherLine();

			RandomLine();
			RandomLine(GA::Point2D PT_frompt, GA::Point2D PT_topt, float* f_ubounds, float* f_lbounds, float f_smoothness, int i_segments)
			{
				frompt = PT_frompt; topt=PT_topt;
				ubounds = f_ubounds; lbounds = f_lbounds; smoothness = f_smoothness; segments = i_segments;
			};
			float GetNextThetaValue(float range);
			int segments;
		private:
			Point2D RotateRWithOrigin(float radian)
			{
				Point2D newpt;
				// compute location
				newpt.x =(float)cos(radian)*1.0f - (float)sin(radian) * 0;
				newpt.y = (float)sin(radian)*1.0f + (float)cos(radian) * 0;
				return newpt;
			};
			Point2D RotateRWithOrigin(Point2D vec, float radian)
			{
				Point2D newpt;
				// compute location
				newpt.x =(float)cos(radian)*vec.x - (float)sin(radian) * vec.y;
				newpt.y = (float)sin(radian)*vec.x + (float)cos(radian) * vec.y;
				return newpt;
			};
			Point2D RotateR(Point2D pt1, float radian)
			{
				Point2D newpt;
				// compute location
				newpt.x =(float)cos(radian)*pt1.x - (float)sin(radian) * pt1.y;
				newpt.y = (float)sin(radian)*pt1.x + (float)cos(radian) * pt1.y;
				return newpt;
			}
			Point2D RotateRAtItsOwnPosition(Point2D pt1, Point2D pt2,float radian)
			{
				Point2D vec;
				// translate to origin
				vec.x = pt2.x -pt1.x; vec.y= pt2.y - pt1.y;
				vec=RotateRWithOrigin(vec,radian);
				// translate back to the actual position
				vec.x = pt2.x + pt1.x; vec.y= pt2.y + pt1.y;
				return vec;
			};
			float FindAngleOfTwoPointsWithX(Point2D pt1,Point2D pt2)
			{
				// acos [Dot[Begin,End]/Length[Begin] * Length[End]] -> angle in radian
				Point2D vec;
				vec.x = pt2.x -pt1.x; vec.y= pt2.y - pt1.y;

				if (vec.y < 0) {
					return (float)-acos( (1 * vec.x + 0 * vec.y) /
                        (1 * sqrt(vec.x * vec.x +
                                   vec.y * vec.y)));
				}
				else {
					return (float)acos( (1 * vec.x + 0 * vec.y) /
                       (1 * sqrt(vec.x * vec.x +
                                  vec.y * vec.y)));
				}
			};
			GA::Point2D frompt, topt;
			float* ubounds; float* lbounds; float smoothness;
	};
	float* RandomLine::GetAnotherLine(float* f_ubounds, float* f_lbounds, float f_smoothness, int i_segments)
	{
		ubounds = f_ubounds; lbounds = f_lbounds; smoothness = f_smoothness; segments = i_segments;
		return GetAnotherLine();
	}
	float* RandomLine::GetAnotherLine()
	{
		//*** this section generates the initial line centered at 0,0
		Point2D curpt,temppt; // current x and y coordinates
		float* line = new float[segments];
		Point2D* points = new Point2D[segments+1];
		points[0].x = 0.0f; points[0].y = 0.0f;
		float theta;
		curpt.x = 0.0f; curpt.y = 0.0f;
		// generate lines with -45 <= theta <= 45 degree where positive direction is counter clockwise
		for(int i = 0; i < segments; i++)
		{
                        //cerr << "Bounds are " << lbounds[i] << ".." << ubounds[i] << endl;
			do{
				// we'll create a linear relationship to smoothness parameter : such that anything > 1/2 PI * smoothness is rejected
				theta=GetNextThetaValue((float)(0.25f *  PI * smoothness));
				line[i] = theta;
				// update location, temppt is now the position with respect to origin
				temppt = RotateRWithOrigin(theta);
				// translate to the current location
				temppt.x = curpt.x + temppt.x;
				temppt.y = curpt.y + temppt.y;
//                                cerr << "Trying " << temppt.y << endl;
			}while(temppt.y > ubounds[i] || temppt.y < lbounds[i]);
			curpt.x = temppt.x; curpt.y = temppt.y;
			points[i+1].x = curpt.x; points[i+1].y = curpt.y;
#ifdef DEBUG
			printf("x is %f y is %f, theta is %f\n",temppt.x, temppt.y, line[i]);
#endif
		}
		// ** now we'll have to rotate them to where the actual orientation and compute the actual angle
		// find the angle of the frompt and topt
		float rotangle = FindAngleOfTwoPointsWithX(frompt,topt);
#ifdef DEBUG
			printf("Rotation angle is %f \n",rotangle);
#endif
		// rotate all points by rotangle
		Point2D oldpos = points[0], tempos;
		for(int i = 1; i <= segments; i++)
		{
			tempos.x  = points[i].x; tempos.y = points[i].y;
			// we'l have to translate all points to origina, rotate by rotangle, and translate back to their position
		  // this is done by RotateRAtItsOwnPosition
			//points[i] = RotateRAtItsOwnPosition(oldpos,points[i],rotangle);
			points[i] = RotateR(points[i],rotangle);
			//points[i].x += oldpos.x; points[i].y += oldpos.y;
			oldpos = tempos;
		}
		// translate all points to fromtpt
		for(int i = 0; i < segments; i++)
		{
			points[i].x += frompt.x;
			points[i].y += frompt.y;
		}
		// compute all angles
		for(int i = 1; i <= segments; i++)
		{
			line[i-1]=FindAngleOfTwoPointsWithX(points[i-1],points[i]);
		}
#ifdef DEBUG
		for(int i = 0; i < segments; i++)
		{
			printf("after process theta x is %f y is %f, theta is %f\n",points[i].x, points[i].y, line[i]);
		}
#endif
		return line;
	}

	float* RandomLine::GetLine(GA::Point2D PT_frompt, GA::Point2D PT_topt, float* f_ubounds, float* f_lbounds, float f_smoothness, int i_segments)
	{
		//*** this section generates the initial line centered at 0,0
		//Point2D curpt,temppt; // current x and y coordinates
		frompt = PT_frompt; topt=PT_topt;
		ubounds = f_ubounds; lbounds = f_lbounds; smoothness = f_smoothness; segments = i_segments;
		/*
		float* line = new float[i_segments];
		Point2D* points = new Point2D[i_segments+1];
		points[0].x = 0.0f; points[0].y = 0.0f;
		float theta;
		curpt.x = 0.0f; curpt.y = 0.0f;
		// generate lines with -45 <= theta <= 45 degree where positive direction is counter clockwise
		for(int i = 0; i < segments; i++)
		{
			do{
				// we'll create a linear relationship to smoothness parameter : such that anything > 1/2 PI * smoothness is rejected
				theta=GetNextThetaValue((float)(0.25f *  PI * smoothness));
				line[i] = theta;
				// update location, temppt is now the position with respect to origin
				temppt = RotateRWithOrigin(theta);
				// translate to the current location
				temppt.x = curpt.x + temppt.x;
				temppt.y = curpt.y + temppt.y;

			}while(temppt.y > ubounds[i] || temppt.y < lbounds[i]);
			curpt.x = temppt.x; curpt.y = temppt.y;
			points[i+1].x = curpt.x; points[i+1].y = curpt.y;
#ifdef DEBUG
			printf("x is %f y is %f, theta is %f\n",temppt.x, temppt.y, line[i]);
#endif
		}
		// ** now we'll have to rotate them to where the actual orientation and compute the actual angle
		// find the angle of the frompt and topt
		float rotangle = FindAngleOfTwoPointsWithX(frompt,topt);
#ifdef DEBUG
			printf("Rotation angle is %f \n",rotangle);
#endif
		// rotate all points by rotangle
		Point2D oldpos = points[0], tempos;
		for(int i = 1; i <= segments; i++)
		{
			tempos.x  = points[i].x; tempos.y = points[i].y;
			// we'l have to translate all points to origina, rotate by rotangle, and translate back to their position
		  // this is done by RotateRAtItsOwnPosition
			//points[i] = RotateRAtItsOwnPosition(oldpos,points[i],rotangle);
			points[i] = RotateR(points[i],rotangle);
			//points[i].x += oldpos.x; points[i].y += oldpos.y;
			oldpos = tempos;
		}
		// translate all points to fromtpt
		for(int i = 0; i < segments; i++)
		{
			points[i].x += frompt.x;
			points[i].y += frompt.y;
		}
		// compute all angles
		for(int i = 1; i <= segments; i++)
		{
			line[i-1]=FindAngleOfTwoPointsWithX(points[i-1],points[i]);
		}
#ifdef DEBUG
		for(int i = 0; i < segments; i++)
		{
			printf("after process theta x is %f y is %f, theta is %f\n",points[i].x, points[i].y, line[i]);
		}
#endif
		*/
		return GetAnotherLine();
	}
	float RandomLine::GetNextThetaValue(float range)
	{
		float value;
//		do
//		{
//			// seed is already set in GA.h
//			value =(float)( (float)rand() / (float)RAND_MAX) * (float)pow(-1,rand());
//		}while(value > range || value < -range);
//                cerr << "Got it\n";
                value = range * ( (float)rand() / (float)RAND_MAX) * (float)pow(-1,rand());
		return value;
	}
}
