
#include <iostream>
#include "cli.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;


cCLI::cCLI()
{

}

cCLI::~cCLI()
{

}


// 
bool cCLI::ParseKey()
{
	const int key = waitKey(1);


	return true;
}