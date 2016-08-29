// 카메라 영상 출력 예제

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")

using namespace cv;


void main()
{
	IplImage* image = 0;
	CvCapture* capture = cvCaptureFromCAM(0);
	cvNamedWindow("camera", 0);
	cvResizeWindow("camera", 640, 480);
	int result = cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 100);
	//double result = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);

	int frame = 0;
	int curT = timeGetTime();
	int oldT = curT;
	//image = cvQueryFrame(capture);

	while (cvGrabFrame(capture))
	{
		//image = cvRetrieveFrame(capture);
		image = cvQueryFrame(capture);
		cvShowImage("camera", image);

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
	cvDestroyWindow("camera");
}
