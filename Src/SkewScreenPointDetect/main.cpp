
#include "global.h"
#include <iostream>
#include "detectpoint.h"

using namespace std;


int main(int argc, char* argv[])
{
	g_camera.Init();

	g_screen.InitResolution(Rect(0, 0, 1024, 768));
	g_screen.DetectScreen(g_camera, 100 * 100);

	// Skew Screen
	namedWindow("SkewScreen", 0);
	cvResizeWindow("SkewScreen", 640, 480);
	cvMoveWindow("SkewScreen", -660, 520);
	const Mat transmtx = g_screen.SkewTransform(640, 480);
	Mat dst(480, 640, CV_8UC(3));

	cDetectPoint detectPoint;

	bool pause = false;
	while (1)
	{
 		if (!pause)
 			g_screen.ShowPingpong(Point2f(100, 100), Point2f(100, 100), Size2f(20, 20));
		cvWaitKey(10);

		Mat &src = g_camera.ShowCaptureUndistortion();
		cv::warpPerspective(src, dst, transmtx, dst.size());
		imshow("SkewScreen", dst);

		// 포인트 검사.
		detectPoint.DetectPoint(g_screen, g_camera, dst);


		const int key = cvWaitKey(10);
		if (key == ' ')
			pause = !pause;
		else if (key >= 0)
			break;
	}

	cvDestroyWindow("SkewScreen");

	return 0;
}
