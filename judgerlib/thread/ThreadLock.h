#pragma once

namespace IMUST
{
    class MutexProxy;

    class Mutex
    {
    public:
        Mutex();
        ~Mutex();

        void lock();

        void unlock();

        bool try_lock();

    private:
        MutexProxy * pLocker_;

        Mutex(const Mutex &);
        const Mutex & operator = (const Mutex &);
    };

}//end namespace IMUST