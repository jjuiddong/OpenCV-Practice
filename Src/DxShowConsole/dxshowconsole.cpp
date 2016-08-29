
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <windows.h>
#include <mmsystem.h>
#include "dxcapture.h"

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
	cvNamedWindow("HUV05-camera", 0);
	cvResizeWindow("HUV05-camera", 640, 480);

	cDxCapture dxCapture;
	dxCapture.Init(640, 480);

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

// 		dxCapture.WriteBitmapCurrentBuffer("test.bmp");
//  		Mat image;
//  		image = imread("test.bmp", CV_LOAD_IMAGE_COLOR);
// 		imshow("HUV05-camera", image);

		imshow("HUV05-camera", Mat(dxCapture.GetBufferToImage()));
		if (waitKey(1) == VK_ESCAPE)
			break;
	}

	cvDestroyWindow("HUV05-camera");
}

