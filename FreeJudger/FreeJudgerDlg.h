
// FreeJudgerDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "DlgSetup.h"
#include "DlgStart.h"
#include "DlgWatch.h"
#include <vector>

// CFreeJudgerDlg dialog
class CFreeJudgerDlg : public CDialogEx
{
// Construction
public:
	CFreeJudgerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FREEJUDGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
    CTabCtrl m_tabCtrl;
    CDlgStart m_dlgStart;
    CDlgSetup m_dlgSetup;
    CDlgWatch m_dlgWatch;

    size_t    m_nTabSelect;
    std::vector<CDialog*> m_tabDlgs;

    void TurnToTab(size_t index);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
