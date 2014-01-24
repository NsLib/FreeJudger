
#include "WindowsProcess.h"
#include "../logger/Logger.h"
#include "../util/StringTool.h"
#include "../config/AppConfig.h"


namespace IMUST
{

WindowsProcessInOut::WindowsProcessInOut(const OJString &inputFileName,
										const OJString &outputFileName) :
	inputFileHandle_(NULL),
	outputFileHandle_(NULL),
	inputFileName_(inputFileName),
	outputFileName_(outputFileName)
{

}

WindowsProcessInOut::~WindowsProcessInOut()
{
    SAFE_CLOSE_HANDLE_AND_RESET(inputFileHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(outputFileHandle_)
}

bool WindowsProcessInOut::createInputFile()
{
    SAFE_CLOSE_HANDLE_AND_RESET(inputFileHandle_);

	if (inputFileName_.empty())//如果文件名为空，表示不需要此句柄
		return true;

	SECURITY_ATTRIBUTES saAttr = {0};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

    inputFileHandle_ = CreateFileW(inputFileName_.c_str(),
                                GENERIC_READ,          	
                                FILE_SHARE_READ,       	
                                &saAttr,                  
                                OPEN_EXISTING,         	
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

	if (INVALID_HANDLE_VALUE == inputFileHandle_)
	{
        ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
        OJString msg(GetOJString("[process] - IMUST::WindowsProcessInOut::createInputFile - can't open input file: "));
        msg += inputFileName_;
        logger->logError(msg);
        inputFileHandle_ = NULL;
	}

	return inputFileHandle_ != NULL;
}

bool WindowsProcessInOut::createOutputFile()
{
    SAFE_CLOSE_HANDLE_AND_RESET(outputFileHandle_);

    if (outputFileName_.empty())//如果文件名为空，表示不需要此句柄
        return true;

    SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    outputFileHandle_ = CreateFileW(outputFileName_.c_str(),
                                GENERIC_WRITE, 
                                FILE_SHARE_READ,
                                &saAttr,                   	
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);

	if (INVALID_HANDLE_VALUE == outputFileHandle_)
	{
        ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
        OJString msg(GetOJString("[process] - IMUST::WindowsProcessInOut::createOutputFile - can't create output file: "));
        msg += outputFileName_;
        logger->logError(msg);
        outputFileHandle_ = NULL;
	}

    return outputFileHandle_ != NULL;
}

///////////////////////////////////////////////////////////////////////
///windows作业对象
///////////////////////////////////////////////////////////////////////

WindowsJob::WindowsJob() :
	jobHandle_(NULL), iocpHandle_(NULL)
{

}

WindowsJob::~WindowsJob()
{
    SAFE_CLOSE_HANDLE_AND_RESET(iocpHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(jobHandle_)
}

bool WindowsJob::create(LPSECURITY_ATTRIBUTES lpJobAttributes)
{
	if (jobHandle_)
		return false;
	
	jobHandle_ = CreateJobObjectW(lpJobAttributes, NULL);

	return NULL != jobHandle_;
}


DWORD WindowsJob::wait(DWORD time)
{
	return WaitForSingleObject(jobHandle_, time);
}

bool WindowsJob::terminate(DWORD exitCode)
{
	return !!TerminateJobObject(jobHandle_, exitCode);
}


bool WindowsJob::assinProcess(HANDLE handel)
{
	return !!AssignProcessToJobObject(jobHandle_, handel);
}

bool WindowsJob::setLimit(const OJInt32_t timeLimit,
                        const OJInt32_t memoryLimit)
{
    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

    //限制时间，单位为100ns。 1ms = 10的6次方ns = 10000 * 100ns。
    OJInt64_t   limitTime       = timeLimit * 10000;    // ms->100ns
    int         limitMemory     = memoryLimit;   //bytes

    if (limitMemory <= 0)	                //超出int范围了
        limitMemory = 128 * 1024 * 1024;    //默认128M

    //设置基本限制信息
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION subProcessLimitRes;
    ZeroMemory(&subProcessLimitRes, sizeof(subProcessLimitRes));

    JOBOBJECT_BASIC_LIMIT_INFORMATION & basicInfo = subProcessLimitRes.BasicLimitInformation;
    basicInfo.LimitFlags = \
        JOB_OBJECT_LIMIT_JOB_TIME | /*限制job时间*/ \
        JOB_OBJECT_LIMIT_PRIORITY_CLASS | /*限制job优先级*/  \
        JOB_OBJECT_LIMIT_JOB_MEMORY | /*限制job内存*/   \
        JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION /*遇到异常，让进程直接死掉。*/;
    basicInfo.PriorityClass = NORMAL_PRIORITY_CLASS;      //优先级为默认
    basicInfo.PerJobUserTimeLimit.QuadPart = limitTime; //子进程执行时间ns(1s=10^9ns)
    subProcessLimitRes.JobMemoryLimit = limitMemory;    //内存限制

    if (!setInformation(JobObjectExtendedLimitInformation, &subProcessLimitRes, sizeof(subProcessLimitRes)))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsJob::setLimit - can't set job JobObjectExtendedLimitInformation info"));
        return false;
    }
        
    //让完成端口发出时间限制的消息
    JOBOBJECT_END_OF_JOB_TIME_INFORMATION timeReport;
    ZeroMemory(&timeReport, sizeof(timeReport));
    timeReport.EndOfJobTimeAction = JOB_OBJECT_POST_AT_END_OF_JOB;//时间到了，通过管道发出信息。

    if (!setInformation(JobObjectEndOfJobTimeInformation, &timeReport, sizeof(JOBOBJECT_END_OF_JOB_TIME_INFORMATION)))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsJob::setLimit - can't set job JobObjectEndOfJobTimeInformation info"));
        return false;
    }

    //UI限制。禁止访问一些资源。
    JOBOBJECT_BASIC_UI_RESTRICTIONS subProcessLimitUi;
    ZeroMemory(&subProcessLimitUi, sizeof(subProcessLimitUi));
    subProcessLimitUi.UIRestrictionsClass = JOB_OBJECT_UILIMIT_NONE | \
        JOB_OBJECT_UILIMIT_DESKTOP| \
        JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS| \
        JOB_OBJECT_UILIMIT_DISPLAYSETTINGS| \
        JOB_OBJECT_UILIMIT_EXITWINDOWS| /*关机*/    \
        JOB_OBJECT_UILIMIT_GLOBALATOMS| \
        JOB_OBJECT_UILIMIT_HANDLES| \
        JOB_OBJECT_UILIMIT_READCLIPBOARD;

    if (!setInformation(JobObjectBasicUIRestrictions, &subProcessLimitUi, sizeof(subProcessLimitUi)))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsJob::setLimit - can't set job JobObjectBasicUIRestrictions info"));
        return false;
    }

    //将作业关联到完成端口，以确定其运行情况，及退出的原因。完成端口可理解为管道，job和应用程序分别位于管道的两端。
    //应用程序可以通过管道，查询job的工作状态。
    s_mutex_.lock();
    ULONG id = ++s_id_;
    s_mutex_.unlock();

    //创建完成端口
    iocpHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, id, 0);
    if (NULL == iocpHandle_)
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsJob::setLimit - create IOCP failed"));
        return false;
    }
    JOBOBJECT_ASSOCIATE_COMPLETION_PORT jobCP;
    ZeroMemory(&jobCP, sizeof(jobCP));
    jobCP.CompletionKey = (PVOID)id;
    jobCP.CompletionPort = iocpHandle_;
    if (!setInformation(JobObjectAssociateCompletionPortInformation, &jobCP, sizeof(jobCP)))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsJob::setLimit - can't set job JobObjectAssociateCompletionPortInformation info"));
        return false;
    }

    return true;
}

