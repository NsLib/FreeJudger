
#include "Process.h"
#include "../logger/Logger.h"

namespace IMUST
{

namespace
{

typedef int (__stdcall *pOldMessageBox)(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption,UINT uType);
pOldMessageBox g_pMessageBox = NULL;

HANDLE pBegin = GetModuleHandle(NULL);
PBYTE  pBegin2 = (PBYTE)pBegin;

PIMAGE_DOS_HEADER DOS = PIMAGE_DOS_HEADER(pBegin2);
PIMAGE_NT_HEADERS NT = PIMAGE_NT_HEADERS(pBegin2+DOS->e_lfanew);
PIMAGE_OPTIONAL_HEADER OPTION = &(NT->OptionalHeader);
PIMAGE_IMPORT_DESCRIPTOR IMPORT = PIMAGE_IMPORT_DESCRIPTOR(OPTION->DataDirectory[1].VirtualAddress + pBegin2);

int __stdcall HookMessageBox(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption,UINT uType)
{
    OJCerr << GetOJString("HOOK OK") << std::endl;
    return 0;
}


int HookApi(const char *DllName, const char *FunName)
{
    while (IMPORT->Name)
    {
        char* OurDllName = (char*)(IMPORT->Name + pBegin2);
        if (0 == strcmpi(DllName , OurDllName))
            break;
        IMPORT++;
    }

    PIMAGE_IMPORT_BY_NAME  pImportByName = NULL;
    PIMAGE_THUNK_DATA   pOriginalThunk = NULL;
    PIMAGE_THUNK_DATA   pFirstThunk = NULL;

    pOriginalThunk = (PIMAGE_THUNK_DATA)(IMPORT->OriginalFirstThunk + pBegin2);
    pFirstThunk = (PIMAGE_THUNK_DATA)(IMPORT->FirstThunk + pBegin2);

    while (pOriginalThunk->u1.Function)
    {
        DWORD u1 = pOriginalThunk->u1.Ordinal;
        if ((u1 & IMAGE_ORDINAL_FLAG) != IMAGE_ORDINAL_FLAG)
        {
            pImportByName = (PIMAGE_IMPORT_BY_NAME)((DWORD)pOriginalThunk->u1.AddressOfData + pBegin2);
            char* OurFunName = (char*)(pImportByName->Name);
            if (0 == strcmpi(FunName,OurFunName))
            {
                MEMORY_BASIC_INFORMATION mbi_thunk;
                VirtualQuery(pFirstThunk, &mbi_thunk, sizeof(MEMORY_BASIC_INFORMATION));
                DWORD dwOLD;
                VirtualProtect(pFirstThunk,sizeof(DWORD),PAGE_READWRITE,&dwOLD);
                g_pMessageBox =(pOldMessageBox)(pFirstThunk->u1.Function);
                pFirstThunk->u1.Function = (DWORD)HookMessageBox;        
                VirtualProtect(pFirstThunk,sizeof(DWORD),dwOLD,0);
                break;
            }
        }
        pOriginalThunk++;
        pFirstThunk++;
    }
    return 0;
}

WindowsProcessInOut::WindowsProcessInOut(const OJString &inputFileName,
										const OJString &outputFileName) :
	inputFileHandle_(NULL),
	outputFileHandle_(NULL),
	inputFileName_(inputFileName),
	outputFileName_(outputFileName)
{
    HookApi("User32.dll","MessageBoxW");
    MessageBox(NULL,L"没有HOOK到",L"HOOK",MB_OK);
}

WindowsProcessInOut::~WindowsProcessInOut()
{
	if (inputFileHandle_)
		::CloseHandle(inputFileHandle_);
	if (outputFileHandle_)
		::CloseHandle(outputFileHandle_);
}

HANDLE WindowsProcessInOut::createInputFile()
{
	if (inputFileName_.empty())
		return NULL;

	SECURITY_ATTRIBUTES saAttr = {0};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE handle = CreateFileW(
		inputFileName_.c_str(),
		GENERIC_READ,          	
		FILE_SHARE_READ,       	
		&saAttr,                  
		OPEN_EXISTING,         	
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (!handle)
	{
		ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
		logger->logTrace(GetOJString("[process] - IMUST::WindowsProcessInOut::createInputFile can't creat file"));
	}

	return handle;
}

HANDLE WindowsProcessInOut::createOutputFile()
{
    if (outputFileName_.empty())
        return NULL;

    SECURITY_ATTRIBUTES saAttr = {0};
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE handle = CreateFileW(
        outputFileName_.c_str(),
        GENERIC_WRITE, 
        FILE_SHARE_READ,
        &saAttr,                   	
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

	if (!handle)
	{
		ILogger *logger = LoggerFactory::getLogger(LoggerId::AppInitLoggerId);
		logger->logTrace(GetOJString("[process] - IMUST::WindowsProcessInOut::createOutputFile can't creat file"));
	}

    return handle;
}

WindowsJob::WindowsJob() :
	jobHandle_(NULL)
{

}

WindowsJob::~WindowsJob()
{
	if (jobHandle_)
		::CloseHandle(jobHandle_);
}

bool WindowsJob::create(LPSECURITY_ATTRIBUTES lpJobAttributes)
{
	if (jobHandle_)
		return false;
	
	jobHandle_ = CreateJobObjectW(lpJobAttributes, NULL);

	return INVALID_HANDLE_VALUE != jobHandle_;
}


DWORD WindowsJob::wait(DWORD time)
{
	return WaitForSingleObject(jobHandle_, time);
}

bool WindowsJob::terminate(DWORD exitCode)
{
	return !!TerminateJobObject(jobHandle_, exitCode);
}

bool WindowsJob::setInformation(JOBOBJECTINFOCLASS infoClass,
								LPVOID lpInfo,
								DWORD cbInfoLength)
{
	return !!SetInformationJobObject(jobHandle_, infoClass, lpInfo, cbInfoLength);
}

bool WindowsJob::assinProcess(HANDLE handel)
{
	return !!AssignProcessToJobObject(jobHandle_, handel);
}

}	// namespace

Mutex WindowsProcess::s_mutex_;
ULONG WindowsProcess::s_id_ = 0;

WindowsProcess::WindowsProcess(
	const OJString &inputFileName,
	const OJString &outputFileName) :
	WindowsProcessInOut(inputFileName, outputFileName),
    processHandle_(NULL),
	threadHandle_(NULL),
	iocpHandle_(NULL)
{

}

WindowsProcess::~WindowsProcess()
{
    if (processHandle_)
        ::CloseHandle(processHandle_);
	if (threadHandle_)
		::CloseHandle(threadHandle_);
	if (iocpHandle_)
		::CloseHandle(iocpHandle_);
}

OJInt32_t WindowsProcess::create(const OJString &cmd,
								const OJInt32_t timeLimit,
								const OJInt32_t memoryLimit,
								bool startImmediately)
{
	OJInt64_t limitTime = timeLimit * 10000; // ms
	int limitMemory = memoryLimit * 1024; //bytes

	if (limitMemory < 0)	//超出int范围了
		limitMemory = 128 * 1024 * 1024; //默认128M

	if (!jobHandle_.create())
		return -1;

	//设置基本限制信息
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION subProcessLimitRes;
	ZeroMemory(&subProcessLimitRes, sizeof(subProcessLimitRes));


	JOBOBJECT_BASIC_LIMIT_INFORMATION & basicInfo = subProcessLimitRes.BasicLimitInformation;
	basicInfo.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_TIME| \
		JOB_OBJECT_LIMIT_PRIORITY_CLASS| \
		JOB_OBJECT_LIMIT_PROCESS_MEMORY| \
		JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION;
	basicInfo.PriorityClass = NORMAL_PRIORITY_CLASS;      //优先级为默认
	basicInfo.PerProcessUserTimeLimit.QuadPart = limitTime; //子进程执行时间ns(1s=10^9ns)
	subProcessLimitRes.ProcessMemoryLimit = limitMemory;    //内存限制

	jobHandle_.setInformation(JobObjectExtendedLimitInformation,
							&subProcessLimitRes, 
							sizeof(subProcessLimitRes));

	//让完成端口发出时间限制的消息
	JOBOBJECT_END_OF_JOB_TIME_INFORMATION timeReport;
	ZeroMemory(&timeReport, sizeof(timeReport));
	timeReport.EndOfJobTimeAction = JOB_OBJECT_POST_AT_END_OF_JOB;

	jobHandle_.setInformation(
		JobObjectEndOfJobTimeInformation, 
		&timeReport,
		sizeof(JOBOBJECT_END_OF_JOB_TIME_INFORMATION));


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

	jobHandle_.setInformation(
		JobObjectBasicUIRestrictions,
		&subProcessLimitUi,
		sizeof(subProcessLimitUi));

	//将作业关联到完成端口，以确定其运行情况，及退出的原因
	s_mutex_.lock();
	ULONG id = ++s_id_;
	s_mutex_.unlock();

	iocpHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, id, 0);

	JOBOBJECT_ASSOCIATE_COMPLETION_PORT jobCP;
	ZeroMemory(&jobCP, sizeof(jobCP));

	jobCP.CompletionKey = (PVOID)id;
	jobCP.CompletionPort = iocpHandle_;

	jobHandle_.setInformation(
		JobObjectAssociateCompletionPortInformation,
		&jobCP,
		sizeof(jobCP));

#define CMDLINE_BUFFER_SIZE 1024
	OJChar_t cmdline[CMDLINE_BUFFER_SIZE];
	wcscpy_s(cmdline, cmd.c_str());
#undef CMDLINE_BUFFER_SIZE

	STARTUPINFO   si;
	PROCESS_INFORMATION   pi;

	ZeroMemory(&si, sizeof(si));
	si.wShowWindow = SW_SHOWNORMAL;
	si.cb = sizeof(si); 
	ZeroMemory(&pi, sizeof(pi)); 

	bool res =  !!CreateProcess(NULL,    //   No module name (use command line).   
		cmdline, //   Command line.   
		NULL,    //   Process handle not inheritable.   
		NULL,    //   Thread handle not inheritable.   
		TRUE,    //   Set handle inheritance to ...
		CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB, //   No creation  flags.  
		NULL,    //   Use parent 's environment block.   
		NULL,    //   Use parent 's starting  directory.   
		&si,     //   Pointer to STARTUPINFO structure. 
		&pi);    //   Pointer to PROCESS_INFORMAT\ION structure.

	if (!res)
		return -1;

	processHandle_ = pi.hProcess;
	threadHandle_ = pi.hThread;

	if(startImmediately)
		start();

	return 1;
}

OJInt32_t WindowsProcess::start()
{
	if (!jobHandle_.assinProcess(processHandle_))
		return -1;

    ResumeThread(threadHandle_);

	DWORD ExecuteResult = -1;  
	ULONG completeKey;  
	LPOVERLAPPED processInfo;  
	bool done = false;  
	while(!done)  
	{  
		GetQueuedCompletionStatus(iocpHandle_,  
								&ExecuteResult,   
								&completeKey,   
								&processInfo,   
								INFINITE);  

		switch (ExecuteResult)   
		{  
		case JOB_OBJECT_MSG_NEW_PROCESS:    
			//MessageBoxW(NULL, L"JOB_OBJECT_MSG_NEW_PROCESS", L"", MB_OK);
			break;  

		case JOB_OBJECT_MSG_END_OF_JOB_TIME:  
			//MessageBoxW(NULL, L"Job time limit reached", L"", MB_OK); 
			exitCode_ = 1;  
			done = true;  
			break;  

		case JOB_OBJECT_MSG_END_OF_PROCESS_TIME:   
			//MessageBoxW(NULL, L"Job process time limit reached", L"", MB_OK); 
			exitCode_ = 1;  
			done = true;  
			break;  

		case JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT:   
			//MessageBoxW(NULL, L"Process exceeded memory limit", L"", MB_OK);  
			OJCerr <<GetOJString("Process exceeded memory limit") <<std::endl;
			exitCode_ = 2;  
			done = true;  
			break;  

		case JOB_OBJECT_MSG_JOB_MEMORY_LIMIT:   
			//MessageBoxW(NULL, L"Process exceeded job memory limit", L"", MB_OK);
			exitCode_ = 2;  
			done = true;  
			break;  

		case JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT:  
			//MessageBoxW(NULL, L"Too many active processes in job", L"", MB_OK);
			break;  

		case JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO:  
			//MessageBoxW(NULL, L"Job contains no active processes", L"", MB_OK);  
			done = true;  
			break;  

		case JOB_OBJECT_MSG_EXIT_PROCESS:   
			//MessageBoxW(NULL, L"Process terminated", L"", MB_OK);
			done = true;  
			break;  

		case JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS:   
			//MessageBoxW(NULL, L"Process terminated abnormally", L"", MB_OK); 
			exitCode_ = 3;  
			done = true;  
			break;  

		default:  
			//MessageBoxW(NULL, L"Unknown notification:", L"", MB_OK);
			exitCode_ = 99;  
			break;  
		}  
	}  

    jobHandle_.terminate();
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