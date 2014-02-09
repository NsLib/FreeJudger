#pragma once

namespace IMUST
{
    typedef std::shared_ptr<class IValueProxy> ValueProxyPtr;
}

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

    afx_msg LRESULT OnDlgUpate(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
    int m_nThread;

    void onWatchNumThread(IMUST::ValueProxyPtr value);
    void onWatchNumJudgeTask(IMUST::ValueProxyPtr value);

    virtual BOOL OnInitDialog();
    int m_numJudgeTask;
};
