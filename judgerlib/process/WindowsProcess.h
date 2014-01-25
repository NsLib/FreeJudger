
#include "Process.h"

namespace IMUST
{

///进程的输入输出重定向到文件中
class JUDGER_API WindowsProcessInOut : public IProcess
{
public:
    ///构造函数。输入和输出文件可以分别为空字符串，表示不重定向到文件中。
	WindowsProcessInOut(const OJString &inputFileName,
						const OJString &outputFileName);

	virtual ~WindowsProcessInOut();

protected:
    virtual bool createInputFile();
    virtual bool createOutputFile();

protected:
    HANDLE		inputFileHandle_;///<输入文件句柄
    HANDLE		outputFileHandle_;///<输出文件句柄
    OJString	inputFileName_;///<输入文件名
    OJString	outputFileName_;///<输出文件名
};

///windows作业对象
class JUDGER_API WindowsJob
{
    MAKE_CLASS_UNCOPYABLE(WindowsJob);

public:
	WindowsJob();
	~WindowsJob();

    ///创建windows作业对象
	bool create(bool useToExcuter, LPSECURITY_ATTRIBUTES lpJobAttributes = NULL);

    ///等待一段时间。或者一直等待直到作业对象结束。
	DWORD wait(DWORD time = INFINITE);

    ///终止作业
	bool terminate(DWORD exitCode = 4);

    ///将进程加入作业中。
    ///@param   handel  进程句柄
	bool assinProcess(HANDLE handel);

    ///设置作业时间和内存限制
    ///@param   timeLimit 限制时间。单位为毫秒(ms)。
    ///@param   memoryLimit 限制内存。单位为字节(byte)。
    bool setLimit(const OJInt32_t timeLimit,
                const OJInt32_t memoryLimit);

    ///通过完成端口，查询作业进行的状态。
    bool getState(DWORD &executeResult, 
                ULONG &completionKey, 
                LPOVERLAPPED &processInfo, 
                const DWORD time = INFINITE);

    ///查询作业信息。内存占用等情况。
    bool queryInformation(JOBOBJECTINFOCLASS informationClass,
        LPVOID lpInformation,
        DWORD cbInformationLength,
        LPDWORD lpReturnLength);

private:
    ///设置限制信息
    bool setInformation(JOBOBJECTINFOCLASS infoClass,
        LPVOID lpInfo,
        DWORD cbInfoLength);

private:
    bool        useToExcuter_;  ///<用于执行判题程序，job的限制较为严格。
	HANDLE      jobHandle_;     ///<作业对象句柄
    HANDLE		iocpHandle_;    ///<完成端口句柄

private:
    static	Mutex		s_mutex_;///<分配id锁
    static	ULONG		s_id_;///<为端口分配id
};


///windows进程
class JUDGER_API WindowsProcess : public WindowsProcessInOut
{
public:
	WindowsProcess(bool useToExcuter, const OJString &inputFileName = GetOJString(""),
					const OJString &outputFileName = GetOJString(""));
    virtual ~WindowsProcess();

    virtual OJInt32_t create(const OJString &cmd,
							const OJInt32_t timeLimit,
							const OJInt32_t memoryLimit,
							bool startImmediately = true);
    
    virtual OJInt32_t start();
    virtual OJInt32_t join(OJInt32_t time);
    virtual OJInt32_t getExitCode();
    virtual void kill();

    virtual bool isRunning();
    virtual OJInt32_t getResult(){ return result_; }
    virtual OJInt32_t getRunTime(){ return runTime_;}
    virtual OJInt32_t getRunMemory(){ return runMemory_;}

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
    bool        alive_;         ///< 进程是存活状态
    bool        useToExcuter_;  ///<用于执行解题程序
    HANDLE		processHandle_; ///<进程句柄
	HANDLE		threadHandle_;  ///<主线程句柄
	WindowsJob	jobHandle_;     ///<windows作业对象句柄
	OJInt32_t	result_;        ///<进程退出原因
    OJInt32_t   runTime_;       ///<进程使用时间
    OJInt32_t   runMemory_;     ///<进程使用内存


private:
	static	Mutex		s_mutex_;
	static	ULONG		s_id_;	
};

///使用低权限的Windows用户创建进程
class JUDGER_API WindowsUserProcess : public WindowsProcess
{
public:
    WindowsUserProcess(bool useToExcuter, WindowsUserPtr user,
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

}   // namespace IMUST