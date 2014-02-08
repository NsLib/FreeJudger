#pragma once


#include "../judgerlib/judgecore/JudgeCore.h"

typedef std::shared_ptr<IMUST::JudgeCore> JudgeCorePtr;

JudgeCorePtr getJudgeCore();
void setJudgeCore(JudgeCorePtr judgeCore);
