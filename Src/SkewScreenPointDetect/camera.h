//
// 카메라가 촬영한 영상을 가져오고, 윈도우에 출력하는 기능을 한다.
// 카메라 렌즈 왜곡을 보정하는 기능을 한다.
//
#pragma once

#include "CameraDistortion.h"


class cCamera
{
public:
	cCamera();
	virtual ~cCamera();

	bool Init(const int index=0);
	IplImage* GetCapture();
	cv::Mat& GetCaptureUndistortion();
	
	IplImage* ShowCapture();
	cv::Mat& ShowCaptureUndistortion();


protected:
	CvCapture* m_capture;
	cv::Mat m_undistortCapture;
	CameraDistortion m_camDistort;
};
