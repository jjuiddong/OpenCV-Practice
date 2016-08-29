//
// 스크린에서 레이저 포인트를 찾아, 위치를 계산한다.
//
// 스크린은 흰색 바탕에 검은색 레이저 포인트만 있다고 가정한다.
//
#pragma once


class cDetectPoint
{
public:
	cDetectPoint();
	virtual ~cDetectPoint();

	cv::Point DetectPoint(cScreen &screen, cCamera &camera, const cv::Mat &src);
};

