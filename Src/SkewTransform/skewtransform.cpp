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
#pragma comment(lib, "winmm.lib")

using namespace cv;

Mat SkewTransform(const int width, const int height, const vector<Point2f> &srcPoints);


void main()
{
	// 120 degree camera
	const double fx = 576.994601f;
	const double fy = 578.579609f;
	const double cx = 338.294750f;
	const double cy = 235.554530f;
	const double k1 = 0.367167f;
	const double k2 = -2.032185f;
	const double p1 = 0.003787f;
	const double p2 = -0.016283f;
	const double k3 = 0.f;
	const double skew_c = 0;


	CameraDistortion camDistort;
	camDistort.SetParams(cx, cy, fx, fy, k1, k2, k3, p1, p2, skew_c);

	IplImage* image = 0;
	CvCapture* capture = cvCaptureFromCAM(1);
	cvNamedWindow("camera", 0);
	cvNamedWindow("skewtransfrom", 0);
	cvResizeWindow("camera", 640, 480);
	cvResizeWindow("skewtransfrom", 640, 480);

	bool undistord = true;

	Mat dst(480, 640, CV_8UC(3));
	vector<Point2f> srcPoints(4);
	srcPoints[0] = Point2f(282, 110);
	srcPoints[1] = Point2f(436, 97);
	srcPoints[2] = Point2f(436, 175);
	srcPoints[3] = Point2f(279, 167);

	const Mat transmtx = SkewTransform(640, 480, srcPoints);

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
			imshow("camera", dst);
		}
		else
		{
			imshow("camera", Mat(image));
		}

		cv::warpPerspective(Mat(image), dst, transmtx, dst.size());
		imshow("skewtransfrom", dst);

		const int key = cvWaitKey(10);
		if (key == ' ')
			undistord = !undistord;
		else if (key >= 0)
			break;
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("camera");
}


Mat SkewTransform(const int width, const int height, const vector<Point2f> &srcPoints)
{
	vector<Point2f> quad(4);
	quad[0] = Point2f(0, 0);
	quad[1] = Point2f((float)width, (float)0);
	quad[2] = Point2f((float)width, (float)height);
	quad[3] = Point2f((float)0, (float)height);

	Mat tm = getPerspectiveTransform(srcPoints, quad);
	return tm;
}
