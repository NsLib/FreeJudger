
// FreeJudger.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FreeJudger.h"
#include "FreeJudgerDlg.h"
#include "../judgerlib/judgecore/InitApp.h"
#include "../judgerlib/util/Watch.h"
#include "Common.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFreeJudgerApp

BEGIN_MESSAGE_MAP(CFreeJudgerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFreeJudgerApp construction

CFreeJudgerApp::CFreeJudgerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CFreeJudgerApp object

CFreeJudgerApp theApp;


// CFreeJudgerApp initialization

BOOL CFreeJudgerApp::InitInstance()
{
    IMUST::WatchTool::DoWatchTest();

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    //初始化应用程序
    if(!IMUST::InitApp())
    {
        ::MessageBoxW(NULL, L"InitAppConfig failed!", L"ERROR", MB_ICONSTOP);
        return FALSE;
    }

	CFreeJudgerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

    //安全释放评判内核
    JudgeCorePtr core = getJudgeCore();
    if(core && core->isRunning())
        core->stopService();

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

