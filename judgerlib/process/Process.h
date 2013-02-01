
#include "../util/Utility.h"
#include "../platformlayer/PlatformLayer.h"
#include "../thread/Thread.h"

#include <Windows.h>

namespace IMUST
{

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
};

namespace
{

class JUDGER_API WindowsProcessInOut : public IProcess
{
public:
	WindowsProcessInOut(const OJString &inputFileName,
						const OJString &outputFileName);
	virtual ~WindowsProcessInOut();

protected:
    virtual HANDLE createInputFile();
    virtual HANDLE createOutputFile();

protected:
    HANDLE		inputFileHandle_;
    HANDLE		outputFileHandle_;
    OJString	inputFileName_;
    OJString	outputFileName_;
};

class JUDGER_API WindowsJob
{
public:
	WindowsJob();
	~WindowsJob();

	bool create(LPSECURITY_ATTRIBUTES lpJobAttributes = NULL);
	DWORD wait(DWORD time = INFINITE);
	bool terminate(DWORD exitCode = 4);
	bool setInformation(JOBOBJECTINFOCLASS infoClass,
						LPVOID lpInfo,
						DWORD cbInfoLength);
	bool assinProcess(HANDLE handel);

private:
	HANDLE jobHandle_;
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

private:
    HANDLE		processHandle_;
	HANDLE		threadHandle_;
	HANDLE		iocpHandle_;
	WindowsJob	jobHandle_;
	OJInt32_t	exitCode_;


private:
	static	Mutex		s_mutex_;
	static	ULONG		s_id_;	
};






}   // namespace IMUST