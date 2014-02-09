#pragma once


#include "../judgerlib/judgecore/JudgeCore.h"

const DWORD WM_DLG_UPDATE =  WM_USER + 1;

typedef std::shared_ptr<IMUST::JudgeCore> JudgeCorePtr;

JudgeCorePtr getJudgeCore();
void setJudgeCore(JudgeCorePtr judgeCore);
