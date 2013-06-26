#ifndef PHOTOMETRIC_STEREO_H
#define PHOTOMETRIC_STEREO_H

#include "opencv2/opencv.hpp"
#include"opencv2/highgui//highgui.hpp"
#include<vector>
#include<iostream>
#include< string>


#define MASK_BG 0

class PhotometricStereo{
	friend class geometry;
public:
	PhotometricStereo()
	{
	}
	~PhotometricStereo()
	{
	}
	void RemoveNoise();
	void RemoveHighlight();
	void Removeshadow();
	//void MakeMask();
	Mat& CalculateLightDirection();
	Mat& CalculateNormals();
    Mat& CalculateGradients(float min_angle_rad = 0.001f);
    //int CalculateDepths();
private:
	std::string imagesname_;
	int width_;
	int height_;
	int numlights_;
	cv::Mat maskimage_;	
	cv::Mat lights_;
	//cv::Mat normal_;
	//cv::Mat gradient_;
	vector<cv::Mat>colorimagesmat_;
	vector<cv::Mat>grayimagesmat_;	

};


#endif