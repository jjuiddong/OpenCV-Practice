
//
// 2016-03-16, jjuiddong
//
// 마우스 왼쪽 버튼 클릭 4번으로 외곽 사각형 꼭지점을 지정한다.
// 4개가 맞춰지면 perspective transform 을 이용해 화면을 변환 한다.
// 마우스 오른쪽 버튼을 클릭하여, 원본 이미지 포인트에서, 변환된 포인트를 확인할 수 있다.
//
//

#include<iostream>
#include<opencv2/opencv.hpp>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")

#include "vector3.h"


using namespace std;
using namespace cv;

#define OUT

void MouseClickHandler(int event, int x, int y, int flags, void* userdata);
bool GetIntersectPoint(const Point2f& AP1, const Point2f& AP2, const Point2f& BP1, const Point2f& BP2, Point2f* IP);
void OrderedContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst);
bool OrderedContours2(const vector<cv::Point> &src, OUT vector<cv::Point> &dst);


vector<Point> g_mouseClickPos;
Point g_camPos(0,0);
Mat g_perspectivMat;


int main(int argc, char *argv[])
{
	namedWindow("MouseClick");
	cvResizeWindow("MouseClick", 640, 480);
	setMouseCallback("MouseClick", MouseClickHandler, NULL);

	Mat mouseMat(480, 640, CV_8UC3);
	Mat dest(480, 640, CV_8UC3);

	while (1)
	{
		Point oldPos(0,0);
		mouseMat.setTo(Scalar(200, 200, 200));
		for (unsigned int i = 0; i < g_mouseClickPos.size(); ++i)
		{
			const Point pos = g_mouseClickPos[i];
			circle(mouseMat, pos, 10, Scalar(0, 0, 0), CV_FILLED);
		}

		if (g_mouseClickPos.size() == 4)
		{
			vector<Point> contour(4);
			OrderedContours(g_mouseClickPos, contour);

			line(mouseMat, contour[0], contour[1], Scalar(0, 0, 0), 2);
			line(mouseMat, contour[1], contour[2], Scalar(0, 0, 0), 2);
			line(mouseMat, contour[2], contour[3], Scalar(0, 0, 0), 2);
			line(mouseMat, contour[3], contour[0], Scalar(0, 0, 0), 2);

			line(mouseMat, contour[0], contour[2], Scalar(0, 0, 0), 2);
			line(mouseMat, contour[1], contour[3], Scalar(0, 0, 0), 2);
		}

		circle(mouseMat, g_camPos, 10, Scalar(0, 100, 0), CV_FILLED);
		imshow("MouseClick", mouseMat);

		if (g_mouseClickPos.size() >= 4)
		{
			namedWindow("Homography");
			cvResizeWindow("Homography", 640, 480);
			dest.setTo(Scalar(200, 200, 200));
			cv::warpPerspective(mouseMat, dest, g_perspectivMat, dest.size());


			std::vector<Point2f> camera_corners;
			camera_corners.push_back(g_camPos);
			std::vector<Point2f> world_corners;
			perspectiveTransform(camera_corners, world_corners, g_perspectivMat);

			circle(dest, world_corners[0], 10, Scalar(0, 0, 100), CV_FILLED);

			imshow("Homography", dest);
		}

		if (waitKey(1) == 27) break;
	}
	return 0;
}



void MouseClickHandler(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		static int mouseIdx = 0;
		if (mouseIdx < 4)
		{
			g_mouseClickPos.push_back(Point(x, y));
		}
		else
		{
			g_mouseClickPos[mouseIdx % 4] = Point(x, y);
		}

		if (g_mouseClickPos.size() >= 4)
		{
			const int width = 640;
			const int height = 480;
			vector<Point2f> quad(4);
			quad[0] = Point2f(0, 0);
			quad[1] = Point2f((float)width, (float)0);
			quad[2] = Point2f((float)width, (float)height);
			quad[3] = Point2f((float)0, (float)height);

			vector<Point> contour(4);
			OrderedContours(g_mouseClickPos, contour);

			Point2f ip; // 중점
			if (!GetIntersectPoint(contour[0], contour[2], contour[3], contour[1], &ip))
			{
				OrderedContours2(g_mouseClickPos, contour);
			}

			vector<Point> contour2(4);
			OrderedContours2(g_mouseClickPos, contour2);

			const float scale = 1.f;
			const Point2f offset1 = ((Point2f)contour[0] - ip) * (scale - 1.f);
			const Point2f offset2 = ((Point2f)contour[1] - ip) * (scale - 1.f);
			const Point2f offset3 = ((Point2f)contour[2] - ip) * (scale - 1.f);
			const Point2f offset4 = ((Point2f)contour[3] - ip) * (scale - 1.f);

			vector<Point2f> screen(4);
			screen[0] = (Point2f)contour[0] + offset1;
			screen[1] = (Point2f)contour[1] + offset2;
			screen[2] = (Point2f)contour[2] + offset3;
			screen[3] = (Point2f)contour[3] + offset4;

			g_perspectivMat = getPerspectiveTransform(screen, quad);
		}

		++mouseIdx;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		g_camPos = Point(x, y);
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}

}


