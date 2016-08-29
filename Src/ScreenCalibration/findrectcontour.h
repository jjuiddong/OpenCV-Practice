#pragma once



// 영상에서 rect를 찾아 리턴한다.
bool FindRectContour(IplImage *image, IplImage *binImage, 
	const int minArea, const int maxArea, OUT vector<cv::Point> &out);


