
// SharedMemReaderDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

using namespace boost::interprocess;


// CSharedMemReaderDlg dialog
class CSharedMemReaderDlg : public CDialogEx
{
// Construction
public:
	CSharedMemReaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SHAREDMEMREADER_DIALOG };
	void MainLoop();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	bool m_loop;
	shared_memory_object m_sharedmem;
	mapped_region m_mmap;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CListBox m_ListBox;
};