bool WindowsJob::getState(DWORD &executeResult, 
                        ULONG &completionKey, 
                        LPOVERLAPPED &processInfo, 
                        const DWORD time)
{
    return !!GetQueuedCompletionStatus(iocpHandle_, &executeResult, &completionKey, &processInfo, time);
}


bool WindowsJob::setInformation(JOBOBJECTINFOCLASS infoClass,
                                LPVOID lpInfo,
                                DWORD cbInfoLength)
{
    return !!SetInformationJobObject(jobHandle_, infoClass, lpInfo, cbInfoLength);
}

bool WindowsJob::queryInformation(JOBOBJECTINFOCLASS informationClass,
        LPVOID lpInformation,
        DWORD cbInformationLength,
        LPDWORD lpReturnLength)
{
    return !!QueryInformationJobObject(jobHandle_, informationClass, lpInformation,
        cbInformationLength, lpReturnLength);
}

ULONG   WindowsJob::s_id_(0);
Mutex   WindowsJob::s_mutex_;


///////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////

WindowsProcess::WindowsProcess(
	const OJString &inputFileName,
	const OJString &outputFileName) :
	WindowsProcessInOut(inputFileName, outputFileName),
    processHandle_(NULL),
	threadHandle_(NULL),
    exitCode_(ProcessExitCode::SystemError),
    alive_(false)
{

}

