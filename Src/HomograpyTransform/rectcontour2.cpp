
//#include "stdafx.h"
#include<opencv2/opencv.hpp>
#include "rectcontour2.h"

using namespace cv;

#define OUT


// contours 를 순서대로 정렬한다.
// 0 ---------- 1
// |				    |
// |					|
// |					|
// 3 ---------- 2
//
// chIndices 가 NULL이 아니면, src 인덱스가 바뀐 정보를 리턴한다.
void OrderedContours(const cv::Point src[4], OUT cv::Point dst[4], OUT int *chIndices=NULL)
{
	// 가장 큰 박스를 찾는다.
	Point center = src[0];
	center += src[1];
	center += src[2];
	center += src[3];

	// 중심점 계산
	center.x /= 4;
	center.y /= 4;

	bool check[4] = { false, false, false, false };
	bool check2[4] = { false, false, false, false };

	// 시계방향으로 left top 부터 순서대로 0, 1, 2, 3 위치에 포인트가 저장된다.
	for (int k = 0; k < 4; ++k)
	{
		if (!check[0] && (src[k].x < center.x) && (src[k].y < center.y))
		{
			dst[0] = src[k];
			check[0] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[0] = k;
		}
		if (!check[3] && (src[k].x < center.x) && (src[k].y > center.y))
		{
			dst[3] = src[k];
			check[3] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[3] = k;
		}
		if (!check[1] && (src[k].x > center.x) && (src[k].y < center.y))
		{
			dst[1] = src[k];
			check[1] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[1] = k;
		}
		if (!check[2] && (src[k].x > center.x) && (src[k].y > center.y))
		{
			dst[2] = src[k];
			check[2] = true;
			check2[k] = true;
			if (chIndices)
				chIndices[2] = k;
		}
	}

	// 사각형 꼭지점 중, 설정되지 않는 점이 있다면, 
	// 원본(src)에서 빠진 점을 채운다.
	// 정확한 사각형이 되는 것을 보장하지 않는다.
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
					if (chIndices)
						chIndices[i] = k;

					dst[i] = src[k];
				}
			}
		}
	}
}


void DrawLines(cv::Mat &dst, const cv::Point lines[4], const cv::Scalar &color, const int thickness,
	const bool isLoop=false)
{
	for (u_int i = 0; i < 3; ++i)
		line(dst, lines[i], lines[i + 1], color, thickness);

	if (isLoop)
		line(dst, lines[3], lines[0], color, thickness);
}


cRectContour2::cRectContour2()
{
}

cRectContour2::cRectContour2(const vector<cv::Point> &contours)
{
	Init(contours);
}

cRectContour2::cRectContour2(const vector<cv::KeyPoint> &contours)
{
	Init(contours);
}


cRectContour2::cRectContour2(const cv::Point &center, const float size)
{
	Init(center, size);
}

cRectContour2::~cRectContour2()
{

}


// 초기화.
bool cRectContour2::Init(const cv::Point contours[4])
{
	OrderedContours(contours, m_contours, m_chIndices);
	return true;
}

bool cRectContour2::Init(const vector<cv::Point> &contours)
{
	cv::Point in[4];
	in[0] = contours[0];
	in[1] = contours[1];
	in[2] = contours[2];
	in[3] = contours[3];
	Init(in);
	return true;
}
bool cRectContour2::Init(const vector<cv::Point2f> &contours)
{
	cv::Point ct[4];
	ct[0] = contours[0];
	ct[1] = contours[1];
	ct[2] = contours[2];
	ct[3] = contours[3];
	return Init(ct);
}


// 초기화.
bool cRectContour2::Init(const vector<cv::KeyPoint> &keypoints)
{
	if (keypoints.size() < 4)
		return false;

	Point keys[4];
	for (u_int i = 0; i < keypoints.size(); ++i)
		keys[i] = Point(keypoints[i].pt);

	Init(keys);
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
bool cRectContour2::Init(const cv::Point &center, const float size)
{
	m_contours[0] = center + Point(-(int)(size / 2.f), -(int)(size / 2.f));
	m_contours[1] = center + Point((int)(size / 2.f), -(int)(size / 2.f));
	m_contours[2] = center + Point((int)(size / 2.f), (int)(size / 2.f));
	m_contours[3] = center + Point(-(int)(size / 2.f), (int)(size / 2.f));
	return true;
}

void cRectContour2::Normalize()
{
	Point keys[4];
	for (u_int i = 0; i < 4; ++i)
		keys[i] = m_contours[i];
	OrderedContours(keys, m_contours);
}


// 박스 출력.
void cRectContour2::Draw(cv::Mat &dst, const cv::Scalar &color, const int thickness)
// color = cv::Scalar(0, 0, 0), thickness = 1
{
	DrawLines(dst, m_contours, color, thickness);
}


// 사각형의 중점을 리턴한다.
Point cRectContour2::Center() const
{
	Point center;
	for each (auto &pt in m_contours)
		center += pt;

	center = Point(center.x / 4, center.y / 4);
	return center;
}


// 사각형의 중점을 중심으로 스케일한다.
void cRectContour2::ScaleCenter(const float scale)
{
	const Point center = Center();

	for (u_int i = 0; i < 4; ++i)
	{
		m_contours[i] = center + ((m_contours[i] - center) * scale);
	}
}


// 가로 세로 각각 스케일링 한다.
// 0 -------- 1
// |          |
// |    +     |
// |          |
// 3 -------- 2
void cRectContour2::Scale(const float vscale, const float hscale)
{

	// 가로 스케일링
	Point tmp1[4];
	tmp1[0] = m_contours[1] + (m_contours[0] - m_contours[1]) * hscale;
	tmp1[1] = m_contours[0] + (m_contours[1] - m_contours[0]) * hscale;
	tmp1[2] = m_contours[3] + (m_contours[2] - m_contours[3]) * hscale;
	tmp1[3] = m_contours[2] + (m_contours[3] - m_contours[2]) * hscale;

	// 세로 스케일링
	cv::Point tmp2[4];
	tmp2[0] = m_contours[3] + (m_contours[0] - m_contours[3]) * vscale;
	tmp2[3] = m_contours[0] + (m_contours[3] - m_contours[0]) * vscale;
	tmp2[1] = m_contours[2] + (m_contours[1] - m_contours[2]) * vscale;
	tmp2[2] = m_contours[1] + (m_contours[2] - m_contours[1]) * vscale;

	m_contours[0] = (tmp1[0] + tmp2[0]) * 0.5f;
	m_contours[1] = (tmp1[1] + tmp2[1]) * 0.5f;
	m_contours[2] = (tmp1[2] + tmp2[2]) * 0.5f;
	m_contours[3] = (tmp1[3] + tmp2[3]) * 0.5f;
}


// index 번째 포인터를 리턴한다.
Point cRectContour2::At(const int index) const
{
	return m_contours[index];
}


int cRectContour2::Width() const
{
	return (int)(((m_contours[1].x - m_contours[0].x) +
		(m_contours[2].x - m_contours[3].x)) * 0.5f);
}


int cRectContour2::Height() const
{
	return (int)abs(((m_contours[3].y - m_contours[0].y) +
		(m_contours[2].y - m_contours[1].y)) * 0.5f);
}


cRectContour2& cRectContour2::operator= (const cRectContour2 &rhs)
{
	if (this != &rhs)
	{
		memcpy(m_contours, rhs.m_contours, sizeof(m_contours));
	}

	return *this;
}
