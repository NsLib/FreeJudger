#ifndef IMUST_OJ_THREAD_H
#define IMUST_OJ_THREAD_H

#include <memory>
#include "ThreadLock.h"

namespace IMUST
{

// 线程库采用Boost.Thread，如果需要自行设计，接口要与Boost兼容

    class IThreadFun
    {
    public:
        IThreadFun();
        virtual ~IThreadFun();

        virtual void run() = 0;
    };
    
    template<typename T>
    class ThreadFunObj : public IThreadFun
    {
        T funObj_;
    public:
        explicit ThreadFunObj(const T & v)
            : funObj_(v)
        {}

        ~ThreadFunObj()
        {}

        virtual void run()
        {
            funObj_();
        }
    };


    ///proxy用于屏蔽具体的实现
    class ThreadProxy;

    class Thread
    {
        std::shared_ptr<IThreadFun> fun_;
        ThreadProxy * threadProxy_;

        void start_thread();

    public:

        explicit Thread(std::shared_ptr<IThreadFun> fun)
            : fun_(fun)
        {
            start_thread();
        }
        
        template<typename T>
        explicit Thread(const T & funObj)
            : fun_(new ThreadFunObj<T>(funObj))
        {
            start_thread();
        }

        ~Thread();

        void join();
    };

    
    typedef std::shared_ptr<IMUST::Thread> ThreadPtr;
}

#endif  // IMUST_OJ_THREAD_H