
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
