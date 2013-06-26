#ifndef IMAGE_H
#define IMAGE_H

#include "opencv2/opencv.hpp"
#include"opencv2/highgui//highgui.hpp"
#include <iostream>
#include <fstream>
#include<string>


class Image{
public:
	Image()
	{
	}

	Image(std::string dataset, std::string imagesname);

	~Image()
	{
	}

private:

};



#endif