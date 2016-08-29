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
using std::map;
using std::string;
using std::pair;

double angle(cv::Point pt1, const cv::Point &pt2, const cv::Point &pt0);
void setLabel(cv::Mat& im, const std::string &label, const std::vector<cv::Point>& contour, const cv::Scalar &color=cv::Scalar(0,0,0));
void horizLines(cv::Mat &image, const int w, const int h, const int div);
void verticalLines(cv::Mat &image, const int w, const int h, const int div);
void chessboard(cv::Mat &image, const int w, const int h, const int rows, const int cols);
cv::Mat SkewTransform(vector<cv::Point> &contours, const int width, const int height, const float scale);

void OrderedContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst);
void OrderedLineContours(const vector<cv::Point> &src, OUT vector<cv::Point> &dst);
void DrawLines(cv::Mat &dst, const vector<cv::Point> &lines, const cv::Scalar &color=cv::Scalar(0,0,0), 
	const int thickness=1, const bool isLoop=true);


#include "config.h"
#include "camera.h"
extern cCamera g_camera;

#include "screen.h"
extern cScreen g_screen;

#include "rectcontour.h"
#include "linecontour.h"
