#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "photometric_stereo.h"

using namespace std;
using namespace cv;


ImageGroup::ImageGroup(string dataset, string imagesname)
{
	imagesname_ = imagesname;
	string lightsfile = dataset + "\\CalibratedLight.txt";
	FILE* fid = nullptr;
	fid = fopen(lightsfile.c_str(), "r");

	if (!fid){
		cerr << "Read failure!" << endl;
		exit(0);
	}

	fscanf(fid, "%d\n", &numlights_);
	float** lightdirections = new float* [numlights_];
	lights_.create(numlights_, 3, CV_32FC1);

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

	string maskdir = dataset + "\\" + imagesname + "\\" + imagesname + "." + "mask.jpg";

	// 略有混乱
	Mat mask = imread(maskdir.c_str(), 0);
	width_ = mask.cols;
	height_ = mask.rows;

	maskimage_.create(height_, width_, CV_32FC1);
	mask.convertTo(maskimage_, CV_32FC1);
	
	string image_serial;
	string image_directory = dataset + "\\" + imagesname + "\\" + imagesname +".";



	for(i = 0; i < numlights_; i++)
	{
		char numstr[5] = {};
	    image_serial = image_directory + _itoa(i, numstr, 10) + ".jpg";
		//sprintf(numstr, "%s", i);
		//image_serial = image_directory + numstr + ".jpg";
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
	//读入低分辨率几何结构
}

void ImageGroup:: remove_noise(  )
{
	vector< vector<Mat>  > accimgseq;
	vector< Mat> matseq;
	accimgseq.resize( numlights_ );
    for ( vector<Mat>::size_type i = 0; i != colorimagesmat_.size(); i++ )
	{
		split ( colorimagesmat_[i], accimgseq[i] );
	}
	vector<float*> rptrseq, gptrseq, bptrseq;
	rptrseq.resize( numlights_ );
	gptrseq.resize( numlights_ );
	bptrseq.resize( numlights_ );
	for( vector<float*>::size_type k = 0; k != rptrseq.size(); k++ )
	{
		rptrseq[k] = accimgseq[k][2].ptr<float>(0);
		gptrseq[k] = accimgseq[k][1].ptr<float>(0);
		bptrseq[k] = accimgseq[k][0].ptr<float>(0);
	}
	vector< float> r, g, b;
	int nc = height_ * width_ * maskimage_.channels();
	r.resize( nc );
	g.resize( nc );
	b.resize( nc );

	float* data = maskimage_.ptr< float >(0);
	for ( vector<float>::size_type i = 0; i != r.size(); i++ )
	{
	  for( int k = 0; k < numlights_; k++ )
		{
			r[i] = r[i] + rptrseq[k][i];
			g[i] = g[i] + gptrseq[k][i];
			b[i] = b[i] + bptrseq[k][i];
		}
	}
	for ( long int i = 0; i < nc; i++ )
	{
	  if ( r[i] < 25.0 || g[i] < 25.0 || b[i] < 25.0 )
	  {
		  data[i] = 0.0;
	  }
	}
}
int CalculateNormals(const ImageGroup& ig, Mat& normals)
{
	Mat I(ig.numlights_, 1, CV_32FC1);
	Mat dstmat(3, 1, CV_32FC1);
//	normals.create(ig.height_, ig.width_, CV_32FC3);
	vector<Mat> channel;
	split(normals, channel);


	channel[0].setTo(0.0);
	channel[1].setTo(0.0);
	channel[2].setTo(1.0);

	int i, j, k;
	for(i = 0; i < ig.height_; i++)
	{
		for(j = 0; j < ig.width_; j++)
		{
			if(ig.maskimage_.at<float>(i,j))
			{
				for(k = 0; k != ig.numlights_; k++)
				{
					I.at<float>(k,0) = ig.grayimagesmat_[k].at<float>(i,j);
				}
				bool nid = solve(ig.lights_, I, dstmat, DECOMP_SVD);
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
			if (j == 253)
			{
				int nbreak = 1;
			}
		}
		if (i == 63)
		{
			int nbreak = 1;
		}
	}
 	merge(channel, normals);

	/*ofstream file;
	file.open( "normal.txt");
	for ( int i = 0; i < ig.height_; i++ )
	{
		for ( int j = 0; j < ig.width_; j++ )
		{
			
			file <<i<<" "<<j<<" "<<ig.maskimage_.at<float>(i,j)<<" "<< channel[0].at<float>(i,j) <<"   "<< channel[1].at<float>(i,j) <<"   "<< channel[2].at<float>(i,j) << endl;
	
		}
	}

	file.close();*/
	    
  /* ofstream file5;
   file5.open("graymat.txt");
	for (i = 0; i < ig.height_; i++ )
	{
		for (j = 0; j < ig.width_; j++ )
		{
	
			file5 <<i<<" "<<j<<" "<<ig.grayimagesmat_[0].at<float>(i,j)<<" "<<ig.grayimagesmat_[1].at<float>(i,j)<<endl;
		}
	}
   file5.close();*/

   /* Mat albedo;
	albedo = Mat::zeros( ig.height_, ig.width_, CV_8UC3 );
	Mat tempalbd = abs( normals )*255;
	tempalbd.convertTo( albedo, CV_8U, 1, 0 );
	namedWindow( "surfnormal", CV_WINDOW_AUTOSIZE );
	imshow( "surfnormal", albedo );
	waitKey( 0 );
	destroyWindow( "surfnormal" );*/

	return 1;
}

int CalculateGradients(const ImageGroup& ig, const Mat& normals, Mat& gradients, float min_angle_rad)
{
	int i, j;
	for(i = 0; i < ig.height_; i++)
	{
		for(j = 0; j < ig.width_; j++)
		{
			if(ig.maskimage_.at<float>(i,j))
			{
				if(abs(normals.at<Vec3f>(i,j)[2] < min_angle_rad))
				{
					gradients.at<Vec3f>(i,j)[0] = 0.0;
					gradients.at<Vec3f>(i,j)[1] = 0.0;
					gradients.at<Vec3f>(i,j)[2] = 0.0;
				}
				else
				{
					gradients.at<Vec3f>(i,j)[0] = -normals.at<Vec3f>(i,j)[0] / normals.at<Vec3f>(i,j)[2];
					gradients.at<Vec3f>(i,j)[1] = -normals.at<Vec3f>(i,j)[1] / normals.at<Vec3f>(i,j)[2];
					gradients.at<Vec3f>(i,j)[2] = 1.0f;
				}
			}
		}
	}
	return 1;
}

int CalculateDepths(const ImageGroup& ig, const Mat& gradients, Mat& surface)
{
	int i, j;
	double maxErr;
	double sigma = 1.99;
	maxErr = 1e-4;
	int maxCount = 5000;

	//ofstream file3;
	//file3.open("err1.txt");
	for(int k = 0; k < maxCount; k++)
	{
		double err = 0.0;
	    for (i = 0; i < ig.height_ ; i++)
       {
		   for (j = ig.width_ -1; j >= 0; j--)
	           {
			        if (ig.maskimage_.at<float> (i, j) != MASK_BG)
					{
						double buf = 0.0;
						double temperr = 0.0;
						double b =(gradients.at<Vec3f>(i,j+1)[0] - gradients.at<Vec3f>(i,j)[0]) + (gradients.at<Vec3f>(i-1,j)[1]-gradients.at<Vec3f>(i,j)[1]);//认为边界的梯度为0
					   	if (ig.maskimage_.at<float> (i-1, j) != MASK_BG)//(X,Y+1)
						{buf += surface.at<float>(i-1, j);}
						if (ig.maskimage_.at<float> ( i, j-1 ) != MASK_BG)//(X-1,Y)
						{buf += surface.at<float>(i, j-1 );}
						buf -= 4* surface.at<float>(i,j);
						if (ig.maskimage_.at<float> (i, j+1) != MASK_BG)//(X+1,Y)
						{buf += surface.at<float>(i, j+1);}
						if (ig.maskimage_.at<float> (i+1, j) != MASK_BG)//(X,Y-1)
						{buf += surface.at<float>(i+1, j);}
						temperr = (-0.25) * ( b - buf)*sigma;
						if ( abs(temperr) > abs(err) )
						{err = temperr;}
						surface.at<float>(i, j) += temperr;
						//file3<<err<<" ";
					  }
		       }
      
       }
		        //file3 << err<<" ";
		        if (abs (err) < maxErr)
	           {
			        cout<<"The interation is accomplished in"<<k<<"times"<<endl;
			        return 1;
			        break;
			   }      
	} 
	//file3.close();
	return 0;
}