
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "aruco.h"


#ifdef _DEBUG
#pragma comment(lib, "opencv_core310d.lib")
#pragma comment(lib, "opencv_imgcodecs310d.lib")
// #pragma comment(lib, "opencv_features2d310d.lib")
#pragma comment(lib, "opencv_videoio310d.lib")
#pragma comment(lib, "opencv_highgui310d.lib")
// #pragma comment(lib, "opencv_imgproc310d.lib")
// #pragma comment(lib, "opencv_flann310d.lib")
// #pragma comment(lib, "opencv_xfeatures2d310d.lib")
// #pragma comment(lib, "opencv_calib3d310d.lib")
#pragma comment(lib, "aruco209d.lib")
#else
#pragma comment(lib, "opencv_core310.lib")
#pragma comment(lib, "opencv_imgcodecs310.lib")
//	#pragma comment(lib, "opencv_features2d310.lib")
#pragma comment(lib, "opencv_videoio310.lib")
#pragma comment(lib, "opencv_highgui310.lib")
// 	#pragma comment(lib, "opencv_imgproc310.lib")
// 	#pragma comment(lib, "opencv_flann310.lib")
// 	#pragma comment(lib, "opencv_xfeatures2d310.lib")
// 	#pragma comment(lib, "opencv_calib3d310.lib")
#pragma comment(lib, "aruco209.lib")
#endif


using namespace cv;
using namespace std;

//convinience command line parser
class CmdLineParser { int argc; char **argv; public:   CmdLineParser(int _argc, char **_argv) :argc(_argc), argv(_argv) {} bool operator[] (string param) { int idx = -1;  for (int i = 0; i<argc && idx == -1; i++) if (string(argv[i]) == param) idx = i;    return (idx != -1); } string operator()(string param, string defvalue = "-1") { int idx = -1;    for (int i = 0; i<argc && idx == -1; i++) if (string(argv[i]) == param) idx = i; if (idx == -1) return defvalue;   else  return (argv[idx + 1]); } };


int main(int argc, char **argv) {
	try {
		if (argc < 2) {
			// You can also use ids 2000-2007 but it is not safe since there are a lot of false positives.
			cerr << "Usage: <makerid> outfile.(jpg|png|ppm|bmp)  [options] \n\t[-bs <size>:bit size in pixels. 50 by default ] \n\t[-d <dictionary>: ARUCO default]" << endl;
			auto dict_names = aruco::Dictionary::getDicTypes();
			cerr << "\t\tDictionaries: ";
			for (auto dict : dict_names)    cerr << dict << " "; cerr << endl;
			return -1;
		}
		CmdLineParser cml(argc, argv);
		int pixSize = std::stoi(cml("-bs", "75"));//pixel size each each bit
												  //loads the desired dictionary
		aruco::Dictionary dic = aruco::Dictionary::loadPredefined(cml("-d", "ARUCO"));

		Mat outImg = dic.getMarkerImage_id(stoi(argv[1]), pixSize);
		cv::imwrite(argv[2], outImg);

	}
	catch (std::exception &ex) {
		cout << ex.what() << endl;
	}
}
