
#include <iostream>
#include <string.h>
// for OpenCV2
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/gpu/gpu.hpp"
#include "opencv2/highgui/highgui.hpp"

#ifdef _DEBUG
	#pragma comment(lib, "opencv_core2411d.lib")
	#pragma comment(lib, "opencv_imgproc2411d.lib")
	#pragma comment(lib, "opencv_objdetect2411d.lib")
	#pragma comment(lib, "opencv_gpu2411d.lib")
	#pragma comment(lib, "opencv_highgui2411d.lib")
#else
	#pragma comment(lib, "opencv_core2411.lib")
	#pragma comment(lib, "opencv_imgproc2411.lib")
	#pragma comment(lib, "opencv_objdetect2411.lib")
	#pragma comment(lib, "opencv_gpu2411.lib")
	#pragma comment(lib, "opencv_highgui2411.lib")
#endif

using namespace cv;
using namespace std;

void main()
{
	string mapStr[] = {
		"finland_tupasentie_pro.png",
		"finland_alhojarvi.png",
		"finland_hanisjarventie.png",
		"finland_hanisjarventie2.png",
		"finland_tupasentie_pro5.png",
		"finland_tupasentie_pro6.png",
		"finland_isojarvi.png",
		"finland_kaatselk.png",
		"finland_kakaristo.png",
		"finland_lankamaa.png",
		"finland_lankamaa2.png",
		"finland_mutanen.png",
		"finland_mutanen2.png",
		"finland_tupasentie_pro2.png",
		"finland_tupasentie_pro3.png",
		"finland_tupasentie_pro4.png",
		"kenya_mwatate_pro.png",
	};
	
	string labelStr[] = {
		"finland_tupasentie_label.png",
		"finland_alhojarvi_label.png",
		"finland_hanisjarventie_label.png",
		"finland_isojarvi_label.png",
		"finland_kaatselk_label.png",
		"finland_kakaristo_label.png",
		"finland_lankamaa_label.png",
		"finland_mutanen_label.png",
		"kenya_mwatate_pro_label.png",
	};

	const int mapSize = sizeof(mapStr) / sizeof(string);
	const int labelSize = sizeof(labelStr) / sizeof(string);

	for (int i = 0; i < mapSize; ++i)
	{
		Mat A = imread(mapStr[i]);
		if (A.empty())
			continue;

		double bestMax = 0;
		int bestIdx = 0;
		Point bestLeft_top;
		for (int k = 0; k < labelSize; ++k)
		{
			double min, max;
			Point left_top;
			Mat B = imread(labelStr[k]);
			Mat C(cvSize(A.cols - B.cols + 1, A.rows - B.rows + 1), IPL_DEPTH_32F);
 			cv::matchTemplate(A, B, C, CV_TM_CCOEFF_NORMED);
 			cv::minMaxLoc(C, &min, &max, NULL, &left_top);

			if (bestMax < max)
			{
				bestMax = max;
				bestLeft_top = left_top;
				bestIdx = k;
			}

			cout << "k = " << k << ", min = " << min << ", max = " << max << endl;
		}

		Mat B = imread(labelStr[bestIdx]);
		cv::rectangle(A, bestLeft_top, cvPoint(bestLeft_top.x + B.cols, bestLeft_top.y + B.rows), CV_RGB(255, 0, 0)); // 찾은 물체에 사격형을 그린다.
		cout << "find label = " << labelStr[bestIdx].c_str() << endl;

 		imshow("result", A); // 결과 보기
 		//imshow("T9-sample", B);
 		//imshow("C", C); // 상관계수 이미지 보기
 		cvWaitKey(0);
	}
}
