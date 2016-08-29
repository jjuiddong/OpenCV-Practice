#pragma once


class cCapture
{
public:
	cCapture();
	virtual ~cCapture();

	bool Init();
	IplImage* GetCapture();


protected:
	CvCapture* m_capture;
};
