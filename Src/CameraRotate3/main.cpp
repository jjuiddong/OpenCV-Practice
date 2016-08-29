// 
// CameraRotate3
//
// 영상에서 선을 나타내는 n개의 점을 인식한 후, 그 선을 중심으로 카메라의 회전을 인식해서
// 타겟을 이동시키는 프로그램이다.
//
// 영상에서 인식한 선을 기준으로 skew transform을 생성한다.
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

	vector<Point2f> points(4);
	points[0] = Point2f(420, 320);
	points[1] = Point2f(470, 320);
	points[2] = Point2f(520, 320);
	points[3] = Point2f(570, 320);
	g_screen.ShowPoints(points, Size2f(40,40));
	cvWaitKey(100);

	// 단위 행렬
	const Mat identityTM = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);

	// 화면상에 선을 이루는 점들을 인식한다.
	cDetectLinePoint detectLinePoint;
	detectLinePoint.Detect(g_camera, identityTM);

	// 선을 토대로 skew transform을 구한다.
	const Mat transmtx = detectLinePoint.SkewTransform(g_config.m_cameraWidth, g_config.m_cameraHeight, 500, 400);

	// skew transform으로 다시 선을 이루는 점들을 재인식 한다.
	detectLinePoint.Detect(g_camera, transmtx);

	// 선의 중점을 기준으로 카메라 이동을 계산한다.
 	cRotateCamera rotCamera;
 	cLineContour linePoints(detectLinePoint.GetDetectPoints());
	rotCamera.DetectCameraRotate(g_camera, linePoints.Center(), transmtx);

	return 0;
}
