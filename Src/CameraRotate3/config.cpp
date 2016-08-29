
#include "global.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <stdexcept>


cConfig g_config;


cConfig::cConfig()
{
	if (!Parse("config.txt"))
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
	m_screenWidth = atoi(m_options["screen_width"].c_str());
	m_screenHeight = atoi(m_options["screen_height"].c_str());
	m_cameraWidth = atoi(m_options["camera_width"].c_str());
	m_cameraHeight = atoi(m_options["camera_height"].c_str());

	m_screen_BinaryThreshold = atoi(m_options["screen_binary_threshold"].c_str());
	m_binaryThreshold = atoi(m_options["binary_threshold"].c_str());
	m_rotateSensitivity = atoi(m_options["rotate_sensitivity"].c_str());

	m_detectPointMinArea = (float)atof(m_options["detect_point_minarea"].c_str());
	m_detectPointMaxArea = (float)atof(m_options["detect_point_maxarea"].c_str());
}


// 기본 설정 값으로 설정한다.
void cConfig::InitDefault()
{
	m_screenWidth = 1024;
	m_screenHeight = 768;
	m_cameraWidth = 640;
	m_cameraHeight = 480;
	m_screen_BinaryThreshold = 55;
	m_binaryThreshold = 95;
	m_rotateSensitivity = 200;
	m_detectPointMinArea = 50;
	m_detectPointMaxArea = 200;
}
