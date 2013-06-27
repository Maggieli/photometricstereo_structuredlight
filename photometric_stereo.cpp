#include <Windows.h>
#include "photometric_stereo.h"
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;

PhotometricStereo::PhotometricStereo(string dataset, string imagesname)
{
	imagesname_ = imagesname;
	string lightsfile = dataset + "\\CalibratedLight.txt";
	FILE* fid = nullptr;
	fid = fopen(lightsfile.c_str(), "r");

	if(!fid)
	{
		cerr << "Read failure!" << endl;
		exit(0);
	}

	fscanf(fid, "%d\n", &numlights_);
	float** lightdirections = new float* [numlights_];
	lights_.create(numlights_, 3, CV_32F);

	int i, j;
	for(i = 0; i < numlights_; i++)
	{
		lightdirections[i] = new float[3];
		for(j = 0; j < 3; j++)
		{
			fscanf(fid, "%f", &lights_.at<float>(i,j));
		}
	}
	fclose(fid);

	for(i = 0; i < numlights_; i++)
		delete lightdirections[i];
	delete lightdirections;

	string maskdir = dataset + "\\" + imagesname + "\\" + imagesname + ".mask.jpg";

	Mat mask = imread(maskdir.c_str(), 0);
	width_ = mask.cols;
	height_ = mask.rows;

	maskimage_.create(height_, width_, CV_32F);
	mask.convertTo(maskimage_, CV_32F);

	normal_ = Mat::zeros(height_, width_, CV_32FC3);
	gradient_ = Mat::zeros(height_, width_, CV_32FC3);
	depth_ = Mat::zeros(height_, width_, CV_32FC1);


	string image_serial;
	string image_directory = dataset + "\\" + imagesname + "\\" + imagesname + ".";

	for(i = 0; i < numlights_; i++)
	{
		char numstr[5] = {};
		image_serial = image_directory + _itoa(i, numstr, 10) + ".jpg";
		Mat image = imread(image_serial.c_str(), -1);
		Mat grayimg(height_, width_, CV_8UC1);
		Mat graymat(height_, width_, CV_32FC1);
		Mat colormat(height_, width_, CV_32FC3);
		
		cvtColor(image, grayimg, CV_RGB2GRAY);
		grayimg.convertTo(graymat, CV_32FC1);
		image.convertTo(colormat, CV_32FC3);
		colorimagesmat_.push_back(colormat);
		grayimagesmat_.push_back(graymat);
	}
}

void PhotometricStereo::RemoveNoise()
{
	vector<vector<Mat> > accimgseq;
	vector<Mat> matseq;
	accimgseq.resize(numlights_);
	for(vector<Mat>::size_type i = 0; i != colorimagesmat_.size(); i++)
	{
		split(colorimagesmat_[i], accimgseq[i]);
	}

	vector<float*> rptrseq, gptrseq, bptrseq;
	rptrseq.resize(numlights_);
	gptrseq.resize(numlights_);
	bptrseq.resize(numlights_);

	for(vector<float*>::size_type k = 0; k != rptrseq.size(); k++)
	{
		rptrseq[k] = accimgseq[k][2].ptr<float>(0);
		gptrseq[k] = accimgseq[k][1].ptr<float>(0);
		bptrseq[k] = accimgseq[k][0].ptr<float>(0);
	}

	vector<float> r, g, b;
	int nc = height_ * width_ * maskimage_.channels();
	r.resize(nc);
	g.resize(nc);
	b.resize(nc);

	float *data = maskimage_.ptr<float>(0);
	for(vector<float>::size_type i = 0; i != r.size(); i++)
	{
		for(int k = 0; k < numlights_; k++)
		{
			r[i] += rptrseq[k][i];
			g[i] += gptrseq[k][i];
			b[i] += bptrseq[k][i];
		}
	}

	for(long int i = 0; i < nc; i++)
	{
		if(r[i] < 25.0 || g[i] < 25.0 || b[i] < 25.0)
		{
			data[i] = 0.0;
		}
	}
}

bool PhotometricStereo::CalculateNormals()
{
	Mat I(numlights_, 1, CV_32FC1);
	Mat dstmat(3, 1, CV_32FC1);
	
	vector<Mat> channel;
	split(normal_, channel);

	channel[0].setTo(0.0);
	channel[1].setTo(0.0);
	channel[2].setTo(1.0);

	int i, j, k;
	for(i = 0; i < height_; i++)
	{
		for(j = 0; j < width_; j++)
		{
			if(maskimage_.at<float>(i,j))
			{
				for(k = 0; k != numlights_; k++)
				{
					I.at<float>(k,0) = grayimagesmat_[k].at<float>(i,j);
				}
				bool nid = solve(lights_, I, dstmat, DECOMP_SVD);
				dstmat /= norm(dstmat);
				channel[0].at<float>(i,j) = dstmat.at<float>(0,0);
				channel[1].at<float>(i,j) = dstmat.at<float>(1,0);
				channel[2].at<float>(i,j) = dstmat.at<float>(2,0);

				if(abs(channel[2].at<float>(i,j)) < 1.0e-4)
				{
					channel[0].at<float>(i,j) = 0.0;
					channel[1].at<float>(i,j) = 0.0;
					channel[2].at<float>(i,j) = 1.0;
				}
				if(channel[2].at<float>(i,j) < 0.0)
				{
					channel[2].at<float>(i,j) = abs(channel[2].at<float>(i,j));
				}
			}
		}
	}
 	merge(channel, normal_);

	return true;
}

