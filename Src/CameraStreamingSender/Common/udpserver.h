//
// v.1
// udp server, receive 기능
//
// v.2
// close(), waitforsingleobject
// isConnect()
// add variable m_isReceivData
// thread restart bug fix
//
#pragma once


namespace network
{

	class cUDPServer
	{
	public:
		cUDPServer();
		virtual ~cUDPServer();

		bool Init(const int id, const int port);
		void SetRecvData(const char *buff, const int buffLen);
		int GetRecvData(OUT char *dst, const int maxSize);
		void SetMaxBufferLength(const int length);
		void Close(const bool isWait = false);
		bool IsConnect() const;


		int m_id;
		SOCKET m_socket;
		int m_port;
		bool m_isConnect;
		char *m_buffer;
		int m_bufferLen;
		bool m_isReceiveData; // 패킷을 받았다면 true가 된다. GetRecvData() 함수에서 초기화 된다.
		int m_maxBuffLen;

		HANDLE m_handle;
		DWORD m_threadId;
		CRITICAL_SECTION m_CriticalSection;
		bool m_threadLoop;
	};


	inline bool cUDPServer::IsConnect() const { return m_isConnect; }
}
