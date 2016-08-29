
#include "global.h"


using namespace cv;

#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#pragma comment(lib, "opencv_features2d2411d.lib")
//#pragma comment(lib, "opencv_nonfree2411d.lib")
#pragma comment(lib, "winmm.lib")


extern int g_screenThreshold = 128;



/**
* Helper function to find a cosine of angle between vectors
* from pt0->pt1 and pt0->pt2
*/
double angle(cv::Point pt1, const cv::Point &pt2, const cv::Point &pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
* Helper function to display text in the center of a contour
*/
void setLabel(cv::Mat& im, const std::string &label, const std::vector<cv::Point>& contour, const cv::Scalar &color)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, color, thickness, 8);
}


void horizLines(Mat &image, const int w, const int h, const int div)
{
	const bool startBlank = true;
	const int h2 = h / (div * 2);
	int yOffset = 0;

	for (int k = 0; k < div; ++k)
	{
		yOffset = k * h2 * 2;
		if (startBlank)
			yOffset += h2;

		for (int i = 0; i < h2; ++i)
			line(image, Point(0, i + yOffset), Point(w, i + yOffset), Scalar(0, 0, 0));
	}
}


void verticalLines(Mat &image, const int w, const int h, const int div)
{
	const bool startBlank = true;
	const int w2 = w / (div * 2);
	int xOffset = 0;

	for (int k = 0; k < div; ++k)
	{
		xOffset = k * w2 * 2;
		if (startBlank)
			xOffset += w2;

		for (int i = 0; i < w2; ++i)
			line(image, Point(i + xOffset, 0), Point(i + xOffset, h), Scalar(0, 0, 0));
	}
}


// 체스보드 출력
void chessboard(cv::Mat &image, const int w, const int h, const int rows, const int cols)
{
	
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

	// 시계방향으로 left top 부터 순서대로 0, 1, 2, 3 위치에 포인트가 저장된다.
	for (int k = 0; k < 4; ++k)
	{
		if ((src[k].x < center.x) && (src[k].y < center.y))
			dst[0] = src[k];
		if ((src[k].x < center.x) && (src[k].y > center.y))
			dst[3] = src[k];
		if ((src[k].x > center.x) && (src[k].y < center.y))
			dst[1] = src[k];
		if ((src[k].x > center.x) && (src[k].y > center.y))
			dst[2] = src[k];
	}
}


// contours 를 x가 작은 순서대로 정렬한다.
// 0 --- 1 --- 2 --- N
void OrderedLineContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst)
{
	dst = src;
	for (u_int i = 0; i < dst.size()-1; ++i)
	{
		for (u_int k = i + 1; k < dst.size(); ++k)
		{
			if (dst[i].x > dst[k].x)
			{
				Point tmp = dst[i];
				dst[i] = dst[k];
				dst[k] = tmp;
			}
		}
	}
}


// 선을 그린다.
void DrawLines(Mat &dst, const vector<cv::Point> &lines, const cv::Scalar &color, const int thickness,
	const bool isLoop)
{
	if (lines.size() < 2)
		return;

	for (u_int i = 0; i < lines.size() - 1; ++i)
		line(dst, lines[i], lines[i+1], color, thickness);

	if (isLoop)
		line(dst, lines[lines.size()-1], lines[0], color, thickness);
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


// Skew 변환 행렬을 계산해서 리턴한다.
// scale 값 만큼 contours를 늘려 잡는다.
Mat SkewTransform(vector<Point> &contours, const int width, const int height, const float scale)
{
	vector<Point2f> quad(4);
	quad[0] = Point2f(0, 0);
	quad[1] = Point2f((float)width, (float)0);
	quad[2] = Point2f((float)width, (float)height);
	quad[3] = Point2f((float)0, (float)height);


	Point2f ip; // 중점
	if (!GetIntersectPoint(contours[0], contours[2], contours[3], contours[1], &ip))
		return Mat();

	const Point2f offset1 = ((Point2f)contours[0] - ip) * (scale - 1.f);
	const Point2f offset2 = ((Point2f)contours[1] - ip) * (scale - 1.f);
	const Point2f offset3 = ((Point2f)contours[2] - ip) * (scale - 1.f);
	const Point2f offset4 = ((Point2f)contours[3] - ip) * (scale - 1.f);

// 	const Point2f v1 = contours[0] - contours[2];
// 	const Point2f v2 = contours[1] - contours[3];
// 	const Point2f offset1 = v1 * (scale - 1.f);
// 	const Point2f offset2 = v2 * (scale - 1.f);

	vector<Point2f> screen(4);
	screen[0] = (Point2f)contours[0] + offset1;
	screen[1] = (Point2f)contours[1] + offset2;
	screen[2] = (Point2f)contours[2] + offset3;
	screen[3] = (Point2f)contours[3] + offset4;

 	const Mat tm = getPerspectiveTransform(screen, quad);
	return tm;
}

