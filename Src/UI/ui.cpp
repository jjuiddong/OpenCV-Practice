
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#else
#pragma comment(lib, "opencv_core2411.lib")
#pragma comment(lib, "opencv_highgui2411.lib")
#endif

using namespace cv;

void main()
{
	cvNamedWindow("window", 0);
	cvResizeWindow("window", 640, 480);

	Mat image(480, 640, CV_8UC3);
	image.setTo(Scalar(0, 0, 0));

	// Main message loop
	int curT = timeGetTime();
	int oldT = curT;
	int fps = 0;
	while (1)
	{
		fps++;
		curT = timeGetTime();
		if (curT - oldT > 1000)
		{
			printf("%d\n", fps);
			oldT = curT;
			fps = 0;
		}

		putText(image, "hello", Point(100, 100), 2, 2, Scalar(255, 255, 255));
		imshow("camera", image);

		if (waitKey(1) == VK_ESCAPE)
			break;
	}

	cvDestroyAllWindows();
}

