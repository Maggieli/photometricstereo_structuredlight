#ifndef PHOTOMETRIC_STEREO_H
#define PHOTOMETRIC_STEREO_H

#include "opencv2/opencv.hpp"
#include"opencv2/highgui//highgui.hpp"
#include<vector>
#include<iostream>
#include< string>

using namespace cv;

class PhotometricStereo{
	friend class geometry;
public:
	PhotometricStereo()
	{
	}
	PhotometricStereo(std::string dataset, std::string imagename);
	~PhotometricStereo()
	{
	}
	void RemoveNoise();
	void RemoveHighlight();
	void Removeshadow();
	//void MakeMask();
	//Mat& CalculateLightDirection();
	bool CalculateNormals( );
    bool CalculateGradients(float min_angle_rad = 0.001f);
	bool CalculateDepth();
    void SaveDepths();
		void SetWidth(int width)
	{
		width_ = width;
	}
	int GetWidth()
	{
		return width_;
	}
	void SetHeight(int height)
	{
		height_ = height;
	}
	int GetHeight()
	{
		return height_;
	}
	int GetNumlights()
	{
		return numlights_;
	}
private:
	std::string imagesname_;
	int width_;
	int height_;
	int numlights_;
	cv::Mat maskimage_;	
	Mat lights_;
	Mat normal_;
	Mat gradient_;
	Mat depth_;
	vector<Mat> colorimagesmat_;
	vector<Mat> grayimagesmat_;	

};


#endif