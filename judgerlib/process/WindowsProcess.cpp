
#include "Process.h"
#include "../logger/Logger.h"


namespace IMUST
{

namespace
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

    OJInt64_t   limitTime       = timeLimit * 10000;    // ms
    int         limitMemory     = memoryLimit * 1024;   //bytes

    if (limitMemory <= 0)	                //超出int范围了
        limitMemory = 128 * 1024 * 1024;    //默认128M

    //设置基本限制信息
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION subProcessLimitRes;
    ZeroMemory(&subProcessLimitRes, sizeof(subProcessLimitRes));

    JOBOBJECT_BASIC_LIMIT_INFORMATION & basicInfo = subProcessLimitRes.BasicLimitInformation;
    basicInfo.LimitFlags = JOB_OBJECT_LIMIT_JOB_TIME| \
        JOB_OBJECT_LIMIT_PRIORITY_CLASS| \
        JOB_OBJECT_LIMIT_JOB_MEMORY| \
        JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
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
    timeReport.EndOfJobTimeAction = JOB_OBJECT_POST_AT_END_OF_JOB;

    if (!setInformation(JobObjectEndOfJobTimeInformation, &timeReport, sizeof(JOBOBJECT_END_OF_JOB_TIME_INFORMATION)))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsJob::setLimit - can't set job JobObjectEndOfJobTimeInformation info"));
        return false;
    }

    //UI限制
    JOBOBJECT_BASIC_UI_RESTRICTIONS subProcessLimitUi;
    ZeroMemory(&subProcessLimitUi, sizeof(subProcessLimitUi));
    subProcessLimitUi.UIRestrictionsClass = JOB_OBJECT_UILIMIT_NONE| \
        JOB_OBJECT_UILIMIT_DESKTOP| \
        JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS| \
        JOB_OBJECT_UILIMIT_DISPLAYSETTINGS| \
        JOB_OBJECT_UILIMIT_EXITWINDOWS| \
        JOB_OBJECT_UILIMIT_GLOBALATOMS| \
        JOB_OBJECT_UILIMIT_HANDLES| \
        JOB_OBJECT_UILIMIT_READCLIPBOARD;

    if (!setInformation(JobObjectBasicUIRestrictions, &subProcessLimitUi, sizeof(subProcessLimitUi)))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsJob::setLimit - can't set job JobObjectBasicUIRestrictions info"));
        return false;
    }

    //将作业关联到完成端口，以确定其运行情况，及退出的原因
    s_mutex_.lock();
    ULONG id = ++s_id_;
    s_mutex_.unlock();

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

ULONG   WindowsJob::s_id_(0);
Mutex   WindowsJob::s_mutex_;

}	// namespace


WindowsProcess::WindowsProcess(
	const OJString &inputFileName,
	const OJString &outputFileName) :
	WindowsProcessInOut(inputFileName, outputFileName),
    processHandle_(NULL),
	threadHandle_(NULL)
{

}

WindowsProcess::~WindowsProcess()
{
    SAFE_CLOSE_HANDLE_AND_RESET(processHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(threadHandle_)
}

OJInt32_t WindowsProcess::create(const OJString &cmd,
								const OJInt32_t timeLimit,
								const OJInt32_t memoryLimit,
								bool startImmediately)
{
    if(!createInputFile() || !createOutputFile())
    {
        return -1;
    }

    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
    logger->logTrace(GetOJString("[process] - IMUST::WindowsProcess::create"));

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
	si.wShowWindow = SW_HIDE;
    si.hStdInput = inputFileHandle_;
    si.hStdOutput = si.hStdError = outputFileHandle_;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	
	bool res =  !!CreateProcess(NULL,    //   No module name (use command line).   
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
        logger->logError(GetOJString("[process] - IMUST::WindowsProcess::create - can't creat process"));
        return -1;
    }
		

	processHandle_ = pi.hProcess;
	threadHandle_ = pi.hThread;

	if(startImmediately)
		start();

	return 1;
}

OJInt32_t WindowsProcess::start()
{
    ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);

	if (!jobHandle_.assinProcess(processHandle_))
    {
        logger->logError(GetOJString("[process] - IMUST::WindowsProcess::start - can't assign process to job"));
        return -1;
    }

    ResumeThread(threadHandle_);
    
    SAFE_CLOSE_HANDLE_AND_RESET(inputFileHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(outputFileHandle_)
    SAFE_CLOSE_HANDLE_AND_RESET(threadHandle_)


	DWORD ExecuteResult = -1;  
	ULONG completeKey;  
	LPOVERLAPPED processInfo;  
	bool done = false;  
	while(!done)  
	{
        jobHandle_.getState(ExecuteResult, completeKey, processInfo);

		switch (ExecuteResult)   
		{  
		case JOB_OBJECT_MSG_NEW_PROCESS:    
            DEBUG_MSG(OJStr("JOB_OBJECT_MSG_NEW_PROCESS"));
			break;
		case JOB_OBJECT_MSG_END_OF_JOB_TIME:  
            DEBUG_MSG(OJStr("Job time limit reached")); 
			exitCode_ = 1;  
			done = true;  
			break;  
		case JOB_OBJECT_MSG_END_OF_PROCESS_TIME:   
			DEBUG_MSG(OJStr("Job process time limit reached"));
			exitCode_ = 1;  
			done = true;  
			break;  
		case JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT:   
            DEBUG_MSG(OJStr("Process exceeded memory limit"));  
			exitCode_ = 2;  
			done = true;  
			break;  
		case JOB_OBJECT_MSG_JOB_MEMORY_LIMIT:   
            DEBUG_MSG(OJStr("Process exceeded job memory limit"));
			exitCode_ = 2;  
			done = true;  
			break;  
		case JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT:  
            DEBUG_MSG(OJStr("Too many active processes in job"));
			break;  
		case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:  
            DEBUG_MSG(OJStr("Job contains no active processes")); 
			done = true;  
			break;
		case JOB_OBJECT_MSG_EXIT_PROCESS:   
            DEBUG_MSG(OJStr("Process terminated"));
			done = true;  
			break;  
		case JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS:   
            DEBUG_MSG(OJStr("Process terminated abnormally"));
			exitCode_ = 3;  
			done = true;  
			break;  
		default:  
            DEBUG_MSG(OJStr("Unknown notification"));
			exitCode_ = 99;  
			break;  
		}  
	}  

    while(!jobHandle_.terminate())
    {
        DEBUG_MSG(OJStr("terminate job faild!"));
        Sleep(1000);
    }

    //SAFE_CLOSE_HANDLE_AND_RESET(processHandle_)
	/*
	JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION jobai;  
	ZeroMemory(&jobai, sizeof(jobai));  
	QueryInformationJobObject(m_job.handle(), JobObjectBasicAndIoAccountingInformation,   
		&jobai, sizeof(jobai), NULL);  

	JOBOBJECT_EXTENDED_LIMIT_INFORMATION joeli;  
	ZeroMemory(&joeli, sizeof(joeli));  
	QueryInformationJobObject(m_job.handle(), JobObjectExtendedLimitInformation,   
		&joeli, sizeof(joeli), NULL);  

	m_runTime = jobai.BasicInfo.TotalUserTime.LowPart/10000;  
	m_runMemory = joeli.PeakProcessMemoryUsed/1024;  
	*/

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