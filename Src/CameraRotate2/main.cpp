//
// CameraRotate2
//
// 영상에서 선을 나타내는 n개의 점을 인식한 후, 그 선을 중심으로 카메라의 회전을 인식해서
// 타겟을 이동시키는 프로그램이다.
//
// 선의 중점을 기준으로 타겟이 이동한다.
//
//
#include "global.h"
#include <iostream>
#include "rotatecamera.h"
#include "detectlinepoint.h"
#include "linecontour.h"



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
	points[1] = Point2f(470, 320);
	points[2] = Point2f(520, 320);
	points[3] = Point2f(570, 320);
	g_screen.ShowPoints(points);
	cvWaitKey(100);

	cDetectLinePoint detectLinePoint;
	detectLinePoint.Detect(g_camera, transmtx);

 	cRotateCamera rotCamera;
	cLineContour linePoints(detectLinePoint.GetDetectPoints());
	rotCamera.DetectCameraRotate(g_camera, linePoints.Center(), transmtx);

	return 0;
}
