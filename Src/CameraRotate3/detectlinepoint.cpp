
#include "global.h"
#include "detectlinepoint.h"

using namespace cv;


cDetectLinePoint::cDetectLinePoint()
{
	SimpleBlobDetector::Params params;

	// Change thresholds
	params.minThreshold = 1;
	params.maxThreshold = 20;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = g_config.m_detectPointMinArea;
	params.maxArea = g_config.m_detectPointMaxArea;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1f;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87f;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01f;

	Init(params);
}

cDetectLinePoint::~cDetectLinePoint()
{

}


// 파라메터 초기화.
void cDetectLinePoint::Init(const SimpleBlobDetector::Params &params)
{
	m_params = params;
}


// 사각형 포인트를 인식한다.
// 인식에 성공하면 true를 리턴한다.
bool cDetectLinePoint::Detect(cCamera &camera0, const Mat &skewTransform)
{
	bool isSearch = true;
	int detectCount = 0;
	int thresholdValue = g_config.m_binaryThreshold; // 임계값(Threshold) 설정
	std::vector<Point> detectPoints(4);

	cvNamedWindow("HUV05-binarization", 0);
	cvResizeWindow("HUV05-binarization", 640, 480);
	cvMoveWindow("HUV05-binarization", -660, 520);
	cvCreateTrackbar("Threshold", "HUV05-binarization", &thresholdValue, 255, NULL);


	Mat src(camera0.Resolution().height, camera0.Resolution().width, CV_8UC(3));
	while (isSearch)
	{
		Mat &cam = camera0.ShowCaptureUndistortion(skewTransform, src);
		if (cam.empty())
			continue;

		Mat grayscaleMat = src.clone();
		cvtColor(src, grayscaleMat, CV_RGB2GRAY); // 컬러를 흑백으로 변환

		Mat binaryMat(grayscaleMat.size(), grayscaleMat.type());
		cv::threshold(grayscaleMat, binaryMat, thresholdValue, 255, cv::THRESH_BINARY);


		//----------------------------------------------------------------------------------------------
		//  이미지에서 포인터를 찾는다.
		SimpleBlobDetector detector(m_params);

		// Detect blobs.
		std::vector<KeyPoint> keypoints;
		detector.detect(binaryMat, keypoints);

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
		Mat im_with_keypoints;
		drawKeypoints(binaryMat, keypoints, im_with_keypoints, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("HUV05-binarization", im_with_keypoints);

		if (keypoints.size() == 4)
		{
			// 4개의 포인트를 안정적으로 찾으면, 위치를 저장하고, Detect를 종료한다.
			++detectCount;
			if (detectCount > 20)
			{
				isSearch = false;

				// 4개의 포인트를 순서대로 맞추어 저장한다.
				vector<Point> keys(4);
				for (u_int i = 0; i < keypoints.size(); ++i)
					keys[i] = Point(keypoints[i].pt);
				OrderedLineContours(keys, detectPoints);

				DrawLines(im_with_keypoints, detectPoints, Scalar(0, 255, 0), 2, false);
				imshow("HUV05-binarization", im_with_keypoints);
			}
		}
		else
		{
			detectCount = 0;
		}

		if (cvWaitKey(10) >= 0)
			break;
	}

	cvDestroyWindow("HUV05-binarization");

	m_contour = detectPoints;

	return true;
}


// Skew 변환 행렬을 계산해서 리턴한다.
//
//                |-scale/2|
// +---------------------+ ------
// |              |      | scale/2
// |              |      |   |
// |     *--*--*--*      | -------
// |                     | scale/2
// |                     |   |
// +---------------------+ ------
//
// scale 값 만큼 m_contour 를 늘려 잡는다.
Mat cDetectLinePoint::SkewTransform(const int width, const int height, const float scaleW, const float scaleH)
{
	Point2f p0 = m_contour.front();
	Point2f p1 = m_contour.back();
	Point2f p2 = p1 - p0;

	// p2 normalize
	const float len = sqrt(p2.x*p2.x + p2.y*p2.y);
	p2 = Point2f(p2.x / len, p2.y / len);

	const Mat v1 = (Mat_<double>(1, 3) << p2.x , p2.y , 0);
	const Mat v2 = (Mat_<double>(1, 3) << 0, 0, 1);
	Mat v3 = v2.cross(v1);

	v3.at<double>(0, 0);

	// v3 normalize
	const double len2 = sqrt(v3.at<double>(0, 0)*v3.at<double>(0, 0) + v3.at<double>(0, 1)*v3.at<double>(0, 1));
	Point2f nv3((float)(v3.at<double>(0, 0) / len2), (float)(v3.at<double>(0, 1) / len2));

	Point2f vv1 = nv3 + p2;
	Point2f vv2 = nv3 - p2;

	// vv1, vv2 normalize
	const float len3 = sqrt(vv1.x*vv1.x + vv1.y*vv1.y);
	vv1 = Point2f(vv1.x / len3, vv1.y / len3);
	const float len4 = sqrt(vv2.x*vv2.x + vv2.y*vv2.y);
	vv2 = Point2f(vv2.x / len4, vv2.y / len4);

 	vector<Point> rect(4);
	
	const Point2f offset1(vv1.x * scaleW * 0.5f, 
		vv1.y * scaleH * 0.5f);
	const Point2f offset2(-vv1.x * scaleW * 0.5f,
		-vv1.y * scaleH * 0.5f);
	const Point2f offset3(vv2.x * scaleW * 0.5f,
		vv2.y * scaleH * 0.5f);
	const Point2f offset4(-vv2.x * scaleW * 0.5f,
		-vv2.y * scaleH * 0.5f);

	rect[2] = p1 + offset1;
	rect[0] = p0 + offset2;
	rect[3] = p0 + offset3;
	rect[1] = p1 + offset4;

	return ::SkewTransform(rect, width, height, 1);
}
