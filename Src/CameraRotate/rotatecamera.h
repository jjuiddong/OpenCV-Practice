//
// 카메라가 얼마나 회전하는지 인식하는 역할을 한다.
// 기준 포인트의 상대 값으로 판단한다.
//
#pragma once


class cRotateCamera
{
public:
	cRotateCamera();
	virtual ~cRotateCamera();

	void DetectCameraRotate(cCamera &camera, const vector<cv::Point> &contours, const Mat &skewTransform);

	SimpleBlobDetector::Params m_params;
};
