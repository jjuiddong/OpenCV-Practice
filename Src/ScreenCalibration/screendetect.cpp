
#include "global.h"
#include "screendetect.h"

using namespace cv;



// 스크린을 인식해서 크기와 포인트를 리턴한다.
// 만약 인식하지 못했다면 Rect(0,0,0,0) 을 리턴한다.
// isManualMode : true 이면, threshold 값을 직접 지정한다.
//				  false 이면, 자동으로 설정된다.
Rect ScreenDetectAuto(std::vector<cv::Point> &out, const int minScreenArea, OUT int &outThreshold)
{
	Mat screen(g_screen.GetHeight(), g_screen.GetWidth(), CV_8UC(3));
	screen.setTo(Scalar(255, 255, 255));
	namedWindow("Screen", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Screen", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	const bool startBlank = true;
	const int w = g_screen.GetWidth();
	const int h = g_screen.GetHeight();
	const int div = 32;
	imshow("Screen", screen);

	//-----------------------------------------------------------------------------------------
	// 카메라 영상 정보 출력.
	int threshold = 33; // 임계값(Threshold) 설정
	bool upThreashold = false;
	bool detectScreen = false;
	int detectScreenCount = 0;
	int detectThreshold = threshold;
	std::vector<cv::Point> screenLines(4);
	Rect screenRect;

	IplImage *camera = 0;
	IplImage *binImage = 0;
	IplImage *binOutput = 0;
	cvNamedWindow("HUV05-camera", 0);
	cvNamedWindow("HUV05-binarization", 0);
	cvNamedWindow("HUV05-screen", 0);

	cvResizeWindow("HUV05-camera", 640, 480);
	cvResizeWindow("HUV05-binarization", 640, 480);
	cvResizeWindow("HUV05-screen", 640, 480);

	cvMoveWindow("HUV05-camera", -660, 0);
	cvMoveWindow("HUV05-binarization", -660, 500);
	cvMoveWindow("HUV05-screen", -1320, 500);
	cvCreateTrackbar("Threshold", "HUV05-camera", &threshold, 255, NULL); // "HUV05-camera" 윈도우에 bar 를 붙인다. 

	while (1) {
		camera = g_camera.GetCapture();
		if (!camera)
			continue;

		// 카메라 영상을 이진화 한다.
		if (!binImage) {
			binImage = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
			binOutput = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
		}

		cvCvtColor(camera, binImage, CV_RGB2GRAY); // 컬러를 흑백으로 변환
		// 영상의 각 픽셀(x,y) 값이 threshold 값의 초과는 255 로, 그 이하는 0 으로 변환
		cvThreshold(binImage, binOutput, threshold, 255, CV_THRESH_BINARY);
		binOutput->origin = camera->origin; // 방향이 뒤집어 진것을 바로 잡아줌


		if (cvWaitKey(10) >= 0)
			break;

		cvShowImage("HUV05-camera", camera);
		cvShowImage("HUV05-binarization", binOutput);
		//imshow("HUV05-binarization", bw);


		//----------------------------------------------------------------------------------------------
		// 외곽선 뽑아내기.
		Mat src(camera);

		// Use Canny instead of threshold to catch squares with gradient shading
		cv::Mat bw;
		cv::Canny(Mat(binOutput), bw, 0, 50, 5);

		// Find contours
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		std::vector<cv::Point> approx;
		cv::Mat dst(camera);// src.clone();

		int rectIndex = -1;
		bool findMask = false;
		Rect bigScreen;

		for (u_int i = 0; i < contours.size(); i++)
		{
			// Approximate contour with accuracy proportional
			// to the contour perimeter
			cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

			// Skip small or non-convex objects 
			if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
				continue;

			if (approx.size() == 4) // 사각형만 찾는다.
			{
				// Number of vertices of polygonal curve
				int vtc = approx.size();

				// Get the cosines of all corners
				std::vector<double> cos;
				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

				// Sort ascending the cosine values
				std::sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4 && mincos >= -0.1f && maxcos <= 0.8f)
				{
					setLabel(dst, "RECT", contours[i]);

					line(dst, approx[0], approx[1], Scalar(255, 0, 0), 2);
					line(dst, approx[1], approx[2], Scalar(255, 0, 0), 2);
					line(dst, approx[2], approx[3], Scalar(255, 0, 0), 2);
					line(dst, approx[3], approx[0], Scalar(255, 0, 0), 2);

					// 가장 큰 박스를 찾는다.
					Point center = approx[0];
					center += approx[1];
					center += approx[2];
					center += approx[3];

					// 중심점 계산
					center.x /= 4;
					center.y /= 4;

					// 시계방향으로 left top 부터 순서대로 0, 1, 2, 3 위치에 포인트가 저장된다.
					for (int k = 0; k < 4; ++k)
					{
						if ((approx[k].x < center.x) && (approx[k].y < center.y))
							screenLines[0] = approx[k];
						if ((approx[k].x < center.x) && (approx[k].y > center.y))
							screenLines[3] = approx[k];
						if ((approx[k].x > center.x) && (approx[k].y < center.y))
							screenLines[1] = approx[k];
						if ((approx[k].x > center.x) && (approx[k].y > center.y))
							screenLines[2] = approx[k];
					}

					Rect rect((int)screenLines[0].x, (int)screenLines[0].y,
						(int)(screenLines[1].x - screenLines[0].x),
						(int)(screenLines[2].y - screenLines[0].y));

					if ((rect.width * rect.height) > (bigScreen.width * bigScreen.height))
					{
						bigScreen = rect;

						// bigScreen 크기가 어느정도 커야 한다. 작다면 찾지 못한걸로 간주한다.
						if (rect.width * rect.height > minScreenArea)
							rectIndex = i;
					}
					//break;
				}
			}
		}

		// 박스를 찾지 못했다면, threshold를 낮춘다.
		if (!detectScreen && (rectIndex < 0))
		{
			if (upThreashold)
			{
				threshold = min(threshold + 1, 255);
				if (threshold >= 255)
					upThreashold = false;
			}
			else
			{
				threshold = max(threshold - 1, 0);
				if (threshold <= 0)
					upThreashold = true;
			}

			setTrackbarPos("Threshold", "HUV05-camera", threshold);
		}

		// 박스를 찾고, threshold 값이 일정하다면, detectScreenCount를 증가시킨다.
		if (!detectScreen)
		{
			if ((rectIndex >= 0) && (threshold == detectThreshold))
			{
				++detectScreenCount;

				// 안정적으로 스크린 위치를 찾았다면, threshold를 변경하는 것을 멈춘다.				
				if (detectScreenCount > 30)
				{
					detectScreen = true;
					screenRect = bigScreen;
				}
			}
			else
			{
				detectScreenCount = 1;
			}

			// 박스를 찾았다면, 현재 threshold 값을 저장한다.
			if (rectIndex >= 0)
			{
				detectThreshold = threshold;
			}
		}

		if (detectScreen)
			break;

		imshow("HUV05-screen", dst);
	}


	cvDestroyWindow("HUV05-camera");
	cvDestroyWindow("HUV05-binarization");
	cvDestroyWindow("HUV05-screen");
	cvDestroyWindow("Screen");

	out = screenLines;
	outThreshold = threshold;
	return screenRect;
}



