//
// DirectShow를 이용해 카메라 영상을 가져오는 기능을 한다.
//
// Thread Safe Version
//
#pragma once

#include "dxcaptureutil.h"
#include <opencv2/core/core.hpp>


class cDxCapture
{
public:
	cDxCapture();
	virtual ~cDxCapture();

	bool Init(const int cameraIndex, const int width, const int height, const bool isThreadMode=false);
	void UpdateCurrentBuffer();
	BYTE* GetCurrentBuffer(long &size);
	BYTE* GetCloneBuffer(long &size);
	void WriteBitmapFromCurrentBuffer(const char* fileName);
	HRESULT WriteBitmapFromBuffer(const char* fileName, const BYTE *buffer, const long bufferSize);
	HRESULT HandleGraphEvent(void);
	IplImage* GetCloneBufferToImage();
	

protected:
	HRESULT GetInterfaces(void);
	HRESULT CaptureVideo(const int cameraIndex=0);
	HRESULT FindCaptureDevice(const int cameraIndex, IBaseFilter ** ppSrcFilter);
	HRESULT SetupVideoWindow(void);
	HRESULT ChangePreviewState(int nShow);

	void Msg(TCHAR *szFormat, ...);
	void CloseInterfaces(void);
	void ResizeVideoWindow(void);

	// Remote graph viewing functions
	HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
	void RemoveGraphFromRot(DWORD pdwRegister);


public:
	enum PLAYSTATE { STOPPED, PAUSE, RUNNING, INIT};

	int m_cameraIndex;
	int m_width;
	int m_height;
	bool m_isThreadMode;
	bool m_isUpdateBuffer; // 버퍼가 업데이트 되면 true가 된다. GetCloneBuffer() 호출시 초기화 된다.
	bool m_isUnpluggedInit; // 카메라와 연결이 끊어진 후, 다시 연결되었을 때, true 가 된다.

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
	AM_MEDIA_TYPE m_mt;
	BYTE *m_pFrameBuffer;
	long m_frameBufferSize;
	BYTE *m_pCloneFrameBuffer;
	IplImage *m_iplImage;

	HANDLE m_handle;
	DWORD m_threadId;
	bool m_threadLoop;
	CRITICAL_SECTION m_criticalSection;
};

