
#include "global.h"
#include "screen.h"
#include <iostream>
#include "maincamera.h"

using namespace cv;


cScreen::cScreen() :
	m_cellCols(0)
	, m_cellRows(0)
{
}

cScreen::~cScreen()
{
}


// 스크린 해상도와 영상에서 인식된 스크린의 크기를 설정한다.
void cScreen::Init(const cv::Rect &screenResolution, const vector<cv::Point> &screenContour,
	const int increaseScreenSize)
{
	m_screenResolution = screenResolution;
	m_screenContour = screenContour;
	m_increaseScreenLength = increaseScreenSize;
 	if (m_warpScreen.empty())
		m_warpScreen = Mat(screenResolution.height, screenResolution.width, CV_8UC(3));

	// 인식된 스크린에서, 위아래로 크기를 늘린다.
	m_screenContour[0].x -= increaseScreenSize;
	m_screenContour[0].y -= increaseScreenSize;
	m_screenContour[1].x += increaseScreenSize;
	m_screenContour[1].y -= increaseScreenSize;
	m_screenContour[2].x += increaseScreenSize;
	m_screenContour[2].y += increaseScreenSize;
	m_screenContour[3].x -= increaseScreenSize;
	m_screenContour[3].y += increaseScreenSize;
}


// 스크린 해상도 설정.
void cScreen::InitResolution(const cv::Rect &screenResolution)
{
	m_screenResolution = screenResolution;
	if (m_warpScreen.empty())
		m_warpScreen = Mat(screenResolution.height, screenResolution.width, CV_8UC(3));
}


// 화면에 주사하는 스크린의 해상도의 너비를 리턴한다.
int cScreen::GetWidth()
{
	return m_screenResolution.width;
}


// 화면에 주사하는 스크린의 해상도의 높이를 리턴한다.
int cScreen::GetHeight()
{
	return m_screenResolution.height;
}


// 영상에서 인식된 스크린의 너비를 리턴한다.
int cScreen::GetRecogWidth()
{
	return m_screenContour[1].x - m_screenContour[0].x - (m_increaseScreenLength * 2);
}


// 영상에서 인식된 스크린의 높이를 리턴한다.
int cScreen::GetRecogHeight()
{
	return m_screenContour[3].y - m_screenContour[0].y - (m_increaseScreenLength * 2);
}


// 화면 해상도 와 실제 인식한 영상간의 비율을 리턴한다.
// width, height 순서대로 리턴한다.
pair<float, float> cScreen::GetResolutionRecognitionRate()
{
	const float rate = (float)(GetRecogWidth() * GetRecogHeight()) / (float)(GetWidth() * GetHeight());
	//return pair<float, float>(
	//	(float)GetRecogWidth() / (float)GetWidth(),
	//	(float)GetRecogHeight() / (float)GetHeight());
	return pair<float, float>(rate, rate);
}


// 매핑 테이블 생성
void cScreen::CreateCellMappingTable(const int cols, const int rows)
{
	m_cellCols = cols;
	m_cellRows = rows;
	m_cellMapping.resize(cols*rows);
	for (u_int i = 0; i < m_cellMapping.size(); ++i)
	{
		m_cellMapping[i].errorCount = 0;
		m_cellMapping[i].errorLength = Point(0, 0);
	}
}


// 매핑 테이블 설정.
void cScreen::SetCellMappingTable(const int index, const cv::Rect &screenCell,
	const vector<cv::Point> &cameraCell)
{
	if (index >= (int)m_cellMapping.size())
		return;

	m_cellMapping[index].screenCell = screenCell;
	m_cellMapping[index].cameraCell = cameraCell;
}


void sortCorners(std::vector<cv::Point2f>& corners, cv::Point2f center)
{
	std::vector<cv::Point2f> top, bot;

	for (u_int i = 0; i < corners.size(); i++)
	{
		if (corners[i].y < center.y)
			top.push_back(corners[i]);
		else
			bot.push_back(corners[i]);
	}

	cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
	cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
	cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
	cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];

	corners.clear();
	corners.push_back(tl);
	corners.push_back(tr);
	corners.push_back(br);
	corners.push_back(bl);
}



