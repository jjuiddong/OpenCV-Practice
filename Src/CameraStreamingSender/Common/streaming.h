//
// 스트리밍에 관련된 전역을 선언한다.
//
#pragma once


namespace cvproc
{
	struct sStreamingData
	{
		unsigned char id;			// streaming id (같은 아이디끼리 데이타를 합친 후에 출력한다.)
		unsigned char chunkSize;	// chunk size (이미지 용량 때문에 나눠진 데이타 청크의 갯수)
		unsigned char chunkIndex;	// chunk index
		unsigned char isGray;		// gray 0 : 1
		unsigned char isCompressed;	// jpeg compressed 0 : 1
		int imageBytes;				// image size (byte unit)
		char *data;					// image data
	};


	const static int g_maxStreamSize = (int)pow(2, 15) - 1;
}