bool PhotometricStereo::CalculateGradients(float min_angle_rad)
{
	int i, j;
	for(i = 0; i < height_; i++)
	{
		for(j = 0; j < width_; j++)
		{
			if(maskimage_.at<float>(i,j))
			{
				if(abs(normal_.at<Vec3f>(i,j)[2] < min_angle_rad))
				{
					gradient_.at<Vec3f>(i,j)[0] = 0.0;
					gradient_.at<Vec3f>(i,j)[1] = 0.0;
					gradient_.at<Vec3f>(i,j)[2] = 0.0;
				}
				else
				{
					gradient_.at<Vec3f>(i,j)[0] = -normal_.at<Vec3f>(i,j)[0] / normal_.at<Vec3f>(i,j)[2];
					gradient_.at<Vec3f>(i,j)[1] = -normal_.at<Vec3f>(i,j)[1] / normal_.at<Vec3f>(i,j)[2];
					gradient_.at<Vec3f>(i,j)[2] = 1.0f;
				}
			}
		}
	}


	/*ofstream file1;
	file1.open("gradient.txt");
	for(i = 0; i < height_; i++)
	{
		for(j = 0; j < width_; j++)
		{
			file1<< gradient_.at<Vec3f>(i,j)[0]<< " "<<gradient_.at<Vec3f>(i,j)[1]<<" "<<gradient_.at<Vec3f>(i,j)[2]<<endl;
		}
	}
	file1.close();*/
	return true;
}

bool PhotometricStereo::CalculateDepth()
{
	int i, j;
	double maxErr;
	double sigma = 1.99;
	maxErr = 1e-4;
	int maxCount = 5000;

	ofstream file3;
	file3.open("err.txt");
   /* for (i = 0; i < height_ ; i++)
      {
		   for (j = width_ -1; j >= 0; j--)
	           {
				   file3<< depth_.at<float>(i,j)<<" ";
		   }
	}*/

	for(int k = 0; k < maxCount; k++)
	{
		double err = 0.0;
	    for (i = 0; i < height_ ; i++)
      {
		   for (j = width_ -1; j >= 0; j--)
	           {
			        if (maskimage_.at<float> (i, j))
					{
						double buf = 0.0;
						double temperr = 0.0;
						double b =(gradient_.at<Vec3f>(i,j+1)[0] - gradient_.at<Vec3f>(i,j)[0]) + (gradient_.at<Vec3f>(i-1,j)[1]-gradient_.at<Vec3f>(i,j)[1]);
					   	if (maskimage_.at<float> (i-1, j))//(X,Y+1)
						{buf += depth_.at<float>(i-1, j);}
						if (maskimage_.at<float> ( i, j-1 ))//(X-1,Y)
						{buf += depth_.at<float>(i, j-1 );}
						buf -= 4* depth_.at<float>(i,j);
						if (maskimage_.at<float> (i, j+1))//(X+1,Y)
						{buf += depth_.at<float>(i, j+1);}
						if (maskimage_.at<float> (i+1, j))//(X,Y-1)
						{buf += depth_.at<float>(i+1, j);}
						temperr = (-0.25) * ( b - buf)*sigma;
						if ( abs(temperr) > abs(err) )
						{err = temperr;}
						depth_.at<float>(i, j) += temperr;
						//file3<<err<<" ";
					  }
		       }
      
       }
		        file3 << err<<" ";
		        if (abs (err) < maxErr)
	           {
			        cout<<"The interation is accomplished in"<<k<<"times"<<endl;
			        return 1;
			        break;
			   }      
	} 
	file3.close();


	/*ofstream file;
	file.open(( imagesname_ + "sorr.ply" ).c_str());
	file << "ply" << endl;
	file << "format ascii 1.0" << endl;
	file << "comment made by Min" << endl;
	file << "comment this file is a "<< endl;
	file << "element vertex " << height_*width_<< endl;
	file << "property float32 x" << endl;
	file << "property float32 y" << endl;
	file << "property float32 z" << endl;
	file << "property uint8 red " << endl;
	file << "property uint8 green" << endl;
	file << "property uint8 blue" << endl;
	file << "end_header" << endl;
	for (int i = 0; i < height_; i++)
   {
		for (int j = 0; j <width_; j++)
	   {

			file <<i <<" "<<j <<" "<<depth_.at <float>(i, j)<<" "
				<<colorimagesmat_[0].at<Vec3f>(i, j)[2]<<" "
				<< colorimagesmat_[0].at<Vec3f>(i, j)[1]<<" "
				<< colorimagesmat_[0].at<Vec3f>(i, j)[0]<<endl;
	   }
   }
  file.close();*/
  return true;
}