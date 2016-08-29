//
// DirectShow를 이용해 카메라 영상을 가져오는 기능을 한다.
//
#pragma once

#include "dxcaptureutil.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>



class cDxCapture
{
public:
	cDxCapture();
	virtual ~cDxCapture();

	bool Init(const int width, const int height);
	BYTE* GetCurrentBuffer(long &size);
	void WriteBitmapCurrentBuffer(const char* fileName);
	IplImage* GetBufferToImage();


protected:
	HRESULT GetInterfaces(void);
	HRESULT CaptureVideo();
	HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter);
	HRESULT SetupVideoWindow(void);
	HRESULT ChangePreviewState(int nShow);
	HRESULT HandleGraphEvent(void);

	void Msg(TCHAR *szFormat, ...);
	void CloseInterfaces(void);
	void ResizeVideoWindow(void);

	// Remote graph viewing functions
	HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void RemoveGraphFromRot(DWORD pdwRegister);


protected:
	enum PLAYSTATE { STOPPED, PAUSE, RUNNING, INIT};

	int m_width;
	int m_height;
	DWORD m_dwGraphRegister;
	IVideoWindow  * m_pVW;
	IMediaControl * m_pMC;
	IMediaEventEx * m_pME;
	IGraphBuilder * m_pGraph;
	ICaptureGraphBuilder2 * m_pCapture;
	IBaseFilter *m_pGrabberF;
	ISampleGrabber *m_pGrabber;
	IBaseFilter *m_pNullF;
	PLAYSTATE m_psCurrent;
	BYTE *m_pFrameBuffer;
	long m_frameBufferSize;
	IplImage *m_iplImage;
};