WindowsProcess::~WindowsProcess()
{
    SAFE_CLOSE_HANDLE_AND_RESET(processHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(threadHandle_)
}

bool WindowsProcess::createProcess(
    LPCTSTR lpApplicationName,
    LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation)
{
    return !!CreateProcessW(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);
}

OJInt32_t WindowsProcess::create(const OJString &cmd,
								const OJInt32_t timeLimit,
								const OJInt32_t memoryLimit,
								bool startImmediately)
{
    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

    {
        OJChar_t buffer[1024];
        OJSprintf(buffer, OJStr("[process] - IMUST::WindowsProcess::create CMD='%s' T=%dms, M=%dbytes"),
            cmd.c_str(), timeLimit, memoryLimit);
        logger->logTrace(buffer);
    }

    if(!createInputFile())
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsProcess::create - can't creat inputFile"));
        return -1;
    }

    if(!createOutputFile())
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsProcess::create - can't creat outputFile"));
        return -1;
    }

    if (!jobHandle_.create())
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsProcess::create - can't creat job"));
        return -1;
    }
    if (!jobHandle_.setLimit(timeLimit, memoryLimit))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsProcess::create - set job limit failed"));
        return -1;
    }
    
#define CMDLINE_BUFFER_SIZE 1024
	OJChar_t cmdline[CMDLINE_BUFFER_SIZE];
	wcscpy_s(cmdline, cmd.c_str());
#undef CMDLINE_BUFFER_SIZE

	STARTUPINFO   si;
	PROCESS_INFORMATION   pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi)); 
	
	si.cb = sizeof(si); 
	si.wShowWindow = SW_HIDE;//隐藏窗口
    si.hStdInput = inputFileHandle_;
    si.hStdOutput = si.hStdError = outputFileHandle_;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; //使用handel项和wShowWindow项。
	
	bool res =  createProcess(
        NULL,    //   No module name (use command line).   
		cmdline, //   Command line.   
		NULL,    //   Process handle not inheritable.   
		NULL,    //   Thread handle not inheritable.   
		TRUE,   //   Set handle inheritance to ...
		CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB, //   No creation  flags.  
		NULL,    //   Use parent 's environment block.   
		NULL,    //   Use parent 's starting  directory.   
		&si,     //   Pointer to STARTUPINFO structure. 
		&pi);    //   Pointer to PROCESS_INFORMAT\ION structure.

    
	if (!res)
    {
        OJChar_t buffer[256];
        OJSprintf(buffer, 
            OJStr("[process] - IMUST::WindowsProcess::create - can't creat process. last error: %u"), 
            GetLastError());
        logger->logError(buffer);
        return -1;
    }
		
    alive_ = true;
	processHandle_ = pi.hProcess;
	threadHandle_ = pi.hThread;

	if(startImmediately) start();

	return 1;
}

