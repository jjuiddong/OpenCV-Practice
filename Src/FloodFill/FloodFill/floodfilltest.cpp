
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv/cvaux.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>

#ifdef _DEBUG
#pragma comment(lib, "opencv_core310d.lib")
#pragma comment(lib, "opencv_imgcodecs310d.lib")
#pragma comment(lib, "opencv_features2d310d.lib")
#pragma comment(lib, "opencv_videoio310d.lib")
#pragma comment(lib, "opencv_highgui310d.lib")
#pragma comment(lib, "opencv_imgproc310d.lib")
#pragma comment(lib, "opencv_flann310d.lib")
#pragma comment(lib, "opencv_xfeatures2d310d.lib")
#pragma comment(lib, "opencv_calib3d310d.lib")
#else
#pragma comment(lib, "opencv_core310.lib")
#pragma comment(lib, "opencv_imgcodecs310.lib")
#pragma comment(lib, "opencv_features2d310.lib")
#pragma comment(lib, "opencv_videoio310.lib")
#pragma comment(lib, "opencv_highgui310.lib")
#pragma comment(lib, "opencv_imgproc310.lib")
#pragma comment(lib, "opencv_flann310.lib")
#pragma comment(lib, "opencv_xfeatures2d310.lib")
#pragma comment(lib, "opencv_calib3d310.lib")
#endif


using namespace std;
using namespace cv;

void main()
{
	Mat img = imread("myImage.png");
	
	// alpha channel
	Mat dst = img.clone();
	floodFill(dst, Point(3, 3), Scalar(0, 0, 0));
	cvtColor(dst, dst, CV_BGR2BGRA);
	DWORD *pixel = (DWORD*)dst.data;
	for (int x = 0; x < dst.cols; ++x)
		for (int y = 0; y < dst.rows; ++y)
		{
			if (*pixel == 0xFF000000)
				*pixel = 0x00;
			++pixel;
		}
	
	// labeling
	Mat img_labels, stats, centroids;
	cvtColor(dst, dst, CV_BGRA2GRAY);
	int numOfLables = connectedComponentsWithStats(dst, img_labels,
		stats, centroids, 8, CV_32S);
	
	//라벨링 된 이미지에 각각 직사각형으로 둘러싸기 
	for (int j = 1; j < numOfLables; j++) 
	{
		int area = stats.at<int>(j, CC_STAT_AREA);
		int left = stats.at<int>(j, CC_STAT_LEFT);
		int top = stats.at<int>(j, CC_STAT_TOP);
		int width = stats.at<int>(j, CC_STAT_WIDTH);
		int height = stats.at<int>(j, CC_STAT_HEIGHT);
		rectangle(img, Point(left, top), Point(left + width, top + height),
			Scalar(0, 0, 255), 1);
		putText(img, to_string(j), Point(left + 20, top + 20),
			FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
	}

	imshow("window", img);
	cvWaitKey(0);
}
