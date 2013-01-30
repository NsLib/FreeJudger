#include "DBManager.h"

#include "../taskmanager/TaskManager.h"

#include "../config/AppConfig.h"

namespace IMUST
{
namespace
{
    
const OJInt32_t MaxBufferSize = 1024;

}

namespace Statement
{

const OJString SelectSolution = OJStr("SELECT solution_id, problem_id, user_id, language ")
    OJStr("FROM `solution` WHERE result=%d or result=%d limit 20");

const OJString SelectProblem = OJStr("SELECT time_limit, memory_limit ")
    OJStr("FROM problem WHERE problem_id =%d");

const OJString SelectCode = OJStr("SELECT `source` FROM `source_code` WHERE `solution_id`=%d");

const OJString SelectCustomInput = OJStr("SELECT `input_text` from custominput");

const OJString UpdateSolutionCompiling = OJStr("UPDATE `solution` SET `result`=%d, `judgetime`=NOW() ")
	OJStr("WHERE `solution_id` = %d");

const OJString UpdateSolutionResult = OJStr("UPDATE `solution` SET `result`=%d, ")
    OJStr("`time`=%d, `memory`=%d, `judgetime`=NOW(), `pass_rate`=%f ")
    OJStr("WHERE `solution_id` = %d");

const OJString DeleteCompile = OJStr("DELETE FROM `compileinfo` WHERE `solution_id`=%d");
const OJString InsertCompile = OJStr("INSERT INTO `compileinfo`(solution_id, error)VALUES(%d, \"%s\")");

const OJString DeleteRuntime = OJStr("DELETE FROM `runtimeinfo` WHERE `solution_id`=%d");
const OJString InsertRuntime = OJStr("INSERT INTO `runtimeinfo`(solution_id, error)VALUES(%d, \"%s\")");


const OJString UpdateUserSolved = OJStr("UPDATE `users` SET `solved`=")
    OJStr("(SELECT count(DISTINCT `problem_id`) FROM `solution` ")
    OJStr("WHERE `user_id`=\'%s\' AND `result`=%d) ")
    OJStr("WHERE `user_id`=\'%s\'");

const OJString UpdateUserSubmit = OJStr("UPDATE `users` SET `submit`=")
    OJStr("(SELECT count(*) FROM `solution` WHERE `user_id`=\'%s\') ")
    OJStr("WHERE `user_id`=\'%s\'");

const OJString UpdateProblemAccept = OJStr("UPDATE `problem` SET `accepted`=")
    OJStr("(SELECT count(*) FROM `solution` WHERE `problem_id`=\'%d\' AND `result`=\'%d\') ")
    OJStr("WHERE `problem_id`=\'%d\'");

const OJString UpdateProblemSubmit = OJStr("UPDATE `problem` SET `submit`=")
    OJStr("(SELECT count(*) FROM `solution` WHERE `problem_id`=\'%d\')")
    OJStr("WHERE `problem_id`=\'%d\'");


}

using namespace AppConfig;

DBManager::DBManager(SqlDriverPtr sqlDriver, TaskManagerPtr working, TaskManagerPtr finished)
    : sqlDriver_(sqlDriver)
    , workingTaskMgr_(working)
    , finishedTaskMgr_(finished)
{
}


DBManager::~DBManager(void)
{
}

bool DBManager::run()
{
    assert(sqlDriver_->valid() && "sql driver not valid!");

    OJCout<<Statement::SelectProblem<<std::endl;

    if(!readTask())
    {
        OJCout<<OJStr("read task faild!")<<std::endl;
        return false;
    }
    
    if(!writeFinishedTask())
    {
        return false;
    }

    return true;
}


bool DBManager::readTask()
{
    OJChar_t buffer[MaxBufferSize];
    OJSprintf(buffer, Statement::SelectSolution.c_str(), JudgeCode::Pending, JudgeCode::Rejudge);
    OJCout<<buffer<<std::endl;
    if(!sqlDriver_->query(buffer))
    {
        return false;
    }
    SqlResultPtr result = sqlDriver_->storeResult();
    if(!result)
    {
        return true;
    }
    IMUST::OJUInt32_t cols = result->getNbCols();
    for(IMUST::OJUInt32_t i=0; i<cols; ++i)
    {
        OJCout<<result->getFieldName(i)<<GetOJString("\t");
    }
    OJCout<<std::endl;

    SqlRowPtr row(NULL);
    while(row = result->fetchRow())
    {
        for(OJUInt32_t i=0; i<row->getNbCols(); ++i)
        {
            OJCout<<row->getVar(i).getString()<<OJStr("\t");
        }
        OJCout<<std::endl;
    }

    //OJCout<<sqlDriver_->getErrorString();

    return true;
}

bool DBManager::writeFinishedTask()
{
    return false;
}


}//namespace IMUST