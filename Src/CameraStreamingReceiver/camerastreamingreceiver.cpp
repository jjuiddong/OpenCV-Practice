
#include "stdafx.h"
#include <iostream>
#include "Common/streamingreceiver.h"

using namespace std;


void main(char argc, char *argv[])
{
	if (argc < 3)
	{
		cout << "Command Line <port> <udp=1, tcp=0>" << endl;
		return;
	}

	const int port = atoi(argv[1]);
	const int udp = atoi(argv[2]);

	cvproc::cStreamingReceiver streamRcv;
	if (!streamRcv.Init(udp ? true : false, port))
	{
		cout << "Camera Streaming Error!!!" << endl;
	}
	else
	{
		cout << "Camera Streaming Success" << endl;
	}

	const string windowName = "Camera Streaming Receiver";
	namedWindow(windowName);

	while (1)
	{
		imshow(windowName, streamRcv.Update());

		const int key = cvWaitKey(1);
		if (key == VK_ESCAPE)
			break;
	}
}
