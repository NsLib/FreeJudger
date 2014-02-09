// DlgWatch.cpp : implementation file
//

#include "stdafx.h"
#include "FreeJudger.h"
#include "DlgWatch.h"
#include "afxdialogex.h"

#include "Common.h"
#include "../judgerlib/util/Watch.h"

// CDlgWatch dialog

IMPLEMENT_DYNAMIC(CDlgWatch, CDialogEx)

CDlgWatch::CDlgWatch(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgWatch::IDD, pParent)
    , m_nThread(0)
    , m_numJudgeTask(0)
    , m_nProcess(0)
{

}

CDlgWatch::~CDlgWatch()
{
}

void CDlgWatch::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_TXT_NUM_THREAD, m_nThread);
    DDX_Text(pDX, IDC_TXT_NUM_TASK, m_numJudgeTask);
    DDX_Text(pDX, IDC_TXT_NUM_PROCESS, m_nProcess);
    DDX_Control(pDX, IDC_LST_WATCH, m_lstWatch);
}


BEGIN_MESSAGE_MAP(CDlgWatch, CDialogEx)
    ON_MESSAGE(WM_DLG_UPDATE, &CDlgWatch::OnDlgUpate)
END_MESSAGE_MAP()


// CDlgWatch message handlers

LRESULT CDlgWatch::OnDlgUpate(WPARAM wParam, LPARAM lParam)
{
    if(wParam != 0) UpdateData(TRUE);
    else UpdateData(FALSE);
    return 0;
}

void CDlgWatch::onWatchNumProcess(IMUST::ValueProxyPtr value)
{
    m_nProcess = value->asInt32();

    PostMessage(WM_DLG_UPDATE, 0, 0);
}

void CDlgWatch::onWatchNumThread(IMUST::ValueProxyPtr value)
{
    m_nThread = value->asInt32();

    PostMessage(WM_DLG_UPDATE, 0, 0);
}

void CDlgWatch::onWatchNumJudgeTask(IMUST::ValueProxyPtr value)
{
    m_numJudgeTask = value->asInt32();

    PostMessage(WM_DLG_UPDATE, 0, 0);
}

BOOL CDlgWatch::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here

    m_lstWatch.InsertColumn(0, L"id");
    m_lstWatch.InsertColumn(1, L"id");
    m_lstWatch.InsertColumn(2, L"id");

    using namespace IMUST::WatchTool;

    LockRoot();

    IMUST::IWatchListener * p;

    p = MakeWatchListener(this, &CDlgWatch::onWatchNumProcess);
    Root()->getWatch(OJStr("core/numProcess"))->addListener(p);

    p = MakeWatchListener(this, &CDlgWatch::onWatchNumThread);
    Root()->getWatch(OJStr("core/numThread"))->addListener(p);

    p = MakeWatchListener(this, &CDlgWatch::onWatchNumJudgeTask);
    Root()->getWatch(OJStr("core/numJudgeTask"))->addListener(p);

    UnlockRoot();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
