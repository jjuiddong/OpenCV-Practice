//
// 영상에서 사각형을 나타내는 4점을 인식해서, 4점을 중심으로 카메라의 회전을 인식해
// 타겟을 이동시키는 프로그램이다.
//
// 4점의 중심으로 타겟을 향하게 한다.
//

#include "global.h"
#include <iostream>
#include "detectrectpoint.h"
#include "rotatecamera.h"



using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	g_camera.Init(0, Size(g_config.m_cameraWidth, g_config.m_cameraHeight));

	g_screen.InitResolution(Rect(0, 0, g_config.m_screenWidth, g_config.m_screenHeight));
	g_screen.DetectScreen(g_camera, g_config.m_screenWidth * g_config.m_screenHeight * 0.1f);
	const Mat transmtx = g_screen.SkewTransform(g_config.m_cameraWidth, g_config.m_cameraHeight);

	vector<Point2f> points(4);
	points[0] = Point2f(420, 320);
	points[1] = Point2f(604, 320);
	points[2] = Point2f(604, 448);
	points[3] = Point2f(420, 448);
	g_screen.ShowPoints(points);
	cvWaitKey(100);

	cDetectRectPoint detectRectPoint;
	detectRectPoint.Detect(g_camera, transmtx);

	cRotateCamera rotCamera;
	rotCamera.DetectCameraRotate(g_camera, detectRectPoint.GetDetectPoints(), transmtx);

	return 0;
}
