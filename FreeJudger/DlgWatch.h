#pragma once


// CDlgWatch dialog

class CDlgWatch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWatch)

public:
	CDlgWatch(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWatch();

// Dialog Data
	enum { IDD = IDD_DLG_WATCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
