// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here

#include <memory>
#include <vector>

#include "../platformlayer/PlatformLayer.h"

#include "../config/AppConfig.h"
#include "../filetool/FileTool.h"
#include "../util/StringTool.h"
#include "../util/Watch.h"

#include "../logger/Logger.h"
#include "../taskmanager/TaskManager.h"
#include "../sql/DBManager.h"
#include "../thread/Thread.h"