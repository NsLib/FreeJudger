#pragma once


// CDlgSetup dialog

class CDlgSetup : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetup)

public:
	CDlgSetup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSetup();

// Dialog Data
	enum { IDD = IDD_DLG_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnSetup();
    afx_msg void OnBnClickedBtnCancel();
};
