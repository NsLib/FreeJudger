// DlgStart.cpp : implementation file
//

#include "stdafx.h"
#include "FreeJudger.h"
#include "DlgStart.h"
#include "afxdialogex.h"
#include "Common.h"

// CDlgStart dialog

IMPLEMENT_DYNAMIC(CDlgStart, CDialogEx)

CDlgStart::CDlgStart(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgStart::IDD, pParent)
{

}

CDlgStart::~CDlgStart()
{
}

void CDlgStart::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStart, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_START, &CDlgStart::OnBnClickedBtnStart)
    ON_BN_CLICKED(IDC_BTN_STOP, &CDlgStart::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CDlgStart message handlers


void CDlgStart::OnBnClickedBtnStart()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

    JudgeCorePtr core( new IMUST::JudgeCore() );
    if(!core->startService())
    {
        MessageBox(L"JudgeCore启动失败！", L"ERROR", MB_ICONSTOP);
        return;
    }

    setJudgeCore(core);
    GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
}


void CDlgStart::OnBnClickedBtnStop()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);

    JudgeCorePtr core = getJudgeCore();
    assert(core && "CDlgStart::OnBnClickedBtnStop");

    core->stopService();
    setJudgeCore(nullptr);
    GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}
