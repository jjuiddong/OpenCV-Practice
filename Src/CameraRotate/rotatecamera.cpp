
#include "global.h"
#include "rotatecamera.h"


using namespace cv;


cRotateCamera::cRotateCamera()
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


	m_params = params;
}

cRotateCamera::~cRotateCamera()
{

}


// 카메라 회전을 인식한다.
void cRotateCamera::DetectCameraRotate(cCamera &camera0, const vector<cv::Point> &contours, const Mat &skewTransform)
{
	int thresholdValue = g_config.m_binaryThreshold; // 임계값(Threshold) 설정
	int sensitive = g_config.m_rotateSensitivity;

	cRectContours origContours(contours);
	cRectContours target = origContours;
	target.ScaleCenter(0.2f);


	cvNamedWindow("HUV05-binarization", 0);
	cvResizeWindow("HUV05-binarization", 640, 480);
	cvMoveWindow("HUV05-binarization", -660, 520);
	cvCreateTrackbar("Threshold", "HUV05-binarization", &thresholdValue, 255, NULL);
	cvCreateTrackbar("Sensitivity", "HUV05-binarization", &sensitive, 300, NULL);

	Mat src(camera0.Resolution().height, camera0.Resolution().width, CV_8UC(3));
	while (1)
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
		
		origContours.Draw(im_with_keypoints, Scalar(0, 255, 0), 2);
		target.Draw(im_with_keypoints, Scalar(0, 0, 255), 2);

		if (!keypoints.empty())
		{
			if (keypoints.size() == 4)
			{
				// 4개의 포인트를 안정적으로 찾으면, 위치를 저장하고, Detect를 종료한다.
				cRectContours detectRect(keypoints);
				detectRect.Draw(im_with_keypoints, Scalar(0, 255, 255), 2);

				// 원점에서 벗어난 길이를 출력한다.
				vector<Point> centerLine;
				centerLine.push_back(detectRect.Center());
				centerLine.push_back(origContours.Center());
				DrawLines(im_with_keypoints, centerLine, Scalar(255, 0, 0), 2);
				//

				Point offset = (origContours.Center() - detectRect.Center()) * (sensitive / 100.f);
				target.Init(origContours.Center() + offset, 10.f);
			}
		}

		imshow("HUV05-binarization", im_with_keypoints);

		if (cvWaitKey(10) == VK_ESCAPE)
			break;
	}

	cvDestroyWindow("HUV05-binarization");	
}
