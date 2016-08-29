//
// 스크린에 관련 된 작업을 처리한다.
// 인식한 스크린의 크기와, 실제 영상으로 출력되는 스크린의 크기 비율을 관리한다.
// 
// 카메라의 렌즈 특성에 따른 스크린의 희어짐을 보정하는 역할을 한다.
//
#pragma once

struct sCellMapping
{
	cv::Rect screenCell; // 화면에서 출력하는 cell 위치
	vector<cv::Point> screenCell2; // 화면에서 출력하는 cell 위치
	vector<cv::Point> cameraCell; // 카메라에서 인식한 cell 위치
	cv::Point2f errorLength;  // 카메라에서 인식한 위치를 보정하기 위한 값.
	int errorCount; // errorLength 값을 계산하기 위한 카운트값 (로우패스필터)
	cv::Mat tm;
};


class cCapture;
class cScreen
{
public:
	cScreen();
	virtual ~cScreen();

	void Init(const cv::Rect &screenResolution, const vector<cv::Point> &screenContour, 
		const int increaseScreenSize = 25);

	void InitResolution(const cv::Rect &screenResolution);

	cv::Mat& GetWarpScreen(cCapture &capture);

	int GetWidth();
	int GetHeight();
	int GetRecogWidth();
	int GetRecogHeight();
	pair<float,float> GetResolutionRecognitionRate();

	const cv::Rect& GetScreenResolution() const;
	const vector<cv::Point>& GetScreenContour() const;

	void CreateCellMappingTable(const int cols, const int rows);
	void SetCellMappingTable(const int index, const cv::Rect &screenCell,
		const vector<cv::Point> &cameraCell);
	const vector<sCellMapping>& GetCellMappingTable() const;
	void CalculateCellMapping();
	int GetCellIndex(const cv::KeyPoint &point);
	cv::Point GetPointPos(const cv::KeyPoint &point);
	cv::Point GetPointPos(const cv::KeyPoint &point, const int index);
	void SetMappingError(const cv::KeyPoint &point, const cv::Point &errorPos);

	bool IsInContour(const vector<cv::Point> &contour, const cv::Point2f &pos, OUT float &out);


protected:
	cv::Rect m_screenResolution; // {0,0,w,h}
	vector<cv::Point> m_screenContour;	// 인식된 스크린의 크기 (인덱스 순서)
										// 0 ---------- 1
										// |            |
										// |            |
										// |            |
										// 2 ---------- 3
										//

	int m_increaseScreenLength; // 인식된 스크린에서 가로,세로 증가시킬 값을 저장한다.


	int m_cellCols;
	int m_cellRows;
	vector<sCellMapping> m_cellMapping; // 스크린에 출력하는 셀정보와 카메라에서 인식하는 셀정보를 저장하고 매핑한다.
	cv::Mat m_warpScreen;

};


inline const cv::Rect& cScreen::GetScreenResolution() const { return m_screenResolution; }
inline const vector<cv::Point>& cScreen::GetScreenContour() const { return m_screenContour; }
inline const vector<sCellMapping>& cScreen::GetCellMappingTable() const { return m_cellMapping; }
