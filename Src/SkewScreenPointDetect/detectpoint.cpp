
#include "global.h"
#include "detectpoint.h"


using namespace cv;

cDetectPoint::cDetectPoint()
{
	namedWindow("BinaryScreen", 0);
	cvResizeWindow("BinaryScreen", 640, 480);
	cvMoveWindow("BinaryScreen", -1305, 520);
}

cDetectPoint::~cDetectPoint()
{
	cvDestroyWindow("BinaryScreen");
}


//
Point cDetectPoint::DetectPoint(cScreen &screen, cCamera &camera, const Mat &src)
{
	const int thresholdValue = 33;

	Mat grayscaleMat = src.clone();
	cvtColor(src, grayscaleMat, CV_RGB2GRAY); // 컬러를 흑백으로 변환

	Mat binaryMat(grayscaleMat.size(), grayscaleMat.type());
	threshold(grayscaleMat, binaryMat, thresholdValue, 255, cv::THRESH_BINARY);

	//----------------------------------------------------------------------------------------------
	// 스크린 이미지에서 포인터를 찾는다.
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 1;
	params.maxThreshold = 20;

	// Filter by Area.
	params.filterByArea = true;
	//params.minArea = 1500;
	params.minArea = 10;
	params.maxArea = 200;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1f;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87f;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01f;

	SimpleBlobDetector detector(params);

	// Detect blobs.
	std::vector<KeyPoint> keypoints;
	detector.detect(binaryMat, keypoints);

	int posX = 0, posY = 0;
	if (!keypoints.empty())
	{

	}

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	Mat im_with_keypoints;
	drawKeypoints(binaryMat, keypoints, im_with_keypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("BinaryScreen", im_with_keypoints);

	return Point(posX, posY);
}

