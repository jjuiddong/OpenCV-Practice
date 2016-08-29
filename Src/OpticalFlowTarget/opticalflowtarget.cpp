
#include <iostream>
#include <windows.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv/cvaux.h>
#include "dxcapture.h"
#include "../../Source/Common/Common/common.h"

using namespace std;
using namespace common;

#ifdef _DEBUG
	#pragma comment(lib, "opencv_core2411d.lib")
	#pragma comment(lib, "opencv_highgui2411d.lib")
	#pragma comment(lib, "opencv_imgproc2411d.lib")
	#pragma comment(lib, "opencv_features2d2411d.lib")
	#pragma comment(lib, "opencv_video2411d.lib")
	#pragma comment(lib, "opencv_objdetect2411d.lib")
	//#pragma comment(lib, "opencv_nonfree2411d.lib")
	#pragma comment(lib, "winmm.lib")
#else
	#pragma comment(lib, "opencv_core2411.lib")
	#pragma comment(lib, "opencv_highgui2411.lib")
	#pragma comment(lib, "opencv_imgproc2411.lib")
	#pragma comment(lib, "opencv_features2d2411.lib")
	#pragma comment(lib, "opencv_video2411.lib")
	#pragma comment(lib, "opencv_objdetect2411.lib")
	//#pragma comment(lib, "opencv_nonfree2411.lib")
	#pragma comment(lib, "winmm.lib")
#endif

struct sFlowInfo
{
	Vector3 v;
	float len;
	int size;
};

enum STATE {
	CALIBRATE_X,
	CALIBRATE_Y,
	TARGET,
};


#define MAX_COUNT 20
#define MAX_DISTANCE 200
#define PI 3.1415
STATE g_state = TARGET;
Vector3 g_camPlane[4]; // left, top, right, bottom
float g_movScale = 1.f;

void ChangeState(STATE state);