void cScreen::CalculateCellMapping()
{
	//for each (auto table in m_cellMapping)
	for (u_int i = 0; i < m_cellMapping.size(); ++i)
	{
		auto &table = m_cellMapping[i];

		vector<Point2f> cells(4);
// 		cells[0] = Point2f((float)table.screenCell.x, (float)table.screenCell.y);
// 		cells[1] = Point2f((float)table.screenCell.x + table.screenCell.width, (float)table.screenCell.y);
// 		cells[2] = Point2f((float)(table.screenCell.x + table.screenCell.width), (float)(table.screenCell.y + table.screenCell.height));
// 		cells[3] = Point2f((float)table.screenCell.x, (float)(table.screenCell.y + table.screenCell.height));

		cells[0] = Point2f(0, 0);
		cells[1] = Point2f((float)table.screenCell.width, (float)0);
		cells[2] = Point2f((float)table.screenCell.width, (float)table.screenCell.height);
		cells[3] = Point2f((float)0, (float)table.screenCell.height);

		table.screenCell2.resize(4);
		table.screenCell2[0] = Point(table.screenCell.x, table.screenCell.y);
		table.screenCell2[1] = Point(table.screenCell.x + table.screenCell.width, table.screenCell.y);
		table.screenCell2[2] = Point((table.screenCell.x + table.screenCell.width), (table.screenCell.y + table.screenCell.height));
		table.screenCell2[3] = Point(table.screenCell.x, (table.screenCell.y + table.screenCell.height));


// 		{
// 			// Get mass center
// 			cv::Point2f center(0, 0);
// 			for (u_int i = 0; i < cells.size(); i++)
// 				center += cells[i];
// 
// 			center *= (1. / cells.size());
// 			sortCorners(cells, center);
// 		}


		vector<Point2f> cameraCell(4);
		cameraCell[0] = Point2f((float)table.cameraCell[0].x, (float)table.cameraCell[0].y);
		cameraCell[1] = Point2f((float)table.cameraCell[1].x, (float)table.cameraCell[1].y);
		cameraCell[2] = Point2f((float)table.cameraCell[2].x, (float)table.cameraCell[2].y);
		cameraCell[3] = Point2f((float)table.cameraCell[3].x, (float)table.cameraCell[3].y);

// 		{
// 			// Get mass center
// 			cv::Point2f center(0, 0);
// 			for (u_int i = 0; i < cameraCell.size(); i++)
// 				center += cameraCell[i];
// 
// 			center *= (1. / cameraCell.size());
// 			sortCorners(cameraCell, center);
// 		}
// 

		table.tm = getPerspectiveTransform(cameraCell, cells);
		//table.tm = getPerspectiveTransform(cells, cameraCell);
		//table.tm = getAffineTransform(cameraCell, cells);
// 
// 		//warpPerspective()
// 
// 		// test
// 		Mat src(3/*rows*/, 1 /* cols */, CV_64F);
// 		src.at<double>(0, 0) = cameraCell[0].x;
// 		src.at<double>(1, 0) = cameraCell[0].y;
// 		src.at<double>(2, 0) = 1.0f;
// 		Mat dst = table.tm * src; //USE MATRIX ALGEBRA 
// 		Point screenPos((int)dst.at<double>(0, 0), (int)dst.at<double>(1, 0));
// 
// 		std::cout << src << std::endl;
// 		printf("screenPos = %d, %d\n", screenPos.x, screenPos.y);
	}

}


// 위치 pos가 영역 contour 안에 있다면 true를 리턴한다.
// pos 와 contour 중점 간의 거리를 out에 저장되어 리턴된다.
bool cScreen::IsInContour(const vector<cv::Point> &contour, const cv::Point2f &pos, OUT float &out)
{
	
	return pointPolygonTest(contour, pos, 0) > 0;
	//return true;
}


// point 위치에 해당하는 m_cellMapping[] 배열의 인덱스를 리턴한다.
int cScreen::GetCellIndex(const cv::KeyPoint &point)
{
	// 	const int cameraW = m_screenContour[1].x - m_screenContour[0].x - (m_increaseScreenLength * 2);
	// 	const int cameraH = m_screenContour[3].y - m_screenContour[0].y - (m_increaseScreenLength * 2);
	const int cameraW = m_screenContour[1].x - m_screenContour[0].x;
	const int cameraH = m_screenContour[3].y - m_screenContour[0].y;
	const int cellW = cameraW / m_cellCols;
	const int cellH = cameraH / m_cellRows;
	// 	const float x = point.pt.x - m_increaseScreenLength;
	// 	const float y = point.pt.y - m_increaseScreenLength;
	const float x = point.pt.x;
	const float y = point.pt.y;

	const int cellX = (int)(x / (float)cellW);
	const int cellY = (int)(y / (float)cellH);

	int index = -1;
	for (u_int i = 0; i < m_cellMapping.size(); ++i)
	{
		float length = 0;
		if (IsInContour(m_cellMapping[i].cameraCell, Point2f(x, y), length))
		{
			index = i;
			break;
		}
	}

	if (index < 0)
	{
		index = (cellY * m_cellCols) + cellX;
		if ((int)m_cellMapping.size() <= index)
			return -1;
	}

	return index;
}


