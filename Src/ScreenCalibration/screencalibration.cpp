//
// 화면 휘어짐을 보정하는 알고리즘.
//

#include "global.h"
#include "screendetect.h"
#include "screencalib.h"
#include "maincamera.h"
#include "screen.h"
#include "pointdetectcalibration.h"

using namespace cv;


cCapture g_camera;
cScreen g_screen;


//void drawRectangle(Mat &image, const )
int main(int argc, char* argv[])
{
	while (ShowCursor(false) >= 0);

 	g_camera.Init();
 	cvWaitKey(300);
 
 	g_screen.InitResolution(Rect(0, 0, 1024, 768));
 
	vector<cv::Point> screenLines(4);
	const Rect screenRect = ScreenDetectManual(screenLines, 50000, g_screenThreshold);
	g_screen.Init(Rect(0, 0, 1024, 768), screenLines);

 	ScreenCalibration(g_screen.GetScreenContour());

	DetectPointCalibration();

	return 0;
}
