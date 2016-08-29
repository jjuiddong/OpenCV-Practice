#pragma once


#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.h>
#include "opencv2/nonfree/nonfree.hpp"


using std::vector;
using std::pair;

double angle(cv::Point pt1, const cv::Point &pt2, const cv::Point &pt0);
void setLabel(cv::Mat& im, const std::string &label, const std::vector<cv::Point>& contour, const cv::Scalar &color=cv::Scalar(0,0,0));
void horizLines(cv::Mat &image, const int w, const int h, const int div);
void verticalLines(cv::Mat &image, const int w, const int h, const int div);


extern int g_screenThreshold;
// extern cv::Rect g_screenRect;
// extern vector<cv::Point> g_screenContour;

#include "maincamera.h"
extern cCapture g_camera;

#include "screen.h"
extern cScreen g_screen;
