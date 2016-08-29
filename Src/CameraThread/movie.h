//
// 영상을 윈도우에 출력한다.
// 쓰레드로 작동한다.
//
#pragma once

#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <string>
using std::string;


class AutoCS
{
public:
	AutoCS(CRITICAL_SECTION *p) :m_p(p) { EnterCriticalSection(p); }
	~AutoCS() { LeaveCriticalSection(m_p); }
	CRITICAL_SECTION *m_p;
};



class cMovie
{
public:
	cMovie();
	virtual ~cMovie();

	bool Init(const string &name, const int width, const int height, const bool isThreadMode = false, const bool isFullScreen = false);
	void Draw(const cv::Mat &image);


public:
	string m_name;
	bool m_isThreadMode;
	int m_width;
	int m_height;
	cv::Mat m_image;

	HANDLE m_handle;
	DWORD m_threadId;
	bool m_threadLoop;
	CRITICAL_SECTION m_criticalSection;
};

