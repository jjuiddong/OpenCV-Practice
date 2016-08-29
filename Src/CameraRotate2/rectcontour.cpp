
#include "global.h"
#include "rectcontour.h"


using namespace cv;


cRectContour::cRectContour() :
	m_contours(4)
{

}

cRectContour::cRectContour(const vector<cv::Point> &contours) :
	m_contours(4)
{
	Init(contours);
}

cRectContour::cRectContour(const vector<cv::KeyPoint> &contours) :
	m_contours(4)
{
	Init(contours);
}


cRectContour::cRectContour(const cv::Point &center, const float size) :
	m_contours(4)
{
	Init(center, size);
}

cRectContour::~cRectContour()
{

}


// 초기화.
bool cRectContour::Init(const vector<cv::Point> &contours)
{
	OrderedContours(contours, m_contours);
	return true;
}


// 초기화.
bool cRectContour::Init(const vector<cv::KeyPoint> &keypoints)
{
	if (keypoints.size() < 4)
		return false;

	vector<Point> keys(4);
	for (u_int i = 0; i < keypoints.size(); ++i)
		keys[i] = Point(keypoints[i].pt);

	OrderedContours(keys, m_contours);

	return true;
}


// 중점 center를 중심으로 size만큼 키워진 사각형을 만든다.
//
// 0 -------- 1
// |          |
// |    +     |
// |          |
// 3 -------- 2
//
bool cRectContour::Init(const cv::Point &center, const float size)
{
	m_contours[0] = center + Point(-(int)(size / 2.f), -(int)(size / 2.f));
	m_contours[1] = center + Point((int)(size / 2.f), -(int)(size / 2.f));
	m_contours[2] = center + Point((int)(size / 2.f), (int)(size / 2.f));
	m_contours[3] = center + Point(-(int)(size / 2.f), (int)(size / 2.f));
	return true;
}


// 박스 출력.
void cRectContour::Draw(cv::Mat &dst, const cv::Scalar &color, const int thickness)
// color = cv::Scalar(0, 0, 0), thickness = 1
{
	DrawLines(dst, m_contours, color, thickness);
}


// 사각형의 중점을 리턴한다.
Point cRectContour::Center() const
{
	Point center;
	for each (auto &pt in m_contours)
		center += pt;
	
	center = Point(center.x / m_contours.size(), center.y / m_contours.size());
	return center;
}


// 사각형의 중점을 중심으로 스케일한다.
void cRectContour::ScaleCenter(const float scale)
{
	const Point center = Center();

	for (u_int i=0; i < m_contours.size(); ++i)
	{
		m_contours[i] = center + ((m_contours[i] - center) * scale);
	}
}
