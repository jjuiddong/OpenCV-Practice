
#include "global.h"
#include <iostream>

using namespace std;


int main(int argc, char* argv[])
{
	g_camera.Init();

	g_screen.InitResolution(Rect(0, 0, 1024, 768));
	g_screen.DetectScreen(g_camera, 200*200);

	// Skew Screen
	namedWindow("SkewScreen", 0);
	cvResizeWindow("SkewScreen", 640, 480);
	cvMoveWindow("SkewScreen", -660, 520);
	const Mat transmtx = g_screen.SkewTransform(640, 480);
	Mat dst(480, 640, CV_8UC(3));

	bool pause = false;
	while (1) 
	{
		if (!pause)
			g_screen.ShowPingpong(Point2f(100,100), Point2f(100,100), Size2f(50,50));
		cvWaitKey(10);

		Mat &src = g_camera.ShowCaptureUndistortion();
		cv::warpPerspective(src, dst, transmtx, dst.size());
		imshow("SkewScreen", dst);

		const int key = cvWaitKey(10);
		if (key == ' ')
			pause = !pause;
		else if (key >= 0)
			break;
	}

	cvDestroyWindow("SkewScreen");

	return 0;
}
