//
// 이미지 정보를 UDP를 통해 전달한다.
// cStreamingReceiver와 같이 동작한다.
// 이미지를 압축하거나, 원본 그대로 전송할 수 있다.
// 이미지 용량이 크면, 나눠서 전송한다.
//
// Protocol
// - (byte) id (이미지를 나타내는 id)
//	- (byte) chunk size (이미지 용량 때문에 나눠진 데이타 청크의 갯수)
//	- (byte) chunk index
//	- (byte) gray 0 : 1
//	- (byte) compressed 0 : 1
//	- (int) image size
//
#pragma once

#include "streaming.h"

namespace cvproc
{
	class cStreamingSender
	{
	public:
		cStreamingSender();
		virtual ~cStreamingSender();

		bool Init(const bool isUDP, const string &ip, const int port, 
			const bool isConvertGray=true, const bool isCompressed=true, const int jpgQuality=40);
		int Send(const cv::Mat &image);


	protected:
		int SendImage(const BYTE *data, const int len, const bool isGray, const bool isCompressed);
		bool SendSplit();


	public:
		network::cUDPClient m_udpClient;
		network::cTCPClient m_tcpClient;
		bool m_isConvertGray;
		bool m_isCompressed;
		int m_jpgCompressQuality;
		cv::vector<uchar> m_compBuffer;
		cv::Mat m_gray;
		char *m_sndBuffer;


	protected:
		enum STATE {
			NOTCONNECT,	// 서버에 접속되어 있지 않은 상태
			READY,	// 이미지 전송이 가능한 상태
			SPLIT,	// 이미지를 여러개의 청크로 나누어서 보내는 상태
		};
		STATE m_state;
		bool m_isUDP;	// udp/ip or tcp/ip

		// Send Split Parameters
		BYTE *m_imagePtr;
		int m_chunkId;
		int m_sendChunkIndex;
		int m_chunkCount;
		int m_imageBytes;
		int m_sendBytes;
	};
}
