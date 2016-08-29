//
// http://darkpgmr.tistory.com/31 예제 소스를 이용한 카메라 왜곡 보정 예제.
//

#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "CameraDistortion.h"


#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#pragma comment(lib, "opencv_features2d2411d.lib")
//#pragma comment(lib, "opencv_nonfree2411d.lib")
#pragma comment(lib, "winmm.lib")


using namespace cv;

void main()
{
	// 60 degreee camera
// 	const double cx = 346.982762f;
// 	const double cy = 229.626156f;
// 	const double fx = 676.748756f;
// 	const double fy = 677.032333f;
// 	const double k1 = -0.331818f;
// 	const double k2 = -0.033314f;
// 	const double k3 = 0.f;
// 	const double p1 = -0.000313f;
// 	const double p2 = 0.000841f;
// 	const double skew_c = 0;

	// 120 degree camera
	const double cx = 338.294750f;
	const double cy = 235.554530f;
	const double fx = 576.994601f;
	const double fy = 578.579609f;
	const double k1 = 0.367167f;
	const double k2 = -2.032185f;
	const double k3 = 0.f;
	const double p1 = 0.003787f;
	const double p2 = -0.016283f;
	const double skew_c = 0;

	CameraDistortion camDistort;
	camDistort.SetParams(cx, cy, fx, fy, k1, k2, k3, p1, p2, skew_c);

	IplImage* image = 0;
	CvCapture* capture = cvCaptureFromCAM(1);
	cvNamedWindow("HUV05-camera", 0);
	cvResizeWindow("HUV05-camera", 640, 480);

	bool undistord = true;

	while (1) {
		cvGrabFrame(capture);
		image = cvRetrieveFrame(capture);
		if (!image)
			continue;

		if (undistord)
		{
			Mat src(image);
			Mat dst = src.clone();
			camDistort.UndistortImage(src, dst);
			//cvShowImage("HUV05-camera", image);
			imshow("HUV05-camera", dst);
		}
		else
		{
			imshow("HUV05-camera", Mat(image));
		}

		const int key = cvWaitKey(10);
		if (key == ' ')
			undistord = !undistord;
		else if (key >= 0)
			break;
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("HUV05-camera");
}

