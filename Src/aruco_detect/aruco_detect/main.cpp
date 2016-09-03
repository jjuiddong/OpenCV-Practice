
#include <string>
#include <iostream>
#include "aruco.h"
#include "cvdrawingutils.h"
#include <opencv2/highgui/highgui.hpp>


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
using namespace aruco;

class CmdLineParser 
{ 
	int argc; char **argv; 
public: 
	CmdLineParser(int _argc, char **_argv) :argc(_argc), argv(_argv) 
	{
	}  
	bool operator[] (string param) 
	{ 
		int idx = -1;  
		for (int i = 0; i<argc && idx == -1; i++) 
			if (string(argv[i]) == param) 
				idx = i;    
		return (idx != -1); 
	} 
	string operator()(string param, string defvalue = "-1") 
	{ 
		int idx = -1;    
		for (int i = 0; i<argc && idx == -1; i++) 
			if (string(argv[i]) == param) 
				idx = i; 
		if (idx == -1) 
			return defvalue;   
		else  
			return (argv[idx + 1]); 
	} 
};


int main(int argc, char **argv) 
{

	CmdLineParser cml(argc, argv);
	if (argc == 1 || cml["-h"]) {
		cerr << "Usage: (in_image|video.avi) [-c cameraParams.yml] [-s markerSize] [-d <dicionary>:ARUCO default] [-o <outImage>] " << endl;
		cerr << "\tDictionaries: "; for (auto dict : aruco::Dictionary::getDicTypes())    cerr << dict << " "; cerr << endl;
		cout << "Example to work with apriltags dictionary : video.avi -d TAG36h11" << endl << endl;
		return 0;
	}

	aruco::CameraParameters CamParam;

	//VideoCapture vreader("video.avi");

	VideoCapture vreader;
	vreader.open(0);

// 		VideoCapture vreader(argv[1]);
// 		if (vreader.isOpened())
// 		{
// 			vreader >> InImage;
// 		}
// 		else 
// 		{ 
// 			cerr << "Could not open input" << endl; 
// 			return -1; 
// 		}

	if (cml["-c"])  
		CamParam.readFromXMLFile(cml("-c"));

	float MarkerSize = std::stof(cml("-s", "-1"));

	MarkerDetector MDetector;
	MDetector.setThresholdParams(7, 7);
	MDetector.setThresholdParamRange(2, 0);

	if (cml["-d"]) 
		MDetector.setDictionary(cml("-d"), 0.f);

	//aruco::Dictionary dic = aruco::Dictionary::loadPredefined("ARUCO");
	//MDetector.setDictionary("ARUCO");

	while (1)
	{
		Mat InImage;
		vreader >> InImage;
		if (!InImage.data)
			break;

		CamParam.resize(InImage.size());
		// Ok, let's detect
		vector< Marker >  Markers = MDetector.detect(InImage, CamParam, MarkerSize);
		//vector< Marker >  Markers = MDetector.detect(InImage);

		for (unsigned int i = 0; i < Markers.size(); i++) 
		{
			//cout << Markers[i] << endl;
			Markers[i].draw(InImage, Scalar(0, 0, 255), 2);
		}

		if (CamParam.isValid() && MarkerSize != -1)
			for (unsigned int i = 0; i < Markers.size(); i++) 
			{
				CvDrawingUtils::draw3dCube(InImage, Markers[i], CamParam);
			}

		// show input with augmented information
		cv::namedWindow("in", 1);
		cv::imshow("in", InImage);
		
		if (waitKey(1) == 27)
			break;
	}

}

