
#pragma once


#include <dshow.h>



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

HRESULT WriteBitmap(PCWSTR, BITMAPINFOHEADER*, size_t, BYTE*, size_t);
HRESULT GrabVideoBitmap(PCWSTR pszVideoFile, PCWSTR pszBitmapFile);
void FreeMediaType(AM_MEDIA_TYPE& mt);
