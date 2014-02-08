#include "stdafx.h"
#include "Common.h"

namespace
{
    JudgeCorePtr g_judgeCore;
}

JudgeCorePtr getJudgeCore()
{
    return g_judgeCore;
}

void setJudgeCore(JudgeCorePtr judgeCore)
{
    g_judgeCore = judgeCore;
}
