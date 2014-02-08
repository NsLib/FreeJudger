#include "Thread.h"

#include "../../thirdpartylib/boost/thread.hpp"

namespace IMUST
{
    IThreadFun::IThreadFun()
    {}

    IThreadFun::~IThreadFun()
    {}

    ////////////////////////////////////////////////////////////////////

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
    public:
        ThreadProxy(IThreadFun * fun)
            : thread_(BoostThreadFun(fun))
        {
        }

        ~ThreadProxy()
        {
        }

        boost::thread thread_;
    };



    Thread::~Thread()
    {
        delete threadProxy_;
    }

    void Thread::start_thread()
    {
        threadProxy_ = new ThreadProxy(fun_.get());
    }

    void Thread::join()
    {
        threadProxy_->thread_.join();
    }


}