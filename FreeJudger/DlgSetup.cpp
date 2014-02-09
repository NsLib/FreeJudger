// DlgSetup.cpp : implementation file
//

#include "stdafx.h"
#include "FreeJudger.h"
#include "DlgSetup.h"
#include "afxdialogex.h"


// CDlgSetup dialog

IMPLEMENT_DYNAMIC(CDlgSetup, CDialogEx)

CDlgSetup::CDlgSetup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgSetup::IDD, pParent)
{

}

CDlgSetup::~CDlgSetup()
{
}

void CDlgSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSetup, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_SETUP, &CDlgSetup::OnBnClickedBtnSetup)
    ON_BN_CLICKED(IDC_BTN_CANCEL, &CDlgSetup::OnBnClickedBtnCancel)
END_MESSAGE_MAP()


// CDlgSetup message handlers


void CDlgSetup::OnBnClickedBtnSetup()
{
    // TODO: Add your control notification handler code here
}


void CDlgSetup::OnBnClickedBtnCancel()
{
    // TODO: Add your control notification handler code here
}
