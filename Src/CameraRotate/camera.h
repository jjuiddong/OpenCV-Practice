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

	bool Init(const int index=0, const cv::Size &resolution=cv::Size(640,480));
	IplImage* GetCapture();
	cv::Mat& GetCaptureUndistortion();
	
	IplImage* ShowCapture();
	cv::Mat& ShowCaptureUndistortion();
	cv::Mat& ShowCaptureUndistortion(const cv::Mat &skewTransform, OUT cv::Mat &dst);
	const cv::Size& Resolution() const;


protected:
	CvCapture* m_capture;
	cv::Mat m_undistortCapture;
	CameraDistortion m_camDistort;
	cv::Size m_resolution; // 카메라 해상도
};


inline const cv::Size& cCamera::Resolution() const { return m_resolution;  }
