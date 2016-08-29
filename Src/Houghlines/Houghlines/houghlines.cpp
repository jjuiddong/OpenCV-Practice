
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define PI 3.1415926

#pragma comment(lib, "opencv_core2411d.lib")
#pragma comment(lib, "opencv_highgui2411d.lib")
#pragma comment(lib, "opencv_imgproc2411d.lib")


int main()
{
	cv::Mat image = cv::imread("road.jpg", 0);
	cv::namedWindow("Original Image");
	cv::imshow("Original Image", image);

	// 캐니 알고리즘 적용
	cv::Mat contours;
	cv::Canny(image, contours, 150, 200);

	// 선 감지 위한 허프 변환
	std::vector<cv::Vec2f> lines;
	cv::HoughLines(contours, lines,
		1, PI / 180, // 단계별 크기
		300);
		//80);  // 투표(vote) 최대 개수

			  // 선 그리기
	cv::Mat result(contours.rows, contours.cols, CV_8U, cv::Scalar(255));
	std::cout << "Lines detected: " << lines.size() << std::endl;

	// 선 벡터를 반복해 선 그리기
	std::vector<cv::Vec2f>::const_iterator it = lines.begin();
	while (it != lines.end()) {
		float rho = (*it)[0];   // 첫 번째 요소는 rho 거리
		float theta = (*it)[1]; // 두 번째 요소는 델타 각도
		if (theta < PI / 4. || theta > 3.*PI / 4.) { // 수직 행
			cv::Point pt1(rho / cos(theta), 0); // 첫 행에서 해당 선의 교차점   
			cv::Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
			// 마지막 행에서 해당 선의 교차점
			cv::line(image, pt1, pt2, cv::Scalar(255), 1); // 하얀 선으로 그리기

		}
		else { // 수평 행
			cv::Point pt1(0, rho / sin(theta)); // 첫 번째 열에서 해당 선의 교차점  
			cv::Point pt2(result.cols, (rho - result.cols*cos(theta)) / sin(theta));
			// 마지막 열에서 해당 선의 교차점
			cv::line(image, pt1, pt2, cv::Scalar(255), 1); // 하얀 선으로 그리기
		}
		std::cout << "line: (" << rho << "," << theta << ")\n";
		++it;
	}

	cv::namedWindow("Detected Lines with Hough");
	cv::imshow("Detected Lines with Hough", image);

	cv::waitKey(0);

	return 0;
}

