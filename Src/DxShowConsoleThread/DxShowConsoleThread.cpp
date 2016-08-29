
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
	cvNamedWindow("camera", 0);
	cvResizeWindow("camera", 640, 480);

	cDxCapture dxCapture;
	dxCapture.Init(1, 640, 480, true);

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

		long size;
		BYTE *buffer = dxCapture.GetCloneBuffer(size);

		// buffer to IplImage
		IplImage *Image = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 3);
		Image->imageSize = size;
		CopyMemory(Image->imageData, buffer, size);
		cvFlip(Image);
		Mat cvMat = Mat(Image, true);
		imshow("camera", cvMat);
		cvReleaseImage(&Image);

//		dxCapture.WriteBitmapFromBuffer("test.bmp", buffer, size);
// 		Mat image;
// 		image = imread("test.bmp", CV_LOAD_IMAGE_COLOR);
//		imshow("camera", image);

 		if (waitKey(1) == VK_ESCAPE)
 			break;
	}

	cvDestroyWindow("camera");
}
