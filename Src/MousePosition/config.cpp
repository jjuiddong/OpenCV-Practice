
//#include "global.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <stdexcept>


cConfig g_config;
using namespace cv;


cConfig::cConfig()
{
	if (!Parse("config.cfg"))
		InitDefault();
}

cConfig::~cConfig()
{

}


// 환경설정 파일을 읽어서 저장한다.
bool cConfig::Parse(const string &fileName)
{
	std::ifstream cfgfile(fileName);
	if (!cfgfile.is_open())
	{
		printf("not exist config file!!\n");
		return false;
	}

	string id, eq, val;
	while (cfgfile >> id)// >> eq >> val)
	{
		if (id[0] == '#')
		{ 
			string line;
			getline(cfgfile, line);
			continue;  // skip comments
		}

		cfgfile >> eq >> val;
		if (eq != "=") throw std::runtime_error("Parse error");
		m_options[id] = val;
	}

	UpdateParseData();

	return true;
}


// config 파일을 읽은 후, 읽은 정보를 실제 변수에 할당한다.
void cConfig::UpdateParseData()
{
	m_sharedMemroyName = atoi(m_options["sharedmemory_name"].c_str());
	m_udpCommunincationEnable = (atoi(m_options["udp_communincation_enable"].c_str()) > 0) ? true : false;
	m_udpPort = atoi(m_options["udp_port"].c_str());

	m_screenWidth = atoi(m_options["screen_width"].c_str());
	m_screenHeight = atoi(m_options["screen_height"].c_str());
	m_cameraWidth = atoi(m_options["camera_width"].c_str());
	m_cameraHeight = atoi(m_options["camera_height"].c_str());
	
	m_cameraIndex = atoi(m_options["camera_index"].c_str());

	m_screen_BinaryThreshold = atoi(m_options["screen_binary_threshold"].c_str());
	m_screenMinimumAreaRate = (float)atof(m_options["screen_minimum_area_rate"].c_str());
	
	m_rotateSensitivity = atoi(m_options["rotate_sensitivity"].c_str());

	m_detectPointThreshold = atoi(m_options["detect_point_threshold"].c_str());
	m_detectPointMinArea = (float)atof(m_options["detect_point_minarea"].c_str());
	m_detectPointMaxArea = (float)atof(m_options["detect_point_maxarea"].c_str());


	m_showCamera = (atoi(m_options["camera_show"].c_str()) > 0) ? true : false;
	m_showScreen = (atoi(m_options["screen_show"].c_str()) > 0) ? true : false;
	m_showDetectPoint = (atoi(m_options["detectpoint_show"].c_str()) > 0) ? true : false;
	m_showDebug = (atoi(m_options["debug_show"].c_str()) > 0) ? true : false;


	m_cx = (float)atof(m_options["cx"].c_str());
	m_cy = (float)atof(m_options["cy"].c_str());
	m_fx = (float)atof(m_options["fx"].c_str());
	m_fy = (float)atof(m_options["fy"].c_str());
	m_k1 = (float)atof(m_options["k1"].c_str());
	m_k2 = (float)atof(m_options["k2"].c_str());
	m_k3 = (float)atof(m_options["k3"].c_str());
	m_p1 = (float)atof(m_options["p1"].c_str());
	m_p2 = (float)atof(m_options["p2"].c_str());
	m_skew_c = (float)atof(m_options["skew_c"].c_str());


	m_screenDetectEnable = (atoi(m_options["screen_detect_enable"].c_str()) > 0) ? true : false;
	m_screenContours[0].x = atoi(m_options["screen_contour_x1"].c_str());
	m_screenContours[0].y = atoi(m_options["screen_contour_y1"].c_str());
	m_screenContours[1].x = atoi(m_options["screen_contour_x2"].c_str());
	m_screenContours[1].y = atoi(m_options["screen_contour_y2"].c_str());
	m_screenContours[2].x = atoi(m_options["screen_contour_x3"].c_str());
	m_screenContours[2].y = atoi(m_options["screen_contour_y3"].c_str());
	m_screenContours[3].x = atoi(m_options["screen_contour_x4"].c_str());
	m_screenContours[3].y = atoi(m_options["screen_contour_y4"].c_str());


	{
		const string windowName = "screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[ windowName+"_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_screenRect = Rect(x, y, w, h);
		m_screenEnable = enable;
	}

	{
		const string windowName = "screen_binariztion";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_screenBinRect = Rect(x, y, w, h);
		m_screenBinEnable = enable;
	}

	{
		const string windowName = "screen_recognition";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_screenRecogRect = Rect(x, y, w, h);
		m_screenRecogEnable = enable;
	}

	{
		const string windowName = "camera";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_cameraRect = Rect(x, y, w, h);
		m_cameraEnable = enable;
	}

	{
		const string windowName = "skew_screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_skewScreenRect = Rect(x, y, w, h);
		m_skewScreenEnable = enable;
	}

	{
		const string windowName = "match_screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_matchScreenRect = Rect(x, y, w, h);
		m_matchScreenEnable = enable;

		m_matchScreenDrawBox = (atoi(m_options[windowName + "_drawbox"].c_str()) > 0) ? true : false;
	}

	{
		const string windowName = "detect_point_binary_screen";
		const bool enable = (atoi(m_options[windowName + "_enable"].c_str()) > 0) ? true : false;
		const int x = atoi(m_options[windowName + "_x"].c_str());
		const int y = atoi(m_options[windowName + "_y"].c_str());
		const int w = atoi(m_options[windowName + "_w"].c_str());
		const int h = atoi(m_options[windowName + "_h"].c_str());
		m_detectPointBinaryScreenRect = Rect(x, y, w, h);
		m_detectPointBinaryScreenEnable = enable;
	}
}


