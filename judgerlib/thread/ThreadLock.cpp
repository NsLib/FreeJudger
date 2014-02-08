#include "ThreadLock.h"
#include "../../thirdpartylib/boost/thread.hpp"

namespace IMUST
{
    class MutexProxy
    {
    public:

        MutexProxy(void)
        {
        }


        ~MutexProxy(void)
        {
        }

        boost::mutex mutex_;
    };


    Mutex::Mutex()
    {
        pLocker_ = new MutexProxy();
    }

    Mutex::~Mutex()
    {
        delete pLocker_;
    }

    void Mutex::lock()
    {
        pLocker_->mutex_.lock();
    }

    void Mutex::unlock()
    {
        pLocker_->mutex_.unlock();
    }

    bool Mutex::try_lock()
    {
        return pLocker_->mutex_.try_lock();
    }
}