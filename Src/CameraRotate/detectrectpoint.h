//
// 사각형을 나타내는 4점의 포인트를 인식해서 Skew transform을 계산한다.
//
#pragma once


class cDetectRectPoint
{
public:
	cDetectRectPoint();
	virtual ~cDetectRectPoint();

	void Init(const SimpleBlobDetector::Params &params);
	bool Detect(cCamera &camera0, const Mat &skewTransform);
	const vector<cv::Point>& GetDetectPoints();
	Mat SkewTransform(const int width, const int height, const float scale=1);


protected:
	vector<cv::Point> m_contour;	// Rect 포인터 (인덱스 순서)
	// 0 ---------- 1
	// |            |
	// |            |
	// |            |
	// 2 ---------- 3

	SimpleBlobDetector::Params m_params;
};


inline const vector<cv::Point>& cDetectRectPoint::GetDetectPoints() {
	return m_contour;
}
