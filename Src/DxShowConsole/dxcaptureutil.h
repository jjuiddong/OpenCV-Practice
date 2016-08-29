//
// dxcapture.h, cpp 에 필요한 전역 함수를 정의함.
//
#pragma once


#include <dshow.h>
#include <strsafe.h>

#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
//#include <qedit.h>
#include <C:\Program Files\Microsoft SDKs\Windows\v6.0\Include\qedit.h>


#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(x) { if (x) x->Release(); x = NULL; }
#endif


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}


HRESULT IsPinConnected(IPin *pPin, BOOL *pResult);
HRESULT IsPinDirection(IPin *pPin, PIN_DIRECTION dir, BOOL *pResult);
HRESULT MatchPin(IPin *pPin, PIN_DIRECTION direction, BOOL bShouldBeConnected, BOOL *pResult);
HRESULT FindUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
HRESULT ConnectFilters(
	IGraphBuilder *pGraph, // Filter Graph Manager.
	IPin *pOut,            // Output pin on the upstream filter.
	IBaseFilter *pDest)    // Downstream filter.
	;
HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IPin *pIn);
HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);

HRESULT WriteBitmap(const char*, BITMAPINFOHEADER*, size_t, BYTE*, size_t);
HRESULT GrabVideoBitmap(PCWSTR pszVideoFile, const char *pszBitmapFile);
void FreeMediaType(AM_MEDIA_TYPE& mt);