OJInt32_t WindowsProcess::start()
{
    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

    //加到作业中
	if (!jobHandle_.assinProcess(processHandle_))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsProcess::start - can't assign process to job"));
        return -1;
    }

    //起点线程
    ResumeThread(threadHandle_);
    
    //关闭不使用的句柄。让进程执行完毕后立即退出。
    SAFE_CLOSE_HANDLE_AND_RESET(inputFileHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(outputFileHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(threadHandle_)

    exitCode_ = ProcessExitCode::Success;

	DWORD ExecuteResult = -1;  
	ULONG completeKey;  
	LPOVERLAPPED processInfo;  
	bool done = false;  
	while(!done)  
	{
        if(!jobHandle_.getState(ExecuteResult, completeKey, processInfo))
        {
            DEBUG_MSG(OJStr("get job State faild!"));
            OJSleep(10);
            continue;
        }
        DWORD dwCode = (DWORD)processInfo;

		switch (ExecuteResult)   
		{  
		case JOB_OBJECT_MSG_NEW_PROCESS:    
            //DEBUG_MSG_VS(OJStr("[WindowsProcess]new process: %u"), dwCode);
			break;
		case JOB_OBJECT_MSG_END_OF_JOB_TIME: //job超时
            DEBUG_MSG(OJStr("[WindowsProcess]Job time limit reached")); 
            exitCode_ = ProcessExitCode::TimeLimited;  
			done = true;  
			break;  
		case JOB_OBJECT_MSG_END_OF_PROCESS_TIME:   //线程超时
			DEBUG_MSG(OJStr("[WindowsProcess]process time limit reached"));
			exitCode_ = ProcessExitCode::TimeLimited;  
			done = true;  
			break;  
		case JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT:   //进程内存超限
            DEBUG_MSG(OJStr("[WindowsProcess]Process exceeded memory limit"));  
			exitCode_ = ProcessExitCode::MemoryLimited;  
			done = true;  
			break;  
        case JOB_OBJECT_MSG_JOB_MEMORY_LIMIT: //job内存超限
            {
                OJInt32_t mem = getRunMemory();  
                DebugMessage(OJStr("[WindowsProcess]exceeded job memory limit with %dkb"), mem);
                exitCode_ = ProcessExitCode::MemoryLimited; 
                done = true;  
            }
			break;  
		case JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT:  //超出运行的进程数量
            DEBUG_MSG(OJStr("[WindowsProcess]Too many active processes in job"));
			break;  
		case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:  
            DEBUG_MSG(OJStr("[WindowsProcess]Job contains no active processes")); 
			done = true;  
			break;
		case JOB_OBJECT_MSG_EXIT_PROCESS: //进程退出
            //DEBUG_MSG_VS(OJStr("[WindowsProcess]Process %u exit."), dwCode);
            if(::GetProcessId(processHandle_) == dwCode)
			{
                done = true;
            }
			break;  
		case JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS: //进程异常结束
            DEBUG_MSG(OJStr("[WindowsProcess]Process terminated abnormally"));
            exitCode_ = ProcessExitCode::RuntimeError;  
			done = true;  
			break;  
		default:  
            DEBUG_MSG(OJStr("[WindowsProcess]Unknown notification"));
			exitCode_ = ProcessExitCode::UnknowCode;  
			break;  
		}  
	}  

    while(!jobHandle_.terminate())//强制关闭作业
    {
        DEBUG_MSG(OJStr("Terminate job faild!"));
        OJSleep(500);
    }

    alive_ = false;//进程结束

    //正常退出。即不是超时等状况。
    if(ProcessExitCode::Success == exitCode_)
    {
        DWORD code = getExitCode();//获取进程返回值，以判断进程是否执行成功。
        if(code != 0)
        {
            exitCode_ = ProcessExitCode::RuntimeError;
            DEBUG_MSG_VS(OJStr("process exit with code : %u, last error: %u"), 
                code, GetLastError());
        }
    }

	return 0;  
}

OJInt32_t WindowsProcess::getRunTime()
{
    JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION jobai;  
	ZeroMemory(&jobai, sizeof(jobai));  
	jobHandle_.queryInformation(JobObjectBasicAndIoAccountingInformation,   
		&jobai, sizeof(jobai), NULL);  

	return jobai.BasicInfo.TotalUserTime.LowPart/10000;  //转换成ms
}
    
OJInt32_t WindowsProcess::getRunMemory()
{
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION joeli;  
	ZeroMemory(&joeli, sizeof(joeli));  
	jobHandle_.queryInformation(JobObjectExtendedLimitInformation,   
		&joeli, sizeof(joeli), NULL);  
    return joeli.PeakProcessMemoryUsed/1024;  //转换成kb
}

bool WindowsProcess::isRunning()
{
    return alive_;

/*  考虑到程序的异常返回值可能会是STILL_ACTIVE，不能用下面的方法判断进程是否正在运行。
    感谢"成诺"发现此bug。
    return getExitCode() == STILL_ACTIVE;
*/
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

///////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////

WindowsUserProcess::WindowsUserProcess(WindowsUserPtr user,
    const OJString &inputFileName,
    const OJString &outputFileName)
    : WindowsProcess(inputFileName, outputFileName)
    , userPtr_(user)
{

}

WindowsUserProcess::~WindowsUserProcess()
{

}

bool WindowsUserProcess::createProcess(
        LPCTSTR lpApplicationName,
        LPTSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCTSTR lpCurrentDirectory,
        LPSTARTUPINFO lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation)
{
    if (!userPtr_)
    {
        throw(std::runtime_error("Invalid windows user !"));
    }

    lpStartupInfo->lpDesktop = OJStr("winsta0\\default");//设置交互桌面

    return userPtr_->createProcess(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation);
}

//////////////////////////////////////////////////////////////////////////

/*static*/ WindowsUserPtr  ProcessFactory::s_userPtr_;

ProcessFactory::ProcessFactory()
{

}

ProcessFactory::~ProcessFactory()
{

}

/*static*/ ProcessPtr ProcessFactory::create(int type,
    const OJString &inputFileName,
    const OJString &outputFileName)
{
    IProcess* pProcess = NULL;

    if (type == ProcessType::Normal)
    {
        pProcess = new WindowsProcess(inputFileName, outputFileName);
    }
    else if (type == ProcessType::WithJob)
    {
        pProcess = new WindowsProcess(inputFileName, outputFileName);
    }
    else if (type == ProcessType::WithUser)
    {
        if (AppConfig::WindowsUser::Enable)
        {
            pProcess = new WindowsUserProcess(s_userPtr_, inputFileName, outputFileName);
        }
        else
        {
            pProcess = new WindowsProcess(inputFileName, outputFileName);
        }
    }
    else
    {
        throw(std::invalid_argument("Invalid process type!"));
    }

    return ProcessPtr(pProcess);
}

/*static*/ void ProcessFactory::setWindowsUser(WindowsUserPtr user)
{
    s_userPtr_ = user;
}



}   // namespace IMUST