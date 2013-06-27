#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include < iostream>
#include <string>
#include <fstream>
#include <vector>
#include"photometric_stereo.h"

using namespace std;
using namespace cv;

void main(){

	//*** directory
	string directory = "D:\\work\\Projects\\Dataset";
	string filename;
	//cout << " Input your directory :";
	//cin >> directory;

	cout << " Input your filename :";
	cin >> filename;
	cin.clear();


	PhotometricStereo stereo_image(directory, filename);
	//stereo_image.remove_noise();


	int width, height;
	width = stereo_image.GetWidth();
	height = stereo_image.GetHeight();

	//Mat surfacenormal(height, width, CV_32FC3, Scalar::all(0.0));
	//Mat gradient(height, width, CV_32FC3, Scalar::all(0.0));
	//Mat depth (height, width, CV_32FC1, Scalar::all(0.0) );

	if(stereo_image.CalculateNormals() )
	{
		cout<<"Compute normals successfully!"<<endl;
	}
	if(stereo_image.CalculateGradients() )
	{
		cout<<"Compute gradient successfully!"<<endl;
	}
	if(stereo_image.CalculateDepth() )
	{
		cout<<"Compute depth successfully!"<<endl;
	}
}




