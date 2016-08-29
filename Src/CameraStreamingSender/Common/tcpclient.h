//
// 2015-11-28, jjuiddong
//
// TCP/IP 프로토콜을 이용해서 통신하는 객체다.
// 최대한 심플하게 만들었다.
//
#pragma once

#include "packetqueue.h"


namespace network
{
	class cTCPClient
	{
	public:
		cTCPClient();
		virtual ~cTCPClient();

		bool Init(const string &ip, const int port, 
			const int packetSize = 512, const int maxPacketCount = 10, const int sleepMillis = 30);
		void Send(BYTE *buff, const int len);
		void Close();
		bool IsConnect() const;


		string m_ip;
		int m_port;
		bool m_isConnect;
		SOCKET m_socket;
		int m_maxBuffLen;
		cPacketQueue m_sendQueue;
		cPacketQueue m_recvQueue;
		
		HANDLE m_handle;
		DWORD m_threadId;
		bool m_threadLoop;
		int m_sleepMillis;
	};


	inline bool cTCPClient::IsConnect() const { return m_isConnect;  }
}
