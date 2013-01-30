#include "Daemon.h"

#include "InitApp.h"
#include "../judgerlib/taskmanager/TaskManager.h"
#include "../judgerlib/thread/Thread.h"
#include "Task.h"

bool g_sigExit = false;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


    IMUST::InitApp();

    IMUST::TaskManager taskManager;

    taskManager.lock();
    for (int i = 0; i < 10; ++i)
        taskManager.addTask(new IMUST::JudgeTask(i));
    taskManager.unlock();

    IMUST::Thread thread1(IMUST::JudgeThread(taskManager, 1));
    IMUST::Thread thread2(IMUST::JudgeThread(taskManager, 2));
    IMUST::Thread thread3(IMUST::JudgeThread(taskManager, 3));
    thread1.join();
    thread2.join();
    thread3.join();


    return 0;
}
