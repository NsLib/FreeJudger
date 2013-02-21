
#include "ExeExcuter.h"
#include "../process/Process.h"
#include "../filetool/FileTool.h"

namespace IMUST
{

ExeExcuter::ExeExcuter(void)
{
}


ExeExcuter::~ExeExcuter(void)
{
}

bool ExeExcuter::run(
    const OJString & exeFile,
    const OJString & inputFile,
    const OJString & outputFile,
    OJInt32_t limitTime,
    OJInt32_t limitMemory
    )
{
    IMUST::WindowsProcess wp(inputFile, outputFile);
    wp.create(exeFile, limitTime, limitMemory);
    result_ = wp.getExitCodeEx();

    runTime_ = wp.getRunTime();
    runMemory_ = wp.getRunMemory();

    return isAccept();
}

}//namespace IMUST