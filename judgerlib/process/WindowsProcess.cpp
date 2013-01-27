
#include "Process.h"

namespace IMUST
{

WindowsProcess::WindowsProcess() :
    processHandle_(NULL)
{

}

WindowsProcess::~WindowsProcess()
{
    if (processHandle_)
        ::CloseHandle(processHandle_);
}

OJInt32_t WindowsProcess::create(const OJString &cmd, bool startImmediately)
{
#define CMDLINE_BUFFER_SIZE 1024
    OJChar_t cmdline[CMDLINE_BUFFER_SIZE];
    wcscpy_s(cmdline, cmd.c_str());
#undef CMDLINE_BUFFER_SIZE

    STARTUPINFO   si;
    PROCESS_INFORMATION   pi; 

    ZeroMemory(&si, sizeof(si)); 
    si.cb = sizeof(si); 
    ZeroMemory(&pi, sizeof(pi)); 

    processHandle_ = pi.hProcess;

    if(startImmediately)
        start();

    return !!CreateProcess( NULL,   //   No module name (use command line).   
        cmdline,                     //   Command line.   
        NULL,                       //   Process handle not inheritable.   
        NULL,                       //   Thread handle not inheritable.   
        FALSE,                      //   Set handle inheritance to FALSE.   
        0,                          //   No creation  flags.   
        NULL,                       //   Use parent 's environment block.   
        NULL,                       //   Use parent 's starting  directory.   
        &si,                        //   Pointer to STARTUPINFO structure. 
        &pi   );                    //   Pointer to PROCESS_INFORMATION structure.
}

OJInt32_t WindowsProcess::start()
{
    ResumeThread(processHandle_);
    return true;
}

bool WindowsProcess::isRunning()
{
    return getExitCode() == STILL_ACTIVE;
}

OJInt32_t WindowsProcess::join(OJInt32_t time)
{
    if (isRunning())
        WaitForSingleObject(processHandle_, time); 

    return getExitCode();
}

OJInt32_t WindowsProcess::getExitCode()
{
    DWORD code = 0;
    GetExitCodeProcess(processHandle_, &code);
    return code;
}

void WindowsProcess::kill()
{
    TerminateProcess(processHandle_, -1);
}







}   // namespace IMUST