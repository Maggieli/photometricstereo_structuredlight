#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <fstream>
#include<string>
#include"geometry.h"

using namespace std;
using namespace cv;

void read_focal_length( string &directory, int& x, int& y){

    int line_no = 0;
	int target_line = 8;
    ifstream in((directory + "Calib_Result.dat").c_str());
    if(!in)
    {
        cerr<<"file open failed!"<<endl;
        exit(1);
    }
    string line;
    while(getline(in, line))
    {
        ++line_no;
        if(line_no == target_line)
        {
            istringstream stream(line);
			stream >> x >> y;
            break;
        }
    }   
}
void read_pointdata( string &directory, string& filename, Mat& low_geometry, int fx, int fy){
	int line_no = 0;
	int target_line = 4;
    ifstream in((directory + filename + ".wrl").c_str());
    if(!in)
    {
        cerr<<"file open failed!"<<endl;
        exit(1);
    }
    string line;
    while(getline(in, line))
    {
        ++line_no;
        if(line_no == target_line)
        {
           do
		   { 
			   istringstream stream(line);
			   double tempoint[3];
			   string endpoint;
			   stream >> endpoint;
			   if(endpoint == "]]}}}]}" )
			   {
				   break;
			   }
			   else
			   {
				   istringstream floatstream(line);
				   float tempoint[3];
				   int u, v;
				   string comma;
				   floatstream >> tempoint[0] >> tempoint[1] >> tempoint[2] >> comma;
				   u = (tempoint[0] * fx)/tempoint[2];
				   v = (tempoint[1] * fy)/tempoint[2];
				   low_geometry.at<float>(u, v) = tempoint[2];
			   }
		   }while(getline(in, line));
       }   
   }
}



