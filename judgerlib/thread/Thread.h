#ifndef IMUST_OJ_THREAD_H
#define IMUST_OJ_THREAD_H

#include "../../thirdpartylib/boost/thread.hpp"

namespace IMUST
{

// 线程库采用Boost.Thread，如果需要自行设计，接口要与Boost兼容
typedef             ::boost::thread                 Thread;
typedef             ::boost::mutex                  Mutex;

}

#endif  // IMUST_OJ_THREAD_H