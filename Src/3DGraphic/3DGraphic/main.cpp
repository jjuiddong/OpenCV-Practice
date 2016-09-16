
#include <iostream>
#include <string>
#include <opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#ifdef _DEBUG
#pragma comment(lib, "opencv_core310d.lib")
#pragma comment(lib, "opencv_imgcodecs310d.lib")
// #pragma comment(lib, "opencv_features2d310d.lib")
#pragma comment(lib, "opencv_videoio310d.lib")
#pragma comment(lib, "opencv_highgui310d.lib")
 #pragma comment(lib, "opencv_imgproc310d.lib")
// #pragma comment(lib, "opencv_flann310d.lib")
// #pragma comment(lib, "opencv_xfeatures2d310d.lib")
// #pragma comment(lib, "opencv_calib3d310d.lib")
#pragma comment(lib, "aruco209d.lib")
#else
#pragma comment(lib, "opencv_core310.lib")
#pragma comment(lib, "opencv_imgcodecs310.lib")
//	#pragma comment(lib, "opencv_features2d310.lib")
#pragma comment(lib, "opencv_videoio310.lib")
#pragma comment(lib, "opencv_highgui310.lib")
// 	#pragma comment(lib, "opencv_imgproc310.lib")
// 	#pragma comment(lib, "opencv_flann310.lib")
// 	#pragma comment(lib, "opencv_xfeatures2d310.lib")
// 	#pragma comment(lib, "opencv_calib3d310.lib")
#pragma comment(lib, "aruco209.lib")
#endif

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
	Mat mat(768,1024,CV_8UC3);

	int i = 0;
	while (1)
	{
		float angle = (float)CV_PI / 180.f * i++;
		if (i >= 360)
			i = 0;

		Point center(500, 500);
		Mat m1(2, 2, CV_32F);
		m1.at<float>(0, 0) = cos(angle);
		m1.at<float>(0, 1) = sin(angle);
		m1.at<float>(1, 0) = -sin(angle);
		m1.at<float>(1, 1) = cos(angle);

		Mat v1(2, 1, CV_32F);
		v1.at<float>(0, 0) = 100;
		v1.at<float>(1, 0) = 0;

		Mat p = m1 * v1;
		line(mat, center, center+Point(p.at<float>(0,0), p.at<float>(1,0)), Scalar(255, 0, 0));

		imshow("img", mat);
		mat.setTo(Scalar(255, 255, 255));

		if (-1 != waitKey(1))
			break;
	}
}
