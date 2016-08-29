
#include "global.h"
#include "camera.h"

cCamera g_camera;

using namespace cv;


cCamera::cCamera() :
	m_capture(NULL)
	,m_resolution(640, 480)
{
	// 카메라 렌즈 왜곡 보정 파라메터
	const double cx = 346.982762f;
	const double cy = 229.626156f;
	const double fx = 676.748756f;
	const double fy = 677.032333f;
	const double k1 = -0.331818f;
	const double k2 = -0.033314f;
	const double k3 = 0.f;
	const double p1 = -0.000313f;
	const double p2 = 0.000841f;
	const double skew_c = 0;
	m_camDistort.SetParams(cx, cy, fx, fy, k1, k2, k3, p1, p2, skew_c);
}

cCamera::~cCamera()
{
	cvDestroyWindow("HUV05-camera");
	cvReleaseCapture(&m_capture);
}


// 카메라 초기화.
bool cCamera::Init(const int index, const cv::Size &resolution) // index=0, resolution = Size(640, 480)
{
	m_capture = cvCaptureFromCAM(index);
	m_resolution = resolution;

	cvNamedWindow("HUV05-camera", 0);
	cvResizeWindow("HUV05-camera", 640, 480);
	cvMoveWindow("HUV05-camera", -660, 0);
	return true;
}


// 카메라가 인식한 영상을 가져온다.
IplImage* cCamera::GetCapture()
{
	if (!m_capture)
		return NULL;

	return cvQueryFrame(m_capture);
}


// 카메라 렌즈에 의한 왜곡되지 않는 영상을 리턴한다.
Mat& cCamera::GetCaptureUndistortion()
{
	IplImage *image = cvQueryFrame(m_capture);
	if (!image)
		return m_undistortCapture;

	Mat src(image);	
	if (m_undistortCapture.empty())
		m_undistortCapture = src.clone();

	m_camDistort.UndistortImage(src, m_undistortCapture);

	return m_undistortCapture;
}


// 카메라가 촬영한 영상을 윈도우에 출력한다.
IplImage* cCamera::ShowCapture()
{
	IplImage *image = GetCapture();
	if (!image)
		return NULL;

	cvShowImage("HUV05-camera", image);
	return image;
}


// 카메라가 촬영한 영상을 보정하여 윈도우에 출력한다.
Mat& cCamera::ShowCaptureUndistortion()
{
	Mat &image = GetCaptureUndistortion();
	if (image.empty())
		return image;

	imshow("HUV05-camera", image);
	return image;
}


// 카메라가 촬영한 영상을 보정하여 윈도우에 출력한다.
// 인자로 넘어온 skewTransform 값으로 특정 영역의 영상을 복사해 dst에 저장한다.
cv::Mat& cCamera::ShowCaptureUndistortion(const cv::Mat &skewTransform, OUT cv::Mat &dst)
{
	Mat &image = GetCaptureUndistortion();
	if (image.empty())
		return image;

	cv::warpPerspective(image, dst, skewTransform, dst.size());

	imshow("HUV05-camera", image);
	return image;
}
