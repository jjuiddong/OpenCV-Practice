//
// 라인을 나타내는 2점 이상의 포인트를 인식해서 Skew transform을 계산한다.
//
#pragma once


class cDetectLinePoint
{
public:
	cDetectLinePoint();
	virtual ~cDetectLinePoint();

	void Init(const SimpleBlobDetector::Params &params);
	bool Detect(cCamera &camera0, const Mat &skewTransform);
	const vector<cv::Point>& GetDetectPoints();
	Mat SkewTransform(const int width, const int height, const float scaleW, const float scaleH);


protected:
	// 0 --- 1 --- 2 --- N
	vector<cv::Point> m_contour;	// Line 포인터 (인덱스 순서)

	SimpleBlobDetector::Params m_params;
};


inline const vector<cv::Point>& cDetectLinePoint::GetDetectPoints() {
	return m_contour;
}
