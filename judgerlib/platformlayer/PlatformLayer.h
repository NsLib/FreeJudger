

#ifndef IMUST_OJ_PLATFORM_LAYER_H
#define IMUST_OJ_PLATFORM_LAYER_H

#include "PlatformConfig.h"

#include <string>
#include <iostream>
#include <cassert>

#include "../../thirdpartylib/boost/thread.hpp"

#include "../util/Utility.h"

namespace IMUST
{
#define OJAPI

typedef             std::wstring                    OJString;
typedef             wchar_t                         OJChar_t;

#define OJStr(str)          L##str
#define GetOJString(str)    ::IMUST::OJString(OJStr(str)) 

#define OJCout      std::wcout
#define OJCerr      std::wcerr

typedef             short                           OJInt16_t;
typedef             int                             OJInt32_t;
typedef             long long                       OJInt64_t;
typedef             float                           OJFloat16_t;
typedef             double                          OJFloat32_t;
typedef             unsigned short                  OJUInt16_t;
typedef             unsigned int                    OJUInt32_t;
typedef             unsigned long long              OJUInt64_t;

// 线程库采用Boost.Thread，如果需要自行设计，接口要与Boost兼容
typedef             ::boost::thread                 Thread;
typedef             ::boost::mutex                  Mutex;






}




#endif  // IMUST_OJ_PLATFORM_LAYER_H