void main()
{
	//////////////////////////////////////////////////////////////////////////
	//image class           
	IplImage* image = 0;
	//T, T-1 image     
	IplImage* current_Img = 0;
	IplImage* Old_Img = 0;
	//Optical Image     
	IplImage * imgA = 0;
	IplImage * imgB = 0;
	IplImage * grayA = 0;
	IplImage * grayB = 0;

	//Window     
	cvNamedWindow("Origin");
	//////////////////////////////////////////////////////////////////////////     

	//////////////////////////////////////////////////////////////////////////      
	//Optical Flow Variables      
	IplImage * eig_image = 0;
	IplImage * tmp_image = 0;
	int corner_count = MAX_COUNT;
	CvPoint2D32f * cornersA = new CvPoint2D32f[MAX_COUNT];
	CvPoint2D32f * cornersB = new CvPoint2D32f[MAX_COUNT];

	//CvSize img_sz;
	int win_size = 20;

	IplImage* pyrA = 0;
	IplImage* pyrB = 0;

	char features_found[MAX_COUNT];
	float feature_errors[MAX_COUNT];
	//////////////////////////////////////////////////////////////////////////     

	cDxCapture dxCapture;
	if (!dxCapture.Init(0, 640, 480, true))
		return;


	Vector3 targetPos;
	g_state = TARGET;
	ZeroMemory(g_camPlane, sizeof(g_camPlane));

	//////////////////////////////////////////////////////////////////////////     
	bool loop = true;
	int oldT = timeGetTime();
	int frame = 0;

	//Routine Start     
	while (loop)
	{
		++frame;
		const int curT = timeGetTime();
		if (curT - oldT > 1000)
		{
			cout << "frame = " << frame << endl;
			frame = 0;
			oldT = curT;
		}



		//Image Create     
		if (Old_Img == 0)
		{
			//image = cvRetrieveFrame(capture);
			image = dxCapture.GetCloneBufferToImage();
			if (!image)
				continue;

			current_Img = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
			Old_Img = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
		}

		if (eig_image == 0)
		{
			eig_image = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
			tmp_image = cvCreateImage(cvSize(image->width, image->height), image->depth, image->nChannels);
		}

		//copy to image class     
		memcpy(Old_Img->imageData, current_Img->imageData, sizeof(char)*image->imageSize);
		image = dxCapture.GetCloneBufferToImage();
		memcpy(current_Img->imageData, image->imageData, sizeof(char)*image->imageSize);

		//////////////////////////////////////////////////////////////////////////     
		//Create image for Optical flow     
		if (imgA == 0)
		{
			imgA = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
			imgB = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
		}
		if (grayA == 0)
		{
			grayA = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
			grayB = cvCreateImage(cvSize(image->width, image->height), IPL_DEPTH_8U, 1);
		}

		//RGB to Gray for Optical Flow
		cvCvtColor(current_Img, grayA, CV_BGR2GRAY);
		cvCvtColor(Old_Img, grayB, CV_BGR2GRAY);
		cvThreshold(grayA, imgA, 200, 255, CV_THRESH_BINARY);
		cvThreshold(grayB, imgB, 200, 255, CV_THRESH_BINARY);

		//
		cvGoodFeaturesToTrack(imgA, eig_image, tmp_image, cornersA, &corner_count, 0.01f, 5.0f, 0, 3, 0, 0.04f);
		corner_count = min(MAX_COUNT, corner_count);
		cvFindCornerSubPix(imgA, cornersA, corner_count, cvSize(win_size, win_size), cvSize(-1, -1),
			cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03f));

		CvSize pyr_sz = cvSize(imgA->width + 8, imgB->height / 3);
		if (pyrA == 0)
		{
			pyrA = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);
			pyrB = cvCreateImage(pyr_sz, IPL_DEPTH_32F, 1);
		}

		cvCalcOpticalFlowPyrLK(imgA, imgB, pyrA, pyrB, cornersA, cornersB, corner_count,
			cvSize(win_size, win_size), 5, features_found, feature_errors,
			cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3), 0);


		/////////////////////////////////////////////////////////////////////////
		vector<sFlowInfo> vecs;

		for (int i = 0; i < corner_count; ++i)
		{
			if (features_found[i] == 0 || feature_errors[i] > MAX_DISTANCE)
				continue;
			cvLine(image, cvPoint((int)cornersA[i].x, (int)cornersA[i].y),
				cvPoint((int)cornersB[i].x, (int)cornersB[i].y), CV_RGB(0, 255, 0), 2);

			//------------------------------------------------------------------------
			// ¿Ãµø ∫§≈Õ ∆Ú±’¿ª ±∏«—¥Ÿ.
			const Vector3 v1(cornersA[i].x, cornersA[i].y, 0);
			const Vector3 v2(cornersB[i].x, cornersB[i].y, 0);
			Vector3 v = v2 - v1;
			const float len = v.Length();
			v.Normalize();

			if (vecs.empty())
			{
				vecs.push_back({ v, len, 1 });
			}
			else
			{
				bool isFind = false;
				for (u_int i = 0; i < vecs.size(); ++i)
				{
					const float dot = vecs[i].v.DotProduct(v);
					if (dot > 0.9f)
					{
						vecs[i].v += v;
						vecs[i].len += len;
						vecs[i].size++;
						isFind = true;
						break;
					}
				}
				if (!isFind)
				{
					vecs.push_back({ v, len, 1 });
				}
			}
			//------------------------------------------------------------------------
		}

		const Vector3 center(640 / 2, 480 / 2, 0);
		const CvPoint cvCenter = { 640 / 2, 480 / 2};

		if (!vecs.empty())
		{
			int maxSizeIdx = 0;
			int maxSize = 0;
			for (u_int i = 0; i < vecs.size(); ++i)
			{
				if (maxSize < vecs[i].size)
				{
					maxSize = vecs[i].size;
					maxSizeIdx = i;
				}
			}

			Vector3 v = vecs[maxSizeIdx].v;
			v.Normalize();
			const float len = vecs[maxSizeIdx].len / vecs[maxSizeIdx].size;
			Vector3 to = center + v * vecs[maxSizeIdx].len / vecs[maxSizeIdx].size;
			cvLine(image, cvPoint((int)center.x, (int)center.y), cvPoint((int)to.x, (int)to.y),
				CV_RGB(255, 0, 0), 2);

			if (len > 2.f)
			{
				const float lpf = 0.9f;
				switch (g_state)
				{
				case CALIBRATE_X:
				{
					if (Vector3(1, 0, 0).DotProduct(v) < 0)
					{
						g_camPlane[0] = g_camPlane[0] * lpf + v*(1.f - lpf);
						g_camPlane[0].Normalize(); 
					}
					else
					{
						g_camPlane[2] = g_camPlane[2] * lpf + v*(1.f - lpf);
						g_camPlane[2].Normalize();
					}
				}
				break;

				case CALIBRATE_Y:
				{
					if (Vector3(0, -1, 0).DotProduct(v) > 0)
					{
						g_camPlane[1] = g_camPlane[1] * lpf + v*(1.f - lpf);
						g_camPlane[1].Normalize();
					}
					else
					{
						g_camPlane[3] = g_camPlane[3] * lpf + v*(1.f - lpf);
						g_camPlane[3].Normalize();
					}
				}
				break;

				case TARGET:
				{
					Vector3 axis[] = {
						Vector3(-1, 0, 0), // left
						Vector3(0, -1, 0), // top
						Vector3(1, 0, 0), // right
						Vector3(0, 1, 0), // bottom
					};

					for (int i = 0; i < 4; ++i)
					{
						if (g_camPlane[i].IsEmpty())
							continue;
						const float m = g_camPlane[i].DotProduct(v);
						if (m > 0)
							targetPos += axis[i] * m * len * g_movScale;
					}
				}
				break;
				}
			}

		}


		CvScalar colors[] = {
			CV_RGB(255, 0, 0),
			CV_RGB(0, 255, 0),
			CV_RGB(0, 0, 255),
			CV_RGB(255, 2255, 0),
		};
		for (int i = 0; i < 4; ++i)
		{
			if (!g_camPlane[i].IsEmpty())
			{
				Vector3 to = center + g_camPlane[i] * 300;
				CvPoint p0 = { (int)center.x, (int)center.y };
				CvPoint p1 = { (int)to.x, (int)to.y };
				cvLine(image, p0, p1, colors[i], 2);
			}
		}


		// Render Target
		const Vector3 target = center + targetPos;
		cvCircle(image, cvPoint((int)target.x, (int)target.y), 10, cvScalar(255, 255, 255));


		//////////////////////////////////////////////////////////////////////////
		cvShowImage("Origin", image);


		//////////////////////////////////////////////////////////////////////////     
		const int key = cvWaitKey(100);
		switch (key)
		{
		case VK_ESCAPE: loop = false; break;
		case '1': ChangeState(CALIBRATE_X); break;
		case '2': ChangeState(CALIBRATE_Y); break;
		case '3': g_movScale += 0.1f; cout << "mov scale = " << g_movScale << endl;  break;
		case '4': g_movScale -= 0.1f; cout << "mov scale = " << g_movScale << endl; break;
		case ' ': targetPos = { 0, 0, 0 }; break;
		default:
			break;
		}
	}

	//close the window
	cvDestroyAllWindows();

	cvReleaseImage(&current_Img);
	cvReleaseImage(&Old_Img);
	cvReleaseImage(&imgA);
	cvReleaseImage(&imgB);
	cvReleaseImage(&grayA);
	cvReleaseImage(&grayB);
	cvReleaseImage(&eig_image);
	cvReleaseImage(&tmp_image);
	delete[] cornersA;
	delete[] cornersB;
	cvReleaseImage(&pyrA);
	cvReleaseImage(&pyrB);
}


void ChangeState(STATE state)
{
	STATE prev = g_state;
	switch (state)
	{
	case CALIBRATE_X: g_state = (CALIBRATE_X == g_state) ? TARGET : CALIBRATE_X; break;
	case CALIBRATE_Y: g_state = (CALIBRATE_Y == g_state) ? TARGET : CALIBRATE_Y; break;
	default:
		break;
	}
	cout << "ChangeState " << prev << " -> " << g_state << endl;
}