// 기본 설정 값으로 설정한다.
void cConfig::InitDefault()
{
	m_sharedMemroyName = "killing_space_camera";
	m_udpCommunincationEnable = true;


	m_screenWidth = 1024;
	m_screenHeight = 768;
	m_cameraWidth = 640;
	m_cameraHeight = 480;
	m_cameraIndex = 0;
	m_screen_BinaryThreshold = 55;
	m_rotateSensitivity = 200;
	m_detectPointThreshold = 33;
	m_detectPointMinArea = 50;
	m_detectPointMaxArea = 200;

	m_cx = 346.982762f;
	m_cy = 229.626156f;
	m_fx = 676.748756f;
	m_fy = 677.032333f;
	m_k1 = -0.331818f;
	m_k2 = -0.033314f;
	m_k3 = 0.f;
	m_p1 = -0.000313f;
	m_p2 = 0.000841f;
	m_skew_c = 0;

	m_showCamera = true;
	m_showScreen = true;
	m_showDetectPoint = true;
	m_showDebug = true;
	m_udpPort = 8888;


	m_screenDetectEnable = true;
	m_screenContours[0] = Point(0, 0);
	m_screenContours[1] = Point(640, 0);
	m_screenContours[2] = Point(640, 480);
	m_screenContours[3] = Point(0, 480);


	// windows
	m_screenRect = Rect(0, 0, 1024, 768);
	m_cameraRect = Rect(-660, 0, 640, 480);
	m_screenBinRect = Rect(-660, 520, 640, 480);
	m_screenRecogRect = Rect(-1320, 520, 640, 480);
	m_skewScreenRect = Rect(-660, 520, 640, 480);
	m_matchScreenRect = Rect(-1680, 0, 640, 480);
	m_detectPointBinaryScreenRect = Rect(-1310, 520, 640, 480);

	m_screenEnable = true;
	m_cameraEnable = true;
	m_screenBinEnable = true;
	m_screenRecogEnable = true;
	m_skewScreenEnable = true;
	m_matchScreenEnable = true;
	m_detectPointBinaryScreenEnable = true;

	m_matchScreenDrawBox = false;

}
