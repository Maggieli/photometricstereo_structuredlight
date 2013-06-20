#ifndef PHOTOMETRIC_STEREO_H
#define PHOTOMETRIC_STEREO_H

#include "opencv2/opencv.hpp"
#include"opencv2/highgui//highgui.hpp"
#include<vector>
#include<iostream>
#include< string>
using namespace cv;
using namespace std;



#define MASK_BG 0

class ImageGroup{
public:
	ImageGroup()
	{
	}

	ImageGroup(string dataset, string imagesname);

	~ImageGroup()
	{
	}

	int width_;
	int height_;
	Mat maskimage_;
	string imagesname_;
	int numlights_;
	Mat lights_;
	vector<Mat>colorimagesmat_;
	vector<Mat>grayimagesmat_;


	void remove_noise();
	
	

};

int CalculateNormals(const ImageGroup& ig, Mat& normals);
int CalculateGradients(const ImageGroup& ig, const Mat& normals, Mat& gradients, float min_angle_rad = 0.001f);
int CalculateDepths(const ImageGroup& ig, const Mat& gradients, Mat& surface);


#endif