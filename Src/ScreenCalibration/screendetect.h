
#pragma once

#include "global.h"


// 스크린을 인식해서 크기와 포인트를 리턴한다.
cv::Rect ScreenDetectAuto(std::vector<cv::Point> &out,
	const int minScreenArea, OUT int &outThreshold);


cv::Rect ScreenDetectManual(std::vector<cv::Point> &out,
	const int minScreenArea, OUT int &outThreshold);