// 영상에서 얻은 포인터의 위치를 실제 스크린에서의 위치로 변환해서 리턴한다.
cv::Point cScreen::GetPointPos(const cv::KeyPoint &point)
{
	/*
// 	const int cameraW = m_screenContour[1].x - m_screenContour[0].x - (m_increaseScreenLength * 2);
// 	const int cameraH = m_screenContour[3].y - m_screenContour[0].y - (m_increaseScreenLength * 2);
	const int cameraW = m_screenContour[1].x - m_screenContour[0].x;
	const int cameraH = m_screenContour[3].y - m_screenContour[0].y;
	const int cellW = cameraW / m_cellCols;
	const int cellH = cameraH / m_cellRows;
// 	const float x = point.pt.x - m_increaseScreenLength;
// 	const float y = point.pt.y - m_increaseScreenLength;
	const float x = point.pt.x;
	const float y = point.pt.y;

	const int cellX = (int)(x / (float)cellW);
	const int cellY = (int)(y / (float)cellH);

// 	const int index = (cellY * m_cellCols) + cellX;
// 	if ((int)m_cellMapping.size() <= index)
// 		return Point(0, 0);


	int index = -1;
	for (u_int i = 0; i < m_cellMapping.size(); ++i)
	{
		float length=0;
		if (IsInContour(m_cellMapping[i].cameraCell, Point2f(x, y), length))
		{
			index = i;
			break;
		}
	}

	if (index < 0)
	{
		index = (cellY * m_cellCols) + cellX;
		if ((int)m_cellMapping.size() <= index)
			return Point(0, 0);
	}
*/
	const int index = GetCellIndex(point);
	if (index < 0)
		return Point(0, 0);

	vector<float> pos(3);
	pos[0] = point.pt.x;
	pos[1] = point.pt.y;
	pos[2] = 1;


	Mat src(3/*rows*/, 1 /* cols */, CV_64F);
	src.at<double>(0, 0) = point.pt.x;
	src.at<double>(1, 0) = point.pt.y;
	src.at<double>(2, 0) = 1.0f;
	Mat dst = m_cellMapping[index].tm * src; //USE MATRIX ALGEBRA 
	//return Point2f(dst.at<double>(0, 0), dst.at<double>(1, 0));


	//MatExpr expr = m_cellMapping[index].tm.mul(pos);
// 	Mat m = m_cellMapping[index].tm.mul(pos);
// 	getAffineTransform()
	//vector<float> ret(3);
	//cvMul(m_cellMapping[index].tm, pos, &ret);

	Point screenPos((int)dst.at<double>(0, 0), (int)dst.at<double>(1, 0));
	screenPos += Point(m_cellMapping[index].screenCell.x, m_cellMapping[index].screenCell.y);
	screenPos += Point((int)m_cellMapping[index].errorLength.x, (int)m_cellMapping[index].errorLength.y);
	return screenPos;
}


cv::Point cScreen::GetPointPos(const cv::KeyPoint &point, const int index)
{
	if (index < 0)
		return Point(0, 0);

	vector<float> pos(3);
	pos[0] = point.pt.x;
	pos[1] = point.pt.y;
	pos[2] = 1;


	Mat src(3/*rows*/, 1 /* cols */, CV_64F);
	src.at<double>(0, 0) = point.pt.x;
	src.at<double>(1, 0) = point.pt.y;
	src.at<double>(2, 0) = 1.0f;
	Mat dst = m_cellMapping[index].tm * src; //USE MATRIX ALGEBRA 
	//return Point2f(dst.at<double>(0, 0), dst.at<double>(1, 0));


	//MatExpr expr = m_cellMapping[index].tm.mul(pos);
	// 	Mat m = m_cellMapping[index].tm.mul(pos);
	// 	getAffineTransform()
	//vector<float> ret(3);
	//cvMul(m_cellMapping[index].tm, pos, &ret);

	Point screenPos((int)dst.at<double>(0, 0), (int)dst.at<double>(1, 0));
	screenPos += Point(m_cellMapping[index].screenCell.x, m_cellMapping[index].screenCell.y);
	screenPos += Point((int)m_cellMapping[index].errorLength.x, (int)m_cellMapping[index].errorLength.y);
	return screenPos;

}


// 와핑된 스크린을 출력한다.
Mat& cScreen::GetWarpScreen(cCapture &capture)
{
// 	const int cellW = GetWidth() / m_cellCols;
// 	const int cellH = GetHeight() / m_cellRows;
	Mat src(capture.GetCapture());
	
	int cnt = 0;
	for each(auto &table in m_cellMapping)
	{
		++cnt;
		if (cnt > 5)
			break;

		Mat quad(table.screenCell.height, table.screenCell.width, CV_8UC(3));
		warpPerspective(src, quad, table.tm, quad.size());

		quad.copyTo(m_warpScreen);

	}

	return m_warpScreen;
}



void cScreen::SetMappingError(const cv::KeyPoint &point, const cv::Point &errorPos)
{
	const int index = GetCellIndex(point);
	if (index < 0)
		return;

	if (m_cellMapping[index].errorCount == 0)
	{
		m_cellMapping[index].errorCount = 1;
		m_cellMapping[index].errorLength = errorPos;
	}
	else
	{
		if (m_cellMapping[index].errorCount < 100)
		{
			++m_cellMapping[index].errorCount;
			m_cellMapping[index].errorLength += (Point2f)errorPos;
		}

		const float lowpass_alpha = 1.f / 100.f;
// 		Point2f pos1 = m_cellMapping[index].errorLength * (1.f - lowpass_alpha);
// 		Point2f pos2 = Point2f((float)errorPos.x, (float)errorPos.y) * lowpass_alpha;
// 		m_cellMapping[index].errorLength = pos1 + pos2;		
	}	
}

