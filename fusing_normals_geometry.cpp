#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <fstream>
#include<string>
#include"fusing_normals_geometry.h"

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
            //´¦Àí
            break;
        }
    }
     
 
}
