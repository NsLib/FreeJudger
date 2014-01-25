#include "stdafx.h"
#include "ExeExcuter.h"

namespace IMUST
{

ExeExcuter::ExeExcuter(void)
{
}


ExeExcuter::~ExeExcuter(void)
{
}

void ExeExcuter::run(
    const OJString & exeFile,
    const OJString & inputFile,
    const OJString & outputFile,
    OJInt32_t limitTime,
    OJInt32_t limitMemory
    )
{
    ProcessPtr wp = ProcessFactory::create(ProcessType::Excuter, inputFile, outputFile);
    wp->create(exeFile, limitTime, limitMemory);
    result_ = wp->getResult();
    runTime_ = wp->getRunTime();
    runMemory_ = wp->getRunMemory();
}

}//namespace IMUST