
#include "global.h"
#include "screen.h"

using namespace cv;

cScreen g_screen;


cScreen::cScreen() : 
	m_oldT(0)
{
}

cScreen::~cScreen()
{
	cvDestroyWindow("Screen");
}


// 스크린 해상도와 영상에서 인식된 스크린의 크기를 설정한다.
void cScreen::Init(const cv::Rect &screenResolution, const vector<cv::Point> &screenContour)
{
	m_screenResolution = screenResolution;
	m_screenContour = screenContour;

	namedWindow("Screen", 0);
	cvResizeWindow("Screen", screenResolution.width, screenResolution.height);

	m_screen = Mat(screenResolution.height, screenResolution.width, CV_8UC(3));
	m_screen.setTo(Scalar(255, 255, 255));
	imshow("Screen", m_screen);
//	namedWindow("Screen", CV_WINDOW_NORMAL);
//	cvSetWindowProperty("Screen", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
}


// 스크린 해상도 설정.
void cScreen::InitResolution(const cv::Rect &screenResolution)
{
	m_screenResolution = screenResolution;

// 	namedWindow("Screen", 0);
// 	cvResizeWindow("Screen", screenResolution.width, screenResolution.height);
	namedWindow("Screen", CV_WINDOW_NORMAL);
	cvSetWindowProperty("Screen", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	m_screen = Mat(screenResolution.height, screenResolution.width, CV_8UC(3));
	m_screen.setTo(Scalar(255, 255, 255));
	imshow("Screen", m_screen);
}


// 화면에 주사하는 스크린의 해상도의 너비를 리턴한다.
int cScreen::GetWidth()
{
	return m_screenResolution.width;
}


// 화면에 주사하는 스크린의 해상도의 높이를 리턴한다.
int cScreen::GetHeight()
{
	return m_screenResolution.height;
}


// 영상에서 인식된 스크린의 너비를 리턴한다.
int cScreen::GetRecogWidth()
{
	return m_screenContour[1].x - m_screenContour[0].x;
}


// 영상에서 인식된 스크린의 높이를 리턴한다.
int cScreen::GetRecogHeight()
{
	return m_screenContour[3].y - m_screenContour[0].y;
}


// 화면 해상도 와 실제 인식한 영상간의 비율을 리턴한다.
// width, height 순서대로 리턴한다.
pair<float, float> cScreen::GetResolutionRecognitionRate()
{
	const float rate = (float)(GetRecogWidth() * GetRecogHeight()) / (float)(GetWidth() * GetHeight());
	return pair<float, float>(rate, rate);
}


// 체스보드 패턴을 출력한다.
void cScreen::ShowChessBoard(const int rows, const int cols)
{
	m_screen.setTo(Scalar(255, 255, 255));
	horizLines(m_screen, m_screenResolution.width, m_screenResolution.height, 5);
	imshow("Screen", m_screen);
}


// 카메라가 스크린을 향해 있어야 한다.
// 스크린을 인식해서, 스크린 크기 정보를 저장한다.
// minScreenArea : 최소 스크린 넓이.
// 스크린을 찾으면 m_screenContour 값이 업데이트 된다.
void cScreen::DetectScreen(cCamera &camera0, const float minScreenArea)
{
	int threshold = 55; // 임계값(Threshold) 설정
	bool detectScreen = false;
	int detectScreenCount = 0;
	std::vector<cv::Point> screenLines(4);
	Rect screenRect;

	IplImage *camera = 0;
	IplImage *binImage = 0;
	IplImage *binOutput = 0;
	cvNamedWindow("HUV05-screen", 0);
	cvNamedWindow("HUV05-binarization", 0);
	cvResizeWindow("HUV05-binarization", 640, 480);
	cvResizeWindow("HUV05-screen", 640, 480);
	cvMoveWindow("HUV05-binarization", -660, 500);
	cvMoveWindow("HUV05-screen", -1320, 500);
	cvCreateTrackbar("Threshold", "HUV05-binarization", &threshold, 255, NULL);

	while (1) {
		Mat &camera1 = camera0.ShowCaptureUndistortion();
		if (camera1.empty())
			continue;
	
		IplImage camera2(camera1);
		camera = &camera2;


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
				if (vtc == 4 && mincos >= -0.2f && maxcos <= 0.8f)
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
				}
			}
		}


		if (!detectScreen)
		{
			if (rectIndex >= 0)
			{
				++detectScreenCount;

				// 안정적으로 스크린 위치를 찾았다면, threshold를 변경하는 것을 멈춘다.				
				if (detectScreenCount > 10)
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

	cvDestroyWindow("HUV05-binarization");
	cvDestroyWindow("HUV05-screen");


 	m_screenContour = screenLines;
}


// 비뚤어진 화면을 바로 잡는 행렬을 리턴한다.
Mat cScreen::SkewTransform(const int width, const int height)
{
	vector<Point2f> quad(4);
	quad[0] = Point2f(0, 0);
	quad[1] = Point2f((float)width, (float)0);
	quad[2] = Point2f((float)width, (float)height);
	quad[3] = Point2f((float)0, (float)height);

	vector<Point2f> screen(4);
	screen[0] = Point2f((float)m_screenContour[0].x, (float)m_screenContour[0].y);
	screen[1] = Point2f((float)m_screenContour[1].x, (float)m_screenContour[1].y);
	screen[2] = Point2f((float)m_screenContour[2].x, (float)m_screenContour[2].y);
	screen[3] = Point2f((float)m_screenContour[3].x, (float)m_screenContour[3].y);

	Mat tm = getPerspectiveTransform(screen, quad);

	return tm;
}


// 스크린 안에서 볼이 움직이는 애니메이션을 한다.
void cScreen::ShowPingpong(const cv::Point2f &ballPos, const cv::Point2f &velocity, const cv::Size2f &ballSize)
{
	const int w = m_screenResolution.width;
	const int h = m_screenResolution.height;

	// 포인터 움직임.
	const int curT = timeGetTime();
	float incT = (float)(curT - m_oldT) / 1000.f;
	if (incT > 1000.f)
	{
		// 처음 시작된 경우.
		incT = 1.f;
		m_ballPos = ballPos;
		m_ballVelocity = velocity;
	}
	else if (incT > 0.1f)
	{
		// 시간 지연이 있었을 경우.
		incT = 0.1f;
	}

	m_oldT = curT;


	const Point2f pointCurPos = m_ballPos + m_ballVelocity * incT;
	if (pointCurPos.x - ballSize.width/2 < 0 && (m_ballVelocity.x < 0))
		m_ballVelocity.x = -m_ballVelocity.x;
	else if (pointCurPos.x + ballSize.width/2 > w && (m_ballVelocity.x > 0))
		m_ballVelocity.x = -m_ballVelocity.x;
	if (pointCurPos.y - ballSize.height/2 < 0 && (m_ballVelocity.y < 0))
		m_ballVelocity.y = -m_ballVelocity.y;
	else if (pointCurPos.y + ballSize.height/2 > h && (m_ballVelocity.y > 0))
		m_ballVelocity.y = -m_ballVelocity.y;
	m_ballPos = pointCurPos;

	m_screen.setTo(Scalar(255, 255, 255));
	//ShowBoxLines(10, 10, false);
	ellipse(m_screen, RotatedRect(pointCurPos, ballSize, 0), Scalar(0, 0, 0), CV_FILLED);
	imshow("Screen", m_screen);
}


// 격자무늬 선을 그린다.
void cScreen::ShowBoxLines(const int rows, const int cols, const bool isClear)
{
	const int w = m_screenResolution.width;
	const int h = m_screenResolution.height;
	const int cx = w / cols;
	const int cy = h / rows;

	if (isClear)
		m_screen.setTo(Scalar(255, 255, 255));

	for (int x = 0; x <= w; x+=cx)
	{
		line(m_screen, Point(x, 0), Point(x, h), Scalar(0, 0, 0));
	}

	for (int y = 0; y <= h; y += cy)
	{
		line(m_screen, Point(0, y), Point(w, y), Scalar(0, 0, 0));
	}
}
