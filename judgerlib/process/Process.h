
#include "../util/Utility.h"
#include "../platformlayer/PlatformLayer.h"
#include "../thread/Thread.h"

#include "WindowsUser.h"
#include <Windows.h>

namespace IMUST
{

namespace ProcessExitCode
{
    const OJInt32_t Success         = 0;
    const OJInt32_t SystemError     = 1;
    const OJInt32_t RuntimeError    = 2;
    const OJInt32_t TimeLimited     = 3;
    const OJInt32_t MemoryLimited   = 4;
    const OJInt32_t UnknowCode      = 99;
}

namespace ProcessType
{
    const OJInt32_t Normal      = 0;
    const OJInt32_t WithJob     = 1;
    const OJInt32_t WithUser    = 2;
}

class JUDGER_API IProcess
{
    MAKE_CLASS_UNCOPYABLE(IProcess);

public:
    IProcess();
    virtual ~IProcess();

    virtual OJInt32_t create(const OJString &cmd,
							const OJInt32_t timeLimit,
							const OJInt32_t memoryLimit,
							bool startImmediately = true) = 0;
    virtual OJInt32_t start() = 0;
    virtual bool isRunning() = 0;
    virtual OJInt32_t join(OJInt32_t time) = 0;
    virtual OJInt32_t getExitCode() = 0;
    virtual void kill() = 0;

    virtual OJInt32_t getExitCodeEx() = 0;
    virtual OJInt32_t getRunTime() = 0;
    virtual OJInt32_t getRunMemory() = 0;
};

typedef std::shared_ptr<IProcess> ProcessPtr;

namespace
{

class JUDGER_API WindowsProcessInOut : public IProcess
{
public:
	WindowsProcessInOut(const OJString &inputFileName,
						const OJString &outputFileName);
	virtual ~WindowsProcessInOut();

protected:
    virtual bool createInputFile();
    virtual bool createOutputFile();

protected:
    HANDLE		inputFileHandle_;
    HANDLE		outputFileHandle_;
    OJString	inputFileName_;
    OJString	outputFileName_;
};

class JUDGER_API WindowsJob
{
    MAKE_CLASS_UNCOPYABLE(WindowsJob);

public:
	WindowsJob();
	~WindowsJob();

	bool create(LPSECURITY_ATTRIBUTES lpJobAttributes = NULL);
	DWORD wait(DWORD time = INFINITE);
	bool terminate(DWORD exitCode = 4);
	bool assinProcess(HANDLE handel);
    bool setLimit(const OJInt32_t timeLimit,
                const OJInt32_t memoryLimit);
    bool getState(DWORD &executeResult, 
                ULONG &completionKey, 
                LPOVERLAPPED &processInfo, 
                const DWORD time = INFINITE);
    bool queryInformation(JOBOBJECTINFOCLASS informationClass,
        LPVOID lpInformation,
        DWORD cbInformationLength,
        LPDWORD lpReturnLength);

private:
    bool setInformation(JOBOBJECTINFOCLASS infoClass,
        LPVOID lpInfo,
        DWORD cbInfoLength);

private:
	HANDLE      jobHandle_;
    HANDLE		iocpHandle_;

private:
    static	Mutex		s_mutex_;
    static	ULONG		s_id_;	
};

}	// namespace

class JUDGER_API WindowsProcess : public WindowsProcessInOut
{
public:
	WindowsProcess(const OJString &inputFileName = GetOJString(""),
					const OJString &outputFileName = GetOJString(""));
    virtual ~WindowsProcess();

    virtual OJInt32_t create(const OJString &cmd,
							const OJInt32_t timeLimit,
							const OJInt32_t memoryLimit,
							bool startImmediately = true);
    virtual OJInt32_t start();
    virtual bool isRunning();
    virtual OJInt32_t join(OJInt32_t time);
    virtual OJInt32_t getExitCode();
    virtual void kill();
    OJInt32_t getExitCodeEx(){ return exitCode_; }
    OJInt32_t getRunTime();
    OJInt32_t getRunMemory();

protected:

    /**
     * 重载此函数，以创建不同类型的进程。 
     */
    virtual bool createProcess(
        LPCTSTR lpApplicationName,
        LPTSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCTSTR lpCurrentDirectory,
        LPSTARTUPINFO lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation);

private:
    HANDLE		processHandle_;
	HANDLE		threadHandle_;
	WindowsJob	jobHandle_;
	OJInt32_t	exitCode_;


private:
	static	Mutex		s_mutex_;
	static	ULONG		s_id_;	
};

/**
 *  使用低权限的Windows用户创建进程
 */
class JUDGER_API WindowsUserProcess : public WindowsProcess
{
public:
    WindowsUserProcess(WindowsUserPtr user,
        const OJString &inputFileName = GetOJString(""),
        const OJString &outputFileName = GetOJString(""));

    virtual ~WindowsUserProcess();

protected:

    virtual bool createProcess(
        LPCTSTR lpApplicationName,
        LPTSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCTSTR lpCurrentDirectory,
        LPSTARTUPINFO lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation);

private:

    WindowsUserPtr   userPtr_;
};


class ProcessFactory
{
    MAKE_CLASS_UNCOPYABLE(ProcessFactory);
public:
    ProcessFactory();
    ~ProcessFactory();

    static ProcessPtr create(int type,
        const OJString &inputFileName = GetOJString(""),
        const OJString &outputFileName = GetOJString(""));

    static void setWindowsUser(WindowsUserPtr user);

private:

    static WindowsUserPtr   s_userPtr_;
};

}   // namespace IMUST