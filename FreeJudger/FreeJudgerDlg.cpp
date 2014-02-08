
// FreeJudgerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FreeJudger.h"
#include "FreeJudgerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFreeJudgerDlg dialog




CFreeJudgerDlg::CFreeJudgerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFreeJudgerDlg::IDD, pParent)
    , m_nTabSelect(-1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CFreeJudgerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}

BEGIN_MESSAGE_MAP(CFreeJudgerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CFreeJudgerDlg::OnSelchangeTab1)
END_MESSAGE_MAP()


// CFreeJudgerDlg message handlers

BOOL CFreeJudgerDlg::OnInitDialog()
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
    m_tabCtrl.InsertItem(0, L"启动");
    m_dlgStart.Create(IDD_DLG_START, &m_tabCtrl);
    m_tabDlgs.push_back(&m_dlgStart);

    m_tabCtrl.InsertItem(1, L"监视");
    m_dlgWatch.Create(IDD_DLG_WATCH, &m_tabCtrl);
    m_tabDlgs.push_back(&m_dlgWatch);

    m_tabCtrl.InsertItem(2, L"设置");
    m_dlgSetup.Create(IDD_DLG_SETUP, &m_tabCtrl);
    m_tabDlgs.push_back(&m_dlgSetup);
    
    CRect rc;
    m_tabCtrl.GetClientRect(&rc);
    rc.top += 22;
    rc.bottom -= 2;
    rc.left += 2;
    rc.right -= 4;

    for(std::vector<CDialog*>::iterator it = m_tabDlgs.begin();
        it != m_tabDlgs.end(); ++it)
    {
        (*it)->MoveWindow(&rc);
    }

    TurnToTab(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFreeJudgerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFreeJudgerDlg::OnPaint()
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
HCURSOR CFreeJudgerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFreeJudgerDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    *pResult = 0;

    size_t CurSel = m_tabCtrl.GetCurSel();
    TurnToTab(CurSel);
}

void CFreeJudgerDlg::TurnToTab(size_t index)
{
    if(m_nTabSelect == index) return;
    if(index >= m_tabDlgs.size()) return;

    m_nTabSelect = index;

    for(size_t i = 0; i < m_tabDlgs.size(); ++i)
    {
        m_tabDlgs[i]->ShowWindow(i == index ? SW_SHOW : SW_HIDE);
    }
}
