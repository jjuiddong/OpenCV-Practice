//
// 라인을 표현하는 클래스.
//
#pragma once


class cLineContour
{
public:
	cLineContour();
	cLineContour(const vector<cv::Point> &contours);
	cLineContour(const vector<cv::KeyPoint> &contours);
	cLineContour(const cv::Point &center, const float size);
	virtual ~cLineContour();

	bool Init(const vector<cv::Point> &contours);
	bool Init(const vector<cv::KeyPoint> &keypoints);
	bool Init(const cv::Point &center, const float size);
	void Draw(cv::Mat &dst, const cv::Scalar &color = cv::Scalar(0, 0, 0), const int thickness = 1);

	cv::Point Center() const;
//	void ScaleCenter(const float scale);


protected:
	vector<cv::Point> m_contours;
};
