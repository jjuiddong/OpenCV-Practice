// 카메라 영상 출력 예제
/*
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <mmsystem.h>
// 
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
// 
// using namespace cv;



void main()
{
	IplImage* image = 0;
	CvCapture* capture = cvCaptureFromCAM(0);
	cvNamedWindow("HUV05-camera", 0);
	cvResizeWindow("HUV05-camera", 640, 480);
	int result = cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 100);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
	//double result = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);

	int frame = 0;
	int curT = timeGetTime();
	int oldT = curT;
	//image = cvQueryFrame(capture);

	while (cvGrabFrame(capture))
	{
		//image = cvRetrieveFrame(capture);
		image = cvQueryFrame(capture);
		cvShowImage("HUV05-camera", image);

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
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("HUV05-camera");
}


/*
#include "opencv2/opencv.hpp"

using namespace cv;

int main(int, char**)
{
	VideoCapture cap("filename"); // put your filename here

	namedWindow("Playback", 1);

	int delay = 15; // 15 ms between frame acquisitions = 2x fast-forward

	while (true)
	{
		Mat frame;
		cap >> frame;
		imshow("Playback", frame);
		if (waitKey(delay) >= 0) break;
	}

	return 0;
}
*/


#include<opencv2/opencv.hpp>
#include<iostream>
#include <windows.h>
#include <time.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")


using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	Mat frame;

	VideoCapture cap(0);
	cap >> frame;
	cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('H', '2', '6', '4'));
	//cap.set(CV_CAP_PROP_FOURCC,CV_FOURCC('M','J','P','G'));
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cap.set(CV_CAP_PROP_FPS, 60);

	Sleep(1000);

	namedWindow("Frame");

	int f = 0;
	bool rec = false;

	time_t start, end;
	time(&start);
	int counter = 0;

	for (;;)
	{
		cap >> frame;

		if (rec) {
			ostringstream filename;
			filename << "frame" << f << ".png";
			imwrite(filename.str().c_str(), frame);
			f++;
			ellipse(frame, Point(100, 100), Size(50, 50), 0, 0, 360, Scalar(0, 0, 255), 20, 8);
		}

		imshow("Frame", frame);

		time(&end);
		++counter;
		double sec = difftime(end, start);
		double fps = counter / sec;
		printf("\n%lf fps", fps);
		printf("\n%lf fps2", cap.get(CV_CAP_PROP_FPS));

		if (waitKey(1) == 27) break;
		if (waitKey(1) == 'r') rec = !rec;
	}
	return 0;
}

