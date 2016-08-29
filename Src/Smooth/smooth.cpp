
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;

#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")
#pragma comment(lib, "opencv_features2d2411d.lib")
#pragma comment(lib, "opencv_nonfree2411d.lib")

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		cout << "command line <image file name>" << endl;
		return 0;
	}

	Mat src = imread(argv[1]);
	if (src.empty())
		return 0;

	//cvConvertImage(&src, &src, );

	Mat dst = src.clone();
	namedWindow("src");
	namedWindow("dst");
	//cvSmooth(&src, &dst);
	//medianBlur(src, dst, 7);
	//GaussianBlur(src, dst, Size(7, 7), 0);
	bilateralFilter(src, dst, 31, 15, 15);
	imshow("src", src);
	imshow("dst", dst);
	waitKey();

	return 1;
}
