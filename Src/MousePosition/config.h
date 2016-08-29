//
// 전역 환경설정 파일을 읽어서, cConfig 클래스의 멤버 변수에 저장한다.
//
#pragma once

#include "opencv2/highgui/highgui.hpp"
#include <string>
#include <map>
using std::string;
using std::map;

class cConfig
{
public:
	cConfig();
	virtual ~cConfig();


protected:
	bool Parse(const string &fileName);
	void UpdateParseData();
	void InitDefault();


public:
	map<string, string> m_options;
	int m_screenWidth;
	int m_screenHeight;
	int m_cameraWidth;
	int m_cameraHeight;
	int m_cameraIndex;
	int m_screen_BinaryThreshold;
	float m_screenMinimumAreaRate;
	int m_rotateSensitivity;
	int m_detectPointThreshold;
	float m_detectPointMinArea;
	float m_detectPointMaxArea;

	bool m_showCamera;
	bool m_showScreen;
	bool m_showDetectPoint;
	bool m_showDebug;


	// camera undistortion
	double m_cx;
	double m_cy;
	double m_fx;
	double m_fy;
	double m_k1;
	double m_k2;
	double m_k3;
	double m_p1;
	double m_p2;
	double m_skew_c;

	// screen contour
	bool m_screenDetectEnable;
	cv::Point m_screenContours[4];

	// shared memory
	string m_sharedMemroyName;

	// UDP Network 
	bool m_udpCommunincationEnable;
	int m_udpPort;

	// Windows
	cv::Rect m_screenRect;
	cv::Rect m_cameraRect;
	cv::Rect m_screenBinRect;
	cv::Rect m_screenRecogRect;
	cv::Rect m_skewScreenRect;
	cv::Rect m_matchScreenRect;
	cv::Rect m_detectPointBinaryScreenRect;

	bool m_screenEnable;
	bool m_cameraEnable;
	bool m_screenBinEnable;
	bool m_screenRecogEnable;
	bool m_skewScreenEnable;
	bool m_matchScreenEnable;
	bool m_detectPointBinaryScreenEnable;

	bool m_matchScreenDrawBox;
};


extern cConfig g_config;
