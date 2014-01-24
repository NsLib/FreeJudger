#include "JavaExcuter.h"
#include "../process/Process.h"
#include "../filetool/FileTool.h"
#include "../util/StringTool.h"

namespace IMUST
{

JavaExcuter::JavaExcuter(void)
{
}


JavaExcuter::~JavaExcuter(void)
{
}


bool JavaExcuter::run(
    const OJString & exeFile,
    const OJString & inputFile,
    const OJString & outputFile,
    OJInt32_t limitTime,
    OJInt32_t limitMemory
    )
{
    OJString exePath = FileTool::GetFilePath(exeFile);
    OJString exeFileName = FileTool::GetFileName(exeFile);//get only name

    OJString cmdBuffer;
    FormatString(cmdBuffer, OJStr("java -cp %s %s"), exePath.c_str(), exeFileName.c_str());

    ProcessPtr wp = ProcessFactory::create(ProcessType::WithUser, inputFile, outputFile);
    wp->create(cmdBuffer, limitTime*30, limitMemory*10);
    result_ = wp->getExitCodeEx();

    runTime_ = wp->getRunTime();
    runMemory_ = wp->getRunMemory();

    return isAccept();
}
}//namespace IMUST