// hhttp://www.gisdeveloper.co.kr/15
// AP1 - AP2 를 지나는 직선
// AP3 - AP4 를 지나는 직선
// 두 직선의 교점을 찾아 IP에 저장한다.
// 교점이 없다면 false를 리턴한다.
bool GetIntersectPoint(const Point2f& AP1, const Point2f& AP2,
	const Point2f& BP1, const Point2f& BP2, Point2f* IP)
{
	float t;
	float s;
	float under = (BP2.y - BP1.y)*(AP2.x - AP1.x) - (BP2.x - BP1.x)*(AP2.y - AP1.y);
	if (under == 0) return false;

	float _t = (BP2.x - BP1.x)*(AP1.y - BP1.y) - (BP2.y - BP1.y)*(AP1.x - BP1.x);
	float _s = (AP2.x - AP1.x)*(AP1.y - BP1.y) - (AP2.y - AP1.y)*(AP1.x - BP1.x);

	t = _t / under;
	s = _s / under;

	if (t<0.0 || t>1.0 || s<0.0 || s>1.0) return false;
	if (_t == 0 && _s == 0) return false;

	IP->x = AP1.x + t * (float)(AP2.x - AP1.x);
	IP->y = AP1.y + t * (float)(AP2.y - AP1.y);

	return true;
}



// contours 를 순서대로 정렬한다.
// 0 ---------- 1
// |            |
// |            |
// |            |
// 2 ---------- 3
void OrderedContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst)
{
	// 가장 큰 박스를 찾는다.
	Point center = src[0];
	center += src[1];
	center += src[2];
	center += src[3];

	// 중심점 계산
	center.x /= 4;
	center.y /= 4;

	vector<bool> check(4, false);
	vector<bool> check2(4, false);

	// 시계방향으로 left top 부터 순서대로 0, 1, 2, 3 위치에 포인트가 저장된다.
	for (int k = 0; k < 4; ++k)
	{
		if (!check[0] && (src[k].x < center.x) && (src[k].y < center.y))
		{
			dst[0] = src[k];
			check[0] = true;
			check2[k] = true;
		}
		if (!check[3] && (src[k].x < center.x) && (src[k].y > center.y))
		{
			dst[3] = src[k];
			check[3] = true;
			check2[k] = true;
		}
		if (!check[1] && (src[k].x > center.x) && (src[k].y < center.y))
		{
			dst[1] = src[k];
			check[1] = true;
			check2[k] = true;
		}
		if (!check[2] && (src[k].x > center.x) && (src[k].y > center.y))
		{
			dst[2] = src[k];
			check[2] = true;
			check2[k] = true;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		if (!check[i])
		{
			for (int k = 0; k < 4; ++k)
			{
				if (!check2[k])
				{
					check[i] = true;
					check2[k] = true;
					dst[i] = src[k];
				}
			}
		}
	}

}


// contours 를 순서대로 정렬한다.
// 0 ---------- 1
// |				    |
// |					|
// |					|
// 3 ---------- 2
bool OrderedContours2(const vector<cv::Point> &src, OUT vector<cv::Point> &dst)
{
	//--------------------------------------------------------------------
	// 4 point cross check
	int crossIndices[3][4] =
	{
		{ 0, 1, 2, 3 },
		{ 0, 2, 1, 3 },
		{ 0, 3, 1, 2 },
	};

	int crossIdx = -1;
	for (int i = 0; i < 3; ++i)
	{
		// line1 = p1-p2 
		// line2 = p3-p4
		const cv::Point p1 = src[crossIndices[i][0]];
		const cv::Point p2 = src[crossIndices[i][1]];
		const cv::Point p3 = src[crossIndices[i][2]];
		const cv::Point p4 = src[crossIndices[i][3]];

		cv::Point2f tmp;
		if (GetIntersectPoint(p1, p2, p3, p4, &tmp))
		{
			crossIdx = i;
			break;
		}
	}

	if (crossIdx < 0)
		return false;

	// p1 ------ p2
	// |              |
	// |              |
	// |              |
	// p4 ------ p3
	const cv::Point p1 = src[crossIndices[crossIdx][0]];
	const cv::Point p2 = src[crossIndices[crossIdx][2]];
	const cv::Point p3 = src[crossIndices[crossIdx][1]];
	const cv::Point p4 = src[crossIndices[crossIdx][3]];


	//--------------------------------------------------------------------
	// 중점 계산
	// 가장 큰 박스를 찾는다.
	Point center = p1;
	center += p2;
	center += p3;
	center += p4;

	// 중심점 계산
	center.x /= 4;
	center.y /= 4;

	Vector3 v1 = Vector3((float)p1.x, (float)p1.y, 0) - Vector3((float)center.x, (float)center.y, 0);
	v1.Normalize();
	Vector3 v2 = Vector3((float)p2.x, (float)p2.y, 0) - Vector3((float)center.x, (float)center.y, 0);
	v2.Normalize();

	// 0 ---------- 1
	// |				    |
	// |					|
	// |					|
	// 3 ---------- 2
	const Vector3 crossV = v1.CrossProduct(v2);
	if (crossV.z > 0)
	{
		dst[0] = p1;
		dst[1] = p2;
		dst[2] = p3;
		dst[3] = p4;
	}
	else
	{
		dst[0] = p2;
		dst[1] = p1;
		dst[2] = p4;
		dst[3] = p3;
	}

	return true;
}
