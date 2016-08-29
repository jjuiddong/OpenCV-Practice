// 카메라 영상 출력 예제

#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <mmsystem.h>
#include "cli.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")

using namespace cv;
using namespace std;

void main()
{
	cout << "check camera..." << endl;

	IplImage* image = 0;
	CvCapture* capture = cvCaptureFromCAM(0);
	cvNamedWindow("HUV05-camera", 0);
	cvResizeWindow("HUV05-camera", 640, 480);
	int result = cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 100);

	cout << "detect camera " << endl;

	cCLI cli;

	int frame = 0;
	int curT = timeGetTime();
	int oldT = curT;

	while (cvGrabFrame(capture))
	{
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

		if (!cli.ParseKey())
			break;
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("HUV05-camera");
}
