// 영상에서 포인트를 찾아서 표시한다.
// 특정 크기 이상, 특정 모양일 때만 검색한다.
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cvaux.h"
#include "opencv2/nonfree/nonfree.hpp"


using namespace cv;

#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#pragma comment(lib, "opencv_features2d2411d.lib")
#pragma comment(lib, "opencv_nonfree2411d.lib")
#pragma comment(lib, "winmm.lib")


/**
* Helper function to find a cosine of angle between vectors
* from pt0->pt1 and pt0->pt2
*/
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
* Helper function to display text in the center of a contour
*/
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}



void main()
{
	int threshold = 128; // 임계값(Threshold) 설정
	IplImage* image = 0;
	IplImage* output = 0;
	IplImage* gray = 0;

	CvCapture* capture = cvCaptureFromCAM(0);
	cvNamedWindow("HUV05-camera", 0);
	cvNamedWindow("HUV05-binarization", 0);
	cvNamedWindow("HUV05-rect", 0);

	cvResizeWindow("HUV05-camera", 640, 480);
	cvResizeWindow("HUV05-binarization", 640, 480);
	cvResizeWindow("HUV05-rect", 640, 480);
	cvCreateTrackbar("Threthold", "HUV05-camera", &threshold, 255, NULL); // "HUV05-camera" 윈도우에 bar 를 붙이고 

	cvMoveWindow("HUV05-camera", 0, 0);
	cvMoveWindow("HUV05-binarization", 660, 0);
	cvMoveWindow("HUV05-rect", 0, 520);

	int fps = 0;
	int frameCnt = 0;
	DWORD oldT = timeGetTime();
	// FPS를 표시해 줄 Font 및 문자열 버퍼 초기화  
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, 1.0);

	while (1) {
		// check frame per seconds
		++frameCnt;
		DWORD curT = timeGetTime();
		if (curT - oldT > 1000)
		{
			//printf("fps = %d\n", fps);
			fps = frameCnt;
			frameCnt = 0;
			oldT = curT;
		}
		//

		// 		cvGrabFrame(capture);
		// 		image = cvRetrieveFrame(capture);
		image = cvQueryFrame(capture);
		if (!image)
			continue;

		// FPS 출력.
		char strBuffer[64] = { 0, };
		sprintf_s(strBuffer, "%d fps", fps);
		cvPutText(image, strBuffer, cvPoint(10, 20), &font, CV_RGB(255, 0, 255));
		cvShowImage("HUV05-camera", image);

		if (!output) {
			gray = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
			output = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
		}

 		cvCvtColor(image, gray, CV_RGB2GRAY); // 컬러를 흑백으로 변환
 
		if (cvWaitKey(10) >= 0)
			break;

		// 영상의 각 픽셀(x,y) 값이 threshold 값의 초과는 255 로, 그 이하는 0 으로 변환
		cvThreshold(gray, output, threshold, 255, CV_THRESH_BINARY);
		output->origin = image->origin; // 방향이 뒤집어 진것을 바로 잡아줌
		cvShowImage("HUV05-binarization", output);



		Mat src(image);
		Mat grayOut(output);

		// Use Canny instead of threshold to catch squares with gradient shading
		cv::Mat bw;
		cv::Canny(grayOut, bw, 0, 50, 5);

		// Find contours
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		std::vector<cv::Point> approx;
		cv::Mat dst = src.clone();

		for (u_int i = 0; i < contours.size(); i++)
		{
			// Approximate contour with accuracy proportional
			// to the contour perimeter
			cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

			// Skip small or non-convex objects 
			if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
				continue;

			if (approx.size() >= 4)
			{
				// Number of vertices of polygonal curve
				int vtc = approx.size();

				// Get the cosines of all corners
				std::vector<double> cos;
				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

				// Sort ascending the cosine values
				std::sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
				{
					setLabel(dst, "RECT", contours[i]);

					line(dst, approx[0], approx[1], Scalar(255, 0, 0), 2);
					line(dst, approx[1], approx[2], Scalar(255, 0, 0), 2);
					line(dst, approx[2], approx[3], Scalar(255, 0, 0), 2);
					line(dst, approx[3], approx[0], Scalar(255, 0, 0), 2);
				}
			}
		}


		cv::imshow("dst", dst);
		imshow("HUV05-rect", dst);
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("HUV05-camera");
	cvDestroyWindow("HUV05-binarization");
	cvDestroyWindow("HUV05-rect");
}