// 수동을 스크린을 찾는다.
cv::Rect ScreenDetectManual(std::vector<Point> &out,
	const int minScreenArea, OUT int &outThreshold)
{
	Mat screen(g_screen.GetHeight(), g_screen.GetWidth(), CV_8UC(3));
	screen.setTo(Scalar(255, 255, 255));
	namedWindow("Screen", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Screen", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	const bool startBlank = true;
	const int w = g_screen.GetWidth();
	const int h = g_screen.GetHeight();
	const int div = 32;
	imshow("Screen", screen);

	//-----------------------------------------------------------------------------------------
	// 카메라 영상 정보 출력.
	int threshold = 33; // 임계값(Threshold) 설정
	bool detectScreen = false;
	int detectScreenCount = 0;
	std::vector<cv::Point> screenLines(4);
	Rect screenRect;

	IplImage *camera = 0;
	IplImage *binImage = 0;
	IplImage *binOutput = 0;
	cvNamedWindow("HUV05-camera", 0);
	cvNamedWindow("HUV05-binarization", 0);
	cvNamedWindow("HUV05-screen", 0);

	cvResizeWindow("HUV05-camera", 640, 480);
	cvResizeWindow("HUV05-binarization", 640, 480);
	cvResizeWindow("HUV05-screen", 640, 480);

	cvMoveWindow("HUV05-camera", -660, 0);
	cvMoveWindow("HUV05-binarization", -660, 500);
	cvMoveWindow("HUV05-screen", -1320, 500);
	cvCreateTrackbar("Threshold", "HUV05-camera", &threshold, 255, NULL); // "HUV05-camera" 윈도우에 bar 를 붙인다. 

	while (1) {
		camera = g_camera.GetCapture();
		if (!camera)
			continue;

		// 카메라 영상을 이진화 한다.
		if (!binImage) {
			binImage = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
			binOutput = cvCreateImage({ 640, 480 }, IPL_DEPTH_8U, 1); // 흑백 이미지 생성
		}

		cvCvtColor(camera, binImage, CV_RGB2GRAY); // 컬러를 흑백으로 변환
		// 영상의 각 픽셀(x,y) 값이 threshold 값의 초과는 255 로, 그 이하는 0 으로 변환
		cvThreshold(binImage, binOutput, threshold, 255, CV_THRESH_BINARY);
		binOutput->origin = camera->origin; // 방향이 뒤집어 진것을 바로 잡아줌


		if (cvWaitKey(10) >= 0)
			break;

		cvShowImage("HUV05-camera", camera);
		cvShowImage("HUV05-binarization", binOutput);
		//imshow("HUV05-binarization", bw);


		//----------------------------------------------------------------------------------------------
		// 외곽선 뽑아내기.
		Mat src(camera);

		// Use Canny instead of threshold to catch squares with gradient shading
		cv::Mat bw;
		cv::Canny(Mat(binOutput), bw, 0, 50, 5);

		// Find contours
		std::vector<std::vector<cv::Point> > contours;
		cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		std::vector<cv::Point> approx;
		cv::Mat dst(camera);// src.clone();

		int rectIndex = -1;
		bool findMask = false;
		Rect bigScreen;

		for (u_int i = 0; i < contours.size(); i++)
		{
			// Approximate contour with accuracy proportional
			// to the contour perimeter
			cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

			// Skip small or non-convex objects 
			if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
				continue;

			if (approx.size() == 4) // 사각형만 찾는다.
			{
				// Number of vertices of polygonal curve
				int vtc = approx.size();

				// Get the cosines of all corners
				std::vector<double> cos;
				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(approx[j%vtc], approx[j - 2], approx[j - 1]));

				// Sort ascending the cosine values
				std::sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour
				if (vtc == 4 && mincos >= -0.1f && maxcos <= 0.8f)
				{
					// 가장 큰 박스를 찾는다.
					Point center = approx[0];
					center += approx[1];
					center += approx[2];
					center += approx[3];

					// 중심점 계산
					center.x /= 4;
					center.y /= 4;

					vector<cv::Point> lines(4);
					// 시계방향으로 left top 부터 순서대로 0, 1, 2, 3 위치에 포인트가 저장된다.
					for (int k = 0; k < 4; ++k)
					{
						if ((approx[k].x < center.x) && (approx[k].y < center.y))
							lines[0] = approx[k];
						if ((approx[k].x < center.x) && (approx[k].y > center.y))
							lines[3] = approx[k];
						if ((approx[k].x > center.x) && (approx[k].y < center.y))
							lines[1] = approx[k];
						if ((approx[k].x > center.x) && (approx[k].y > center.y))
							lines[2] = approx[k];
					}

					Rect rect((int)lines[0].x, (int)lines[0].y,
						(int)(lines[1].x - lines[0].x),
						(int)(lines[2].y - lines[0].y));

					if ((rect.width * rect.height) > (bigScreen.width * bigScreen.height))
					{
						bigScreen = rect;

						// bigScreen 크기가 어느정도 커야 한다. 작다면 찾지 못한걸로 간주한다.
						// 최소 200 X 200 으로 간주한다.
						if (rect.width * rect.height > minScreenArea)
						{
							rectIndex = i;
							screenLines = lines;
						}
					}
					//break;
				}
			}
		}


		// 박스를 찾고, threshold 값이 일정하다면, detectScreenCount를 증가시킨다.
		if (!detectScreen)
		{
			if (rectIndex >= 0)
			{
				++detectScreenCount;

				// 안정적으로 스크린 위치를 찾았다면, threshold를 변경하는 것을 멈춘다.				
				if (detectScreenCount > 30)
				{
					detectScreen = true;
					screenRect = bigScreen;
				}
			}
			else
			{
				detectScreenCount = 1;
			}
		}

		if (detectScreen)
			break;


		setLabel(dst, "Screen", screenLines);
		line(dst, screenLines[0], screenLines[1], Scalar(255, 0, 0), 2);
		line(dst, screenLines[1], screenLines[2], Scalar(255, 0, 0), 2);
		line(dst, screenLines[2], screenLines[3], Scalar(255, 0, 0), 2);
		line(dst, screenLines[3], screenLines[0], Scalar(255, 0, 0), 2);


		imshow("HUV05-screen", dst);
	}

	cvWaitKey(1000);


	cvDestroyWindow("HUV05-camera");
	cvDestroyWindow("HUV05-binarization");
	cvDestroyWindow("HUV05-screen");
	cvDestroyWindow("Screen");

	out = screenLines;
	outThreshold = threshold;
	return screenRect;
}


