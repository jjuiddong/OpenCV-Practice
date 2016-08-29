
#include "global.h"
#include "maincamera.h"



cCapture::cCapture() :
	m_capture(NULL)
{
	
}

cCapture::~cCapture()
{
	cvReleaseCapture(&m_capture);
}


bool cCapture::Init()
{
	m_capture = cvCaptureFromCAM(0);

	return true;
}


IplImage* cCapture::GetCapture()
{
	if (!m_capture)
		return NULL;

	return cvQueryFrame(m_capture);
}
