
namespace IMUST
{

typedef std::shared_ptr<class WindowsUser> WindowsUserPtr;
typedef std::shared_ptr<class IProcess> ProcessPtr;


///进程类型
namespace ProcessType
{
    const OJInt32_t Compiler    = 0;///<编译用的进程
    const OJInt32_t Excuter     = 1;///<执行解题程序用的进程
}


///进程基类
class JUDGER_API IProcess
{
    MAKE_CLASS_UNCOPYABLE(IProcess);

public:
    IProcess();
    virtual ~IProcess();

    /** 创建进程。
     * @param   cmd     命令行参数
     * @param   timeLimit   要限定的时间
     * @param   memoryLimit     要限定的内存
     * @param   startImmediately    创建完成之后，是否立即启动。
     */
    virtual OJInt32_t create(const OJString &cmd,
							const OJInt32_t timeLimit,
							const OJInt32_t memoryLimit,
							bool startImmediately = true) = 0;

    ///启动进程
    virtual OJInt32_t start() = 0;

    ///进程是否允许状态
    virtual bool isRunning() = 0;

    ///等待进程结束。并返回退出码。
    virtual OJInt32_t join(OJInt32_t time) = 0;

    ///获取进程真正的退出码
    virtual OJInt32_t getExitCode() = 0;

    ///杀死进程
    virtual void kill() = 0;

    ///获取转义之后的退出码。为ProcessExitCode中的一个。
    virtual OJInt32_t getResult() = 0;

    ///获取运行时间
    virtual OJInt32_t getRunTime() = 0;

    ///获取占用内存
    virtual OJInt32_t getRunMemory() = 0;
};


///进程工厂
class JUDGER_API ProcessFactory
{
    MAKE_CLASS_UNCOPYABLE(ProcessFactory);

public:
    ProcessFactory();
    ~ProcessFactory();

    ///根据类型创建进程
    static ProcessPtr create(OJInt32_t type,
        const OJString &inputFileName = GetOJString(""),
        const OJString &outputFileName = GetOJString(""));

    static void setWindowsUser(WindowsUserPtr user);

private:

    static WindowsUserPtr   s_userPtr_;
};

}   // namespace IMUST