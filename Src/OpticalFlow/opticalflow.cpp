
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <mmsystem.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv/cvaux.h>
#include "dxcapture.h"

using namespace std;

//#define MAX_COUNT 250
#define MAX_COUNT 50
#define PI 3.1415


#ifdef _DEBUG
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#pragma comment(lib, "opencv_features2d2411d.lib")
#pragma comment(lib, "opencv_video2411d.lib")
#pragma comment(lib, "opencv_objdetect2411d.lib")
//#pragma comment(lib, "opencv_nonfree2411d.lib")
#pragma comment(lib, "winmm.lib")
#else
#pragma comment(lib, "opencv_core2411.lib")
#pragma comment(lib, "opencv_highgui2411.lib")
#pragma comment(lib, "opencv_imgproc2411.lib")
#pragma comment(lib, "opencv_features2d2411.lib")
#pragma comment(lib, "opencv_video2411.lib")
#pragma comment(lib, "opencv_objdetect2411.lib")
//#pragma comment(lib, "opencv_nonfree2411.lib")
#pragma comment(lib, "winmm.lib")
#endif

void main()
{
	//////////////////////////////////////////////////////////////////////////
	//image class           
	IplImage* image = 0;
	//T, T-1 image     
	IplImage* current_Img = 0;
	IplImage* Old_Img = 0;
	//Optical Image     
	IplImage * imgA = 0;
	IplImage * imgB = 0;
	IplImage * grayA = 0;
	IplImage * grayB = 0;

	//Window     
	cvNamedWindow("Origin");
	//////////////////////////////////////////////////////////////////////////     

	//////////////////////////////////////////////////////////////////////////      
	//Optical Flow Variables      
	IplImage * eig_image = 0;
	IplImage * tmp_image = 0;
	int corner_count = MAX_COUNT;
	CvPoint2D32f * cornersA = new CvPoint2D32f[MAX_COUNT];
	CvPoint2D32f * cornersB = new CvPoint2D32f[MAX_COUNT];

	//CvSize img_sz;
	int win_size = 20;

	IplImage* pyrA = 0;
	IplImage* pyrB = 0;

	char features_found[MAX_COUNT];
	float feature_errors[MAX_COUNT];
	//////////////////////////////////////////////////////////////////////////     

	cDxCapture dxCapture;
	if (!dxCapture.Init(0, 640, 480, true))
		return;


	//////////////////////////////////////////////////////////////////////////     
	//Variables for time different video     
	int one_zero = 0;
	int t_delay = 0;

	int oldT = timeGetTime();
	int frame = 0;

	//Routine Start     
	while (1) 
	{
		++frame;
		const int curT = timeGetTime();
		if (curT - oldT > 1000)
		{
			cout << "frame = " << frame << endl;
			frame = 0;
			oldT = curT;
		}

		//Image Create     
		if (Old_Img == 0)
		{
			//image = cvRetrieveFrame(capture);
			image = dxCapture.GetCloneBufferToImage();
			if (!image)
				continue;

			current_Img = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
			Old_Img = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
		}

		if (eig_image == 0)
		{
			eig_image = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
			tmp_image = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
		}

		//copy to image class     
		memcpy(Old_Img->imageData, current_Img->imageData, sizeof(char)*image->imageSize);
		image = dxCapture.GetCloneBufferToImage();
		memcpy(current_Img->imageData, image->imageData, sizeof(char)*image->imageSize);

		//////////////////////////////////////////////////////////////////////////     
		//Create image for Optical flow     
		if (imgA == 0)
		{
			imgA = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
			imgB = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
		}
		if (grayA == 0)
		{
			grayA = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
			grayB = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
		}

		//RGB to Gray for Optical Flow
		cvCvtColor(current_Img, grayA, CV_BGR2GRAY);
		cvCvtColor(Old_Img, grayB, CV_BGR2GRAY);
		cvThreshold(grayA, imgA, 200, 255, CV_THRESH_BINARY);
		cvThreshold(grayB, imgB, 200, 255, CV_THRESH_BINARY);

		//
		cvGoodFeaturesToTrack(imgA, eig_image, tmp_image, cornersA, &corner_count, 0.01, 5.0, 0, 3, 0, 0.04);
		cvFindCornerSubPix(imgA, cornersA, corner_count, cvSize(win_size, win_size), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));


		CvSize pyr_sz = cvSize(imgA->width + 8, imgB->height / 3);
		if (pyrA == 0)
		{
			pyrA = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);
			pyrB = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);
		}

		cvCalcOpticalFlowPyrLK(imgA, imgB, pyrA, pyrB, cornersA, cornersB, corner_count, 
			cvSize(win_size, win_size), 5, features_found, feature_errors, cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3), 0);

		/////////////////////////////////////////////////////////////////////////        
		for (int i = 0; i < corner_count; ++i)
		{
			if (features_found[i] == 0 || feature_errors[i] > MAX_COUNT)
				continue;

			//////////////////////////////////////////////////////////////////////////         
			//Vector Length     
			float fVecLength = sqrt((float)((cornersA[i].x - cornersB[i].x)*(cornersA[i].x - cornersB[i].x) + (cornersA[i].y - cornersB[i].y)*(cornersA[i].y - cornersB[i].y)));
			//Vector Angle     
// 			float fVecSetha = (float)fabs(atan2((float)(cornersB[i].y - cornersA[i].y), (float)(cornersB[i].x - cornersA[i].x)) * 180 / PI);
 			cvLine(image, cvPoint(cornersA[i].x, cornersA[i].y), cvPoint(cornersB[i].x, cornersB[i].y), CV_RGB(0, 255, 0), 2);

			//printf("[%d] - Sheta:%lf, Length:%lf\n", i, fVecSetha, fVecLength);
		}
		//////////////////////////////////////////////////////////////////////////         

		cvShowImage("Origin", image);

		//////////////////////////////////////////////////////////////////////////     

  		if (cvWaitKey(1) >= 0)
  			break;
	}

	//close the window        
	cvDestroyWindow("Origin");
	cvReleaseImage(&Old_Img);
	cvReleaseImage(&imgA);
	cvReleaseImage(&imgB);
	cvReleaseImage(&grayA);
	cvReleaseImage(&grayB);
	cvReleaseImage(&eig_image);
	cvReleaseImage(&tmp_image);
	delete cornersA;
	delete cornersB;
	cvReleaseImage(&pyrA);
	cvReleaseImage(&pyrB);
}
