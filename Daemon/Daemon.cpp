#include "Daemon.h"

#include <cstdlib>
#include <vector>

#include "../judgerlib/platformlayer/PlatformLayer.h"

#include "../judgerlib/taskmanager/TaskManager.h"
#include "../judgerlib/sql/DBManager.h"
#include "../judgerlib/thread/Thread.h"
#include "../judgerlib/judgecore/JudgeCore.h"
#include "../judgerlib/judgecore/InitApp.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    if(!IMUST::InitApp())
    {
        ::MessageBoxW(NULL, L"InitAppConfig failed!", L"ERROR", MB_ICONSTOP);
        return EXIT_FAILURE;
    }

    IMUST::JudgeCore judgeCore;
    if(!judgeCore.startService())
        return EXIT_FAILURE;

    system("pause");

    judgeCore.stopService();

    return EXIT_SUCCESS;
}
