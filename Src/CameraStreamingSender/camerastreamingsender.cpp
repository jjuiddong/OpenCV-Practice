
#include "stdafx.h"
#include <iostream>
#include <string>
#include "Common/dxcapture.h"
#include "Common/streamingsender.h"


using namespace std;


void main(char argc, char *argv[])
{
	if (argc < 7)
	{
		cout << "Command Line <ip> <port> <camera index> <udp=1, tcp=0> <gray=1 or 0> <compressed=1 or 0>" << endl;
		return;
	}

	const string ip = argv[1];
	const int port = atoi(argv[2]);
	const int cameraidx = atoi(argv[3]);
	const bool udp = atoi(argv[4]) ? true : false;
	const bool gray = atoi(argv[5]) ? true : false;
	const bool compressed = atoi(argv[6]) ? true : false;

	cout << "Camera Stream Sender ip = " << ip << ", port = " << port << endl;
	cout << "camera index = " << cameraidx << ", udp = " << udp << ", gray = " << gray << ", compressed = " << compressed << endl << endl;
	cout << "Camera Search..." << endl;

	// 카메라, 스크린 초기화.
	cDxCapture dxCapture1;
	if (dxCapture1.Init(cameraidx, 640, 480, true))
	{
		cout << "Camera Success " << endl;
	}
	else
	{
		cout << "Camera Fail..." << endl;
		return;
	}

	// Streaming  Client
	cvproc::cStreamingSender streamSender;
	if (!streamSender.Init(udp, ip, port, gray, compressed))
	{
		cout << "Camera Streaming Init Error!!!" << endl;
	}
	else
	{
		cout << "Camera Streaming Init Success" << endl;
	}

	const string windowName = "Camera Streaming Sender";
	namedWindow(windowName);

	Mat cam(480, 640, CV_8UC3);
	Mat camClone(480, 640, CV_8UC3);

	int oldT = timeGetTime();
	int frame = 0;
	while (1)
	{
		++frame;
		const int curT = timeGetTime();
		const int deltaT = curT - oldT;
		if (deltaT > 1000)
		{
			//cout << "frame = " << frame << endl;
			frame = 0;
			oldT = curT;
		}

		// Capture Camera
		long camBufferSize;
		BYTE *camBuffer = dxCapture1.GetCloneBuffer(camBufferSize);
		memcpy(cam.data, camBuffer, camBufferSize);
		cv::flip(cam, camClone, 0);

		streamSender.Send(camClone);

		imshow(windowName, camClone);

		const int key = cvWaitKey(1);
		if (key == VK_ESCAPE)
			break;
	}
}
