/*#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <fstream>
#include<string>



class Geometry{
public:
	Geometry()
	{
	}
	Geometry( string directory);
	~Geometry()
	{
	}
	Mat& CalculateSigma();
	void FusePoint();
	double SetThread() const;
private:
	int num_3dpoints_;
	double thread;
	int x_focalength;
	int y_focalength;
	

};

//void read_focal_length( string &directory, int& x, int& y);
//void read_pointdata( string &directory, string& filename, Mat& low_geometry, int fx, int fy);

#endif*/