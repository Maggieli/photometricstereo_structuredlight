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


	ImageGroup stereo_image(directory, filename);
	stereo_image.remove_noise();


	int width, height;
	width = stereo_image.width_;
	height = stereo_image.height_;

	Mat surfacenormal(height, width, CV_32FC3, Scalar::all(0.0));
	Mat gradient(height, width, CV_32FC3, Scalar::all(0.0));
	Mat depth (height, width, CV_32FC1, Scalar::all(0.0) );

	if(CalculateNormals(stereo_image, surfacenormal) )
	{
		cout<<"Compute normals successfully!"<<endl;
	}
	if(CalculateGradients(stereo_image, surfacenormal, gradient) )
	{
		cout<<"Compute gradient successfully!"<<endl;
	}
	if(CalculateDepths(stereo_image, gradient, depth) )
	{
		cout<<"Compute depth successfully!"<<endl;
	}
	ofstream file;
	file.open( ( filename + "sorr.ply" ).c_str() );
	file << "ply" << endl;
	file << "format ascii 1.0" << endl;
	file << "comment made by Min" << endl;
	file << "comment this file is a "<< endl;
	file << "element vertex " << height*width<< endl;
	file << "property float32 x" << endl;
	file << "property float32 y" << endl;
	file << "property float32 z" << endl;
	file << "property uint8 red " << endl;
	file << "property uint8 green" << endl;
	file << "property uint8 blue" << endl;
	file << "end_header" << endl;
	for (int i = 0; i < height; i++)
   {
		for (int j = 0; j <width; j++)
	   {

			file <<i<<" "<< j <<" "<<depth.at < float >(i, j)<<" "
				<<stereo_image.colorimagesmat_[0].at<Vec3f>(i, j)[2]<<" "
				<< stereo_image.colorimagesmat_[0].at<Vec3f>(i, j)[1]<<" "
				<< stereo_image.colorimagesmat_[0].at<Vec3f>(i, j)[0]<<endl;
	   }
   }
	file.close();

}




