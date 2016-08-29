// 카메라 영상 출력 예제

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")
// #pragma comment(lib, "opencv_core2411d.lib")
// #pragma comment(lib, "opencv_highgui2411d.lib")
// #pragma comment(lib, "opencv_imgproc2411d.lib")
// #pragma comment(lib, "opencv_features2d2411d.lib")
//#pragma comment(lib, "opencv_nonfree2411d.lib")
#pragma comment(lib, "opencv_ts300d.lib")
#pragma comment(lib, "opencv_world300d.lib")


using namespace cv;

void main()
{
	IplImage* image = 0;
	CvCapture* capture = cvCaptureFromCAM(0);
	//cvCamSetProperty(0, CVCAM_PROP_ENABLE, &nselected);
	//cvSetCaptureProperty(capture, 0, CVCAM_PROP_ENABLE,)

	cvNamedWindow("HUV05-camera", 0);
	cvResizeWindow("HUV05-camera", 640, 480);
	//int result = cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, 100);


	//double result = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);

	int frame = 0;
	int curT = timeGetTime();
	int oldT = curT;
	//image = cvQueryFrame(capture);

	while (1)// cvGrabFrame(capture))
	{
		//image = cvRetrieveFrame(capture);
		cvGrabFrame(capture);
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
