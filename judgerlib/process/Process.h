
#include "../util/Utility.h"
#include "../platformlayer/PlatformLayer.h"

namespace IMUST
{

class IProcess
{
    MAKE_CLASS_UNCOPYABLE(IProcess);

public:
    IProcess();
    virtual ~IProcess();

    virtual OJInt32_t create(const OJString &cmd, bool startImmediately = true) = 0;
    virtual OJInt32_t start() = 0;
    virtual bool isRunning() = 0;
    virtual OJInt32_t join(OJInt32_t time) = 0;
    virtual OJInt32_t getExitCode() = 0;
    virtual void kill() = 0;
};

class WindowsProcess : public IProcess
{
public:
    WindowsProcess();
    virtual ~WindowsProcess();

    virtual OJInt32_t create(const OJString &cmd, bool startImmediately = true);
    virtual OJInt32_t start();
    virtual bool isRunning();
    virtual OJInt32_t join(OJInt32_t time);
    virtual OJInt32_t getExitCode();
    virtual void kill();

private:
    HANDLE  processHandle_;
};






}   // namespace IMUST