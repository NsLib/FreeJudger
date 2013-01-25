

#ifndef IMUST_OJ_PLATFORM_LAYER_H
#define IMUST_OJ_PLATFORM_LAYER_H

#include "PlatformConfig.h"

#include <string>
#include <iostream>
#include <cassert>

#include "../util/Utility.h"

namespace IMUST
{
#define OJAPI

typedef             std::wstring                    OJString;
typedef             wchar_t                         OJChar_t;

#define OJStr(str)          L##str
#define GetOJString(str)    ::IMUST::OJString(OJStr(str)) 


#define OJCout       std::wcout
#define OJCerr       std::wcerr

typedef             short                           OJInt16_t;
typedef             int                             OJInt32_t;
typedef             long long                       OJInt64_t;
typedef             float                           OJFloat16_t;
typedef             double                          OJFloat32_t;







}




#endif  // IMUST_OJ_PLATFORM_LAYER_H

