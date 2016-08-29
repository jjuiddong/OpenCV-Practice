
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "camera.h"
#include "config.h"

extern cCamera g_camera;
extern cConfig g_config;


#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#pragma comment(lib, "opencv_features2d2411d.lib")


using namespace std;
using namespace cv;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

	}
}


int main(int argc, char** argv)
{
	g_camera.Init(g_config.m_cameraIndex, Size(g_config.m_cameraWidth, g_config.m_cameraHeight));

	//set the callback function for any mouse event
	setMouseCallback("HUV05-camera", CallBackFunc, NULL);

	while (1)
	{
		Mat img = g_camera.ShowCapture();
		if (img.empty())
			continue;

		// Wait until user press some key
		if (cvWaitKey(10) > 0)
			break;
	}

	return 0;
}
