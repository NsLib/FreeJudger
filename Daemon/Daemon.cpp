#include "Daemon.h"

#include <cstdlib>

#include "InitApp.h"
#include "JudgeCore.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    if (!IMUST::InitApp())
        return EXIT_FAILURE;

    IMUST::JudgeCore judgeCore;
    if(!judgeCore.startService())
        return EXIT_FAILURE;

    system("pause");

    judgeCore.stopService();

    return EXIT_SUCCESS;
}
