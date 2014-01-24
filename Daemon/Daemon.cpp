#include "Daemon.h"

#include <cstdlib>

#include "../judgerlib/platformlayer/PlatformLayer.h"

#include "../judgerlib/taskmanager/TaskManager.h"
#include "../judgerlib/sql/DBManager.h"
#include "../judgerlib/thread/Thread.h"
#include "../judgerlib/judgecore/JudgeCore.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    IMUST::JudgeCore judgeCore;
    if(!judgeCore.startService())
        return EXIT_FAILURE;

    system("pause");

    judgeCore.stopService();

    return EXIT_SUCCESS;
}
