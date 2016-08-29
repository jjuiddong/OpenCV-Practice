// 카메라 영상 출력 예제

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv/cvaux.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
#ifdef _DEBUG
	#pragma comment(lib, "opencv_core2411d.lib")
	#pragma comment(lib, "opencv_highgui2411d.lib")
	#pragma comment(lib, "opencv_features2d2411d.lib")
	#pragma comment(lib, "opencv_imgproc2411d.lib")
#else
	#pragma comment(lib, "opencv_core2411.lib")
	#pragma comment(lib, "opencv_highgui2411.lib")
	#pragma comment(lib, "opencv_features2d2411.lib")
	#pragma comment(lib, "opencv_imgproc2411.lib")
#endif

using namespace cv;

bool DetectPoint(const Mat &src, Point &out, bool isCvtGray);


void main()
{
//	IplImage* image = 0;
// 	CvCapture* capture = cvCaptureFromCAM(0);
// 	cvNamedWindow("HUV05-camera", 0);
// 	cvResizeWindow("HUV05-camera", 640, 480);
	//int result = cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 100);
	//double result = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);

	int frame = 0;
	int curT = timeGetTime();
	int oldT = curT;
	//image = cvQueryFrame(capture);

//	while (cvGrabFrame(capture))
	while (1)
	{
		//image = cvRetrieveFrame(capture);
// 		image = cvQueryFrame(capture);
//		cvShowImage("HUV05-camera", image);

		Mat src = imread("guntargetskew.jpg", CV_BGR2GRAY);
		imshow("HUV05-camera", src);


		++frame;
		curT = timeGetTime();
		if (curT - oldT > 1000)
		{
			oldT = curT;
			printf("fps = %d\n", frame);
			frame = 0;
		}

		if (cvWaitKey(1) >= 0)
			break;

		//Mat src(image);
		if (src.empty())
			continue;

		Point pt;
		DetectPoint(src, pt, true);
	}

//	cvReleaseCapture(&capture);
	cvDestroyWindow("HUV05-camera");
}



// src 이미지상에서 점을 찾는다.
// 포인트를 찾았다면 해당 위치를 리턴하고, 그렇지 않다면, 전의 위치를 리턴한다.
bool DetectPoint(const Mat &src, Point &out, bool isCvtGray)
{
	const int thresholdValue = 240;

	SimpleBlobDetector::Params params;

	params.thresholdStep = 10;
	params.minRepeatability = 0;
	params.minDistBetweenBlobs = 10;

	// Change thresholds
	params.minThreshold = 1;
	params.maxThreshold = 20;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 50;
	params.maxArea = 3000;

	// Filter by Circularity
	//params.filterByCircularity = true;
	params.filterByCircularity = false;
	params.minCircularity = 0.2f;
	params.maxCircularity = 1;

	// Filter by Convexity
	//params.filterByConvexity = true;
	params.filterByConvexity = false;
	params.minConvexity = 0.3f;
	params.maxConvexity = 1;

	// Filter by Inertia
	//params.filterByInertia = true;
	params.filterByInertia = false;
	params.minInertiaRatio = 0.01f;
	params.maxInertiaRatio = 1.f;



	Mat grayscaleMat = src.clone();
	if (isCvtGray)
		cvtColor(src, grayscaleMat, CV_RGB2GRAY); // 컬러를 흑백으로 변환

	Mat binaryMat(grayscaleMat.size(), grayscaleMat.type());
	//threshold(grayscaleMat, binaryMat, thresholdValue, 255, cv::THRESH_BINARY_INV);
	threshold(grayscaleMat, binaryMat, thresholdValue, 255, cv::THRESH_BINARY);

	imshow("binarymat", binaryMat);

	//----------------------------------------------------------------------------------------------
	// 스크린 이미지에서 포인터를 찾는다.
	SimpleBlobDetector detector(params);

	// Detect blobs.
	std::vector<KeyPoint> keypoints;
	detector.detect(binaryMat, keypoints);

	if (!keypoints.empty())
	{
		int a = 0;
	}

	// 포인트를 찾았다면 해당 위치를 리턴하고, 그렇지 않다면, 전의 위치를 리턴한다.
	Point pos = (keypoints.empty()) ? Point(0,0) : keypoints[0].pt;

	if (keypoints.size() >= 2)
		pos = keypoints[1].pt;

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	Mat im_with_keypoints;
	if (!keypoints.empty())
		drawKeypoints(binaryMat, keypoints, im_with_keypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	if (!keypoints.empty())
		imshow("BinaryScreen", im_with_keypoints);

	if (keypoints.empty())
		return false;

// 	m_pos = pos;
// 	m_points.resize(keypoints.size());
// 	for (u_int i = 0; i < keypoints.size(); ++i)
// 		m_points[i] = keypoints[i].pt;

	out = pos;

// 	if (!keypoints.empty() && g_config.m_printDetectPoint)
// 		printf("pt.x = %d, pt.y = %d \n", pos.x, pos.y);

	return true;
}
