#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

/*
Simple udp client
Silver Moon (m00n.silv3r@gmail.com)
*/
#include<stdio.h>
#include<winsock2.h>
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
#pragma comment(lib,"winmm.lib") //Winsock Library

#define SERVER "127.0.0.1"  //ip address of udp server
//#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

struct SMGCameraData
{
	float x1, y1, x2, y2; // 총이 가르키는 위치 0 ~ 1 사이 값. 화면의 왼쪽 아래가 {0,0}
	int fire1; // 플레어1 격발, 1:격발, 0:Nothing~
	int fire2; // 플레어2 격발, 1:격발, 0:Nothing~

	// MachineGun Multi 전용.
	float steer; // 핸들 {-1 ~ 1 사이값}
	float accel; // 가속 페달 {0~1 사이값}
	float break0; // 브레이크 페달
	int reset; // 리셋, {1: 리셋}
	int backward; // 후진, {1:후진}
};

int main(void)
{
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	WSADATA wsa;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char *)&si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);
	si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

	//start communication
	int oldT = 0;
	int sendCount = 0;

	while (1)
	{
		int curT = timeGetTime();
		if (curT - oldT > 10)
		{
			oldT = curT;
			static bool isFire = true;

			SMGCameraData data;
			memset(&data, 0, sizeof(data));

			data.x1 = 0.5f;
			data.y1 = 0.5f;
			data.fire1 = isFire? 1 : 0;

			isFire = !isFire;

			//send the message
			if (sendto(s, (char*)&data, sizeof(data), 0, (struct sockaddr *) &si_other, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
//				exit(EXIT_FAILURE);
			}

			printf("sendto() %d\n", sendCount++);
		}
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
