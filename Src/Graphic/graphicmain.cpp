
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")

using namespace std;

void main(char argc, char *argv[])
{
	cv::Mat src(480, 640, CV_8UC3);
	src.setTo(0);

	cvLine(&IplImage(src), cvPoint(100, 100), cvPoint(200, 200), cvScalar(255, 255, 255), 1);
	cvRectangle(&IplImage(src), cvPoint(200, 200), cvPoint(300, 300), cvScalar(255, 0, 0));
	cvRectangle(&IplImage(src), cvPoint(310, 200), cvPoint(410, 300), cvScalar(0, 0, 255), CV_FILLED);
	cvCircle(&IplImage(src), cvPoint(200, 100), 50, cvScalar(0, 255, 0), 2);
	cvCircle(&IplImage(src), cvPoint(310, 100), 50, cvScalar(0, 255, 0), -1);
	cvEllipse(&IplImage(src), cvPoint(300, 400), cvSize(100, 50), 0, 0, 360, cvScalar(0, 255, 255), 1);
	cvEllipse(&IplImage(src), cvPoint(300, 400), cvSize(90, 40), 10, 0, 360, cvScalar(0, 255, 255), 1);
	cvEllipse(&IplImage(src), cvPoint(300, 400), cvSize(80, 30), 20, 0, 360, cvScalar(0, 255, 255), 1);
	cvEllipse(&IplImage(src), cvPoint(300, 400), cvSize(70, 20), 30, 0, 360, cvScalar(0, 255, 255), CV_FILLED);

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5f, 1);
	cvPutText(&IplImage(src), "Hello", cvPoint(500, 400), &font, cvScalar(255, 255, 255));
	cv::putText(src, "Hello", cvPoint(500, 450), CV_FONT_HERSHEY_COMPLEX, 1, cvScalar(255, 255, 255));

	imshow("window", src);

	cvWaitKey();
}
