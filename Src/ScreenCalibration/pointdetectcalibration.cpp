
#include "global.h"
#include "pointdetectcalibration.h"
#include <iostream>

using namespace cv;

Point g_mousePos;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_MOUSEMOVE)
	{
		g_mousePos.x = x;
		g_mousePos.y = y;
	//	cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
	}
}


// 포인트 검사를 시뮬레이션 한다.
void DetectPointCalibration()
{
	const int w = g_screen.GetWidth();
	const int h = g_screen.GetHeight();
	const int rows = 5;
	const int cols = 5;
	const int cellW = w / cols;
	const int cellH = h / rows;

	int threshold = 33;
	IplImage *camera = 0;
	IplImage *binImage = 0;
	IplImage *binOutput = 0;

	Mat screen(h, w, CV_8UC(3));
	namedWindow("Screen", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Screen", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	cvNamedWindow("HUV05-camera", 0);
	cvNamedWindow("HUV05-binarization", 0);
	cvNamedWindow("HUV05-screen", 0);

	cvResizeWindow("HUV05-camera", 640, 480);
	cvResizeWindow("HUV05-binarization", 640, 480);
	cvResizeWindow("HUV05-screen", 640, 480);

	cvMoveWindow("HUV05-camera", -660, 0);
	cvMoveWindow("HUV05-binarization", -660, 520);
	cvMoveWindow("HUV05-screen", -660, 0);
	cvCreateTrackbar("Threshold", "HUV05-camera", &threshold, 255, NULL); // "HUV05-camera" 윈도우에 bar 를 붙인다. 
	cvCreateTrackbar("Threshold", "HUV05-screen", &threshold, 255, NULL); // "HUV05-camera" 윈도우에 bar 를 붙인다. 

	// 마우스 이벤트 핸들러 설정.
	setMouseCallback("Screen", CallBackFunc, NULL);


	std::vector<vector<Point> > contours(1);
	contours[0] = g_screen.GetScreenContour();	
	
	Point2f pointPos(100,100);
	Point2f pointV(100, 100);
	Point recogPointPos(500, 300);
	const float circleW = 30;
	const float circleH = 30;
	
	int curT = timeGetTime();
	int oldT = curT;

	while (1)
	{
		// 포인터 움직임.
		curT = timeGetTime();
		float incT = (float)(curT - oldT) / 1000.f;
		if (incT > 0.1f)
			incT = 0.1f;

		oldT = curT;
		
		Point2f pointCurPos = pointPos + pointV * incT;
		if (pointCurPos.x < 0 && (pointV.x < 0))
			pointV.x = -pointV.x;
		else if (pointCurPos.x > w && (pointV.x > 0))
			pointV.x = -pointV.x;
		if (pointCurPos.y < 0 && (pointV.y < 0))
			pointV.y = -pointV.y;
		else if (pointCurPos.y > h && (pointV.y > 0))
			pointV.y = -pointV.y;
		pointPos = pointCurPos;

		pointPos = g_mousePos;


		screen.setTo(Scalar(255, 255, 255));
		ellipse(screen, RotatedRect(recogPointPos, Size2f(circleW+6, circleH+6), 0), Scalar(200, 200, 255), CV_FILLED);
		ellipse(screen, RotatedRect(pointPos, Size2f(circleW, circleH), 0), Scalar(0, 0, 0), CV_FILLED);
		imshow("Screen", screen);

		//cvWaitKey(10);

		camera = g_camera.GetCapture();


		// 스크린 위치 영상만 마스킹해서 가져온다.
		Mat src(camera);
		Mat mask = Mat::zeros(src.rows, src.cols, CV_8UC1);
		drawContours(mask, contours, 0, Scalar(255), CV_FILLED);
		Mat maskingScreen(src.rows, src.cols, CV_8UC3);
		maskingScreen.setTo(Scalar(255, 255, 255));
		src.copyTo(maskingScreen, mask);

		IplImage dummyScreen(maskingScreen); // 이진화 처리를 위한 더미 스크린 영상

		// 카메라 영상을 이진화 한다.
		if (!binImage) {
			binImage = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
			binOutput = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
		}

		cvCvtColor(&dummyScreen, binImage, CV_RGB2GRAY); // 컬러를 흑백으로 변환
		// 영상의 각 픽셀(x,y) 값이 threshold 값의 초과는 255 로, 그 이하는 0 으로 변환
		cvThreshold(binImage, binOutput, threshold, 255, CV_THRESH_BINARY);
		binOutput->origin = dummyScreen.origin; // 방향이 뒤집어 진것을 바로 잡아 줌

		cvShowImage("HUV05-camera", &dummyScreen);
		cvShowImage("HUV05-binarization", binOutput);
		
		
		//--------------------------------------------------------------------------
		// 스크린 영역 Rect 출력
		Mat dst(camera);
		vector<Point> rectLines = g_screen.GetScreenContour();
		line(dst, rectLines[0], rectLines[1], Scalar(255, 0, 0), 1);
		line(dst, rectLines[1], rectLines[2], Scalar(255, 0, 0), 1);
		line(dst, rectLines[2], rectLines[3], Scalar(255, 0, 0), 1);
		line(dst, rectLines[3], rectLines[0], Scalar(255, 0, 0), 1);
		//imshow("HUV05-screen", dst);


		//----------------------------------------------------------------------------------------------
		// 스크린 이미지에서 포인터를 찾는다.
		SimpleBlobDetector::Params params;

		// Change thresholds
		params.minThreshold = 1;
		params.maxThreshold = 20;

		// Filter by Area.
		params.filterByArea = true;
		//params.minArea = 1500;
		params.minArea = 10;
		params.maxArea = 200;

		// Filter by Circularity
		params.filterByCircularity = true;
		params.minCircularity = 0.1f;

		// Filter by Convexity
		params.filterByConvexity = true;
		params.minConvexity = 0.87f;

		// Filter by Inertia
		params.filterByInertia = true;
		params.minInertiaRatio = 0.01f;

		SimpleBlobDetector detector(params);

		// Detect blobs.
		std::vector<KeyPoint> keypoints;
		detector.detect(binOutput, keypoints);

		int posX = 0, posY = 0;
		if (!keypoints.empty())
		{
// 			const int screenW = approx[2].x - approx[0].x;
// 			const int screenH = approx[1].y - approx[0].y;
// 
// 			int newPosX = ((int)keypoints[0].pt.x - approx[0].x);
// 			int newPosY = ((int)keypoints[0].pt.y - approx[0].y);
// 
// 			newPosX = max(newPosX, 0);
// 			newPosX = min(screenW, newPosX);
// 			newPosY = max(newPosY, 0);
// 			newPosY = min(screenH, newPosY);
// 
// 			posX = newPosX;
// 			posY = newPosY;

			recogPointPos = g_screen.GetPointPos(keypoints[0]);
			if (recogPointPos != Point(0,0))
			{
				Point err = Point((int)pointPos.x, (int)pointPos.y) - recogPointPos;
				g_screen.SetMappingError(keypoints[0], err);
				printf("error %d, %d\n", err.x, err.y);
			}
		}

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
		Mat im_with_keypoints;
		drawKeypoints(dst, keypoints, im_with_keypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("HUV05-screen", im_with_keypoints);

		if (cvWaitKey(10) >= 0)
			break;
	}


	cvDestroyWindow("HUV05-camera");
	cvDestroyWindow("HUV05-binarization");
	cvDestroyWindow("HUV05-screen");
	cvDestroyWindow("Screen");
}

