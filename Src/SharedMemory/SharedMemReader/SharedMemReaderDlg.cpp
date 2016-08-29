// SharedMemReaderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SharedMemReader.h"
#include "SharedMemReaderDlg.h"
#include "afxdialogex.h"
#include <mmsystem.h>
#include <string>

using std::string;
using std::wstring;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//------------------------------------------------------------------------
// 멀티바이트 문자를 유니코드로 변환
//------------------------------------------------------------------------
std::wstring str2wstr(const std::string &str)
{
	int len;
	int slength = (int)str.length() + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

//------------------------------------------------------------------------
// 유니코드를 멀티바이트 문자로 변환
//------------------------------------------------------------------------
std::string wstr2str(const std::wstring &wstr)
{
	const int slength = (int)wstr.length() + 1;
	const int len = ::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, 0, 0, NULL, FALSE);
	char* buf = new char[len];
	::WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), slength, buf, len, NULL, FALSE);
	std::string r(buf);
	delete[] buf;
	return r;
}


CSharedMemReaderDlg::CSharedMemReaderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSharedMemReaderDlg::IDD, pParent)
{
	m_loop = true;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSharedMemReaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_READ, m_ListBox);
}

BEGIN_MESSAGE_MAP(CSharedMemReaderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSharedMemReaderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSharedMemReaderDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSharedMemReaderDlg message handlers

BOOL CSharedMemReaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here


	try
	{
		m_sharedmem = shared_memory_object(open_only, "killing_space_camera", read_only);

		// setting the size of the shared memory
		//sharedmem1.truncate(65536);

		// map the shared memory to current process 
		m_mmap = mapped_region(m_sharedmem, read_only, 0, 256);

		// access the mapped region using get_address
//		strcpy(static_cast<char*>(mmap.get_address()), "Hello World!\n");

	}
	catch (interprocess_exception&e)
	{
		std::stringstream ss;
		ss << e.what() << std::endl;
		ss << "Shared Memory Error!!";
		::AfxMessageBox( str2wstr(ss.str()).c_str() );
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSharedMemReaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSharedMemReaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSharedMemReaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSharedMemReaderDlg::OnBnClickedOk()
{
	m_loop = false;
	CDialogEx::OnOK();
}


void CSharedMemReaderDlg::OnBnClickedCancel()
{
	m_loop = false;
	CDialogEx::OnCancel();
}


void CSharedMemReaderDlg::MainLoop()
{
	//메시지 구조체
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	int oldT = 0;
	while (m_loop)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const int curT = timeGetTime();
		const int deltaT = curT - oldT;
		if (deltaT > 100)
		{
			oldT = curT;


			try
			{
				// 공유메모리 정보를 읽는다.
				char *pstr = static_cast<char*>(m_mmap.get_address());
				if (pstr)
				{
					const wstring str = str2wstr(pstr);
					m_ListBox.AddString(str.c_str());

					if (m_ListBox.GetCount() > 100)
						m_ListBox.DeleteString(0);

					m_ListBox.SetTopIndex(m_ListBox.GetCount() - 1);
				}
			}
			catch (interprocess_exception&e)
			{
				std::stringstream ss;
				ss << e.what() << std::endl;
				ss << "Shared Memory Error";
				::AfxMessageBox(str2wstr(ss.str()).c_str());
			}

		}
	}
}

