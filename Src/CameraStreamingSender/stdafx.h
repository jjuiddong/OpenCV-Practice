#pragma once

#define  _CRT_SECURE_NO_WARNINGS


// 매크로 정의
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if (p) { delete p; p=NULL;} }
#endif
#ifndef SAFE_DELETEA
#define SAFE_DELETEA(p) {if (p) { delete[] p; p=NULL;} }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }
#endif
#ifndef SAFE_RELEASE2
#define SAFE_RELEASE2(p) {if (p) { p->release(); p=NULL;} }
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

#define RET(exp)		{if((exp)) return; }			// exp가 true이면 리턴
#define RETV(exp,val)	{if((exp)) return val; }
#define ASSERT_RET(exp)		{assert(exp); RET(!(exp) ); }
#define ASSERT_RETV(exp,val)		{assert(exp); RETV(!(exp),val ); }
#define BRK(exp)		{if((exp)) break; }			// exp가 break

typedef unsigned int    u_int;


#include <string>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <set>

using std::string;
using std::wstring;
using std::vector;
using std::map;
using std::list;
using std::set;
using std::stringstream;
using std::wstringstream;



#include <windows.h>
#include <winsock.h>
#include <string>
#include <algorithm>

#pragma comment( lib, "wsock32.lib" )


namespace network {
	enum {
		BUFFER_LENGTH = 512,
	};
}

namespace common
{
	/**
	@brief this function only avaible to unique ty value
	remove ty and then rotate vector to sequence elements
	*/
	template <class Seq>
	bool removevector(Seq &seq, const typename Seq::value_type &ty)
	{
		for (size_t i = 0; i < seq.size(); ++i)
		{
			if (seq[i] == ty)
			{
				if ((seq.size() - 1) > i) // elements를 회전해서 제거한다.
					std::rotate(seq.begin() + i, seq.begin() + i + 1, seq.end());
				seq.pop_back();
				return true;
			}
		}
		return false;
	}

	template <class Seq>
	void rotatepopvector(Seq &seq, const unsigned int idx)
	{
		if ((seq.size() - 1) > idx) // elements를 회전해서 제거한다.
			std::rotate(seq.begin() + idx, seq.begin() + idx + 1, seq.end());
		seq.pop_back();
	}
}


class cAutoCS
{
public:
	cAutoCS(CRITICAL_SECTION &cs) :
		m_cs(cs)
		, m_isLeave(false) {
		EnterCriticalSection(&cs);
	}
	virtual ~cAutoCS() {
		if (!m_isLeave)
			LeaveCriticalSection(&m_cs);
		m_isLeave = true;
	}
	void Enter() {
		if (m_isLeave)
			EnterCriticalSection(&m_cs);
		m_isLeave = false;
	}
	void Leave() {
		LeaveCriticalSection(&m_cs);
		m_isLeave = true;
	}
	CRITICAL_SECTION &m_cs;
	bool m_isLeave;
};


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv/cvaux.h>
using namespace cv;


#include "Common/launcher.h"
#include "Common/protocol.h"
#include "Common/udpclient.h"
#include "Common/udpserver.h"
#include "Common/tcpserver.h"
#include "Common/tcpclient.h"


#ifdef _DEBUG
	#pragma comment(lib, "opencv_core2411d.lib")
	#pragma comment(lib, "opencv_highgui2411d.lib")
	#pragma comment(lib, "opencv_imgproc2411d.lib")
	#pragma comment(lib, "opencv_features2d2411d.lib")
	//#pragma comment(lib, "opencv_nonfree2411d.lib")
	#pragma comment(lib, "winmm.lib")
#else
	#pragma comment(lib, "opencv_core2411.lib")
	#pragma comment(lib, "opencv_highgui2411.lib")
	#pragma comment(lib, "opencv_imgproc2411.lib")
	#pragma comment(lib, "opencv_features2d2411.lib")
	//#pragma comment(lib, "opencv_nonfree2411.lib")
	#pragma comment(lib, "winmm.lib")
#endif

