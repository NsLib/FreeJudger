#include "Thread.h"

#include "../platformlayer/PlatformLayer.h"
#include "../../thirdpartylib/boost/thread.hpp"
#include "../util/Watch.h"

namespace IMUST
{
    namespace
    {
        OJInt32_t NumThread = 0;
    }

    IThreadFun::IThreadFun()
    {}

    IThreadFun::~IThreadFun()
    {}

    ////////////////////////////////////////////////////////////////////

#ifdef TARGET_PLATFORM_WINDOWS

#include <process.h>
    
    class ThreadProxy
    {
    public:

        ThreadProxy(IThreadFun * fun)
            : pFun_(fun)
        {
            assert(fun && "ThreadProxy construct");

            threadHandle_ = (HANDLE)_beginthreadex(NULL, 0, _callback, this, 0, 0);
        }

        ~ThreadProxy()
        {
            join();

            if(threadHandle_ != 0)
                ::CloseHandle(threadHandle_);
        }

        void run()
        {
            pFun_->run();
        }
        

        void join()
        {
            if(threadHandle_ != 0)
                ::WaitForSingleObject(threadHandle_, INFINITE);
        }

        static unsigned __stdcall _callback(void * param)
        {
            ThreadProxy * proxy = (ThreadProxy*)param;
            proxy->run();
            return 0;
        }

    private:
        
        ThreadProxy(const ThreadProxy &);
        const ThreadProxy & operator = (const ThreadProxy &);

        HANDLE threadHandle_;
        IThreadFun * pFun_;

    };

#else
    class BoostThreadFun
    {
        IThreadFun * pFun_;
    public:

        BoostThreadFun(IThreadFun * fun)
            : pFun_(fun)
        {}

        void operator()()
        {
            pFun_->run();
        }
    };

    class ThreadProxy
    {
        boost::thread thread_;

    public:
        ThreadProxy(IThreadFun * fun)
            : thread_(BoostThreadFun(fun))
        {
        }

        ~ThreadProxy()
        {
        }

        void join()
        {
            thread_.join();
        }

    };

#endif


    Thread::~Thread()
    {
        delete threadProxy_;

        WatchTool::WatchCount(OJStr("core/numThread"), NumThread, -1);
    }

    void Thread::start_thread()
    {
        threadProxy_ = new ThreadProxy(fun_.get());

        WatchTool::WatchCount(OJStr("core/numThread"), NumThread, +1);
    }

    void Thread::join()
    {
        threadProxy_->join();
    }


}