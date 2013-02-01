#include "DBManager.h"

#include "../taskmanager/TaskManager.h"

#include "../config/AppConfig.h"

#pragma warning(push)
#pragma warning(disable:4996)

namespace IMUST
{
namespace
{
    
const OJInt32_t MaxBufferSize = 1024;

}

namespace Statement
{

const OJString SelectSolution2 = OJStr("SELECT solution_id, problem_id, user_id, language ")
    OJStr("FROM `solution` WHERE result=%d or result=%d limit 5");

const OJString SelectProblem1 = OJStr("SELECT time_limit, memory_limit ")
    OJStr("FROM problem WHERE problem_id =%d");

const OJString SelectCode1 = OJStr("SELECT `source` FROM `source_code` WHERE `solution_id`=%d");

const OJString SelectCustomInput1 = OJStr("SELECT `input_text` from custominput WHERE `solution_id`=%d");

const OJString UpdateSolutionCompiling2 = OJStr("UPDATE `solution` SET `result`=%d, `judgetime`=NOW() ")
	OJStr("WHERE `solution_id` = %d");

const OJString UpdateSolutionResult5 = OJStr("UPDATE `solution` SET `result`=%d, ")
    OJStr("`time`=%d, `memory`=%d, `judgetime`=NOW(), `pass_rate`=%f ")
    OJStr("WHERE `solution_id` = %d");

const OJString DeleteCompile1 = OJStr("DELETE FROM `compileinfo` WHERE `solution_id`=%d");
const OJString InsertCompile2 = OJStr("INSERT INTO `compileinfo`(solution_id, error)VALUES(%d, \"%s\")");

const OJString DeleteRuntime1 = OJStr("DELETE FROM `runtimeinfo` WHERE `solution_id`=%d");
const OJString InsertRuntime2 = OJStr("INSERT INTO `runtimeinfo`(solution_id, error)VALUES(%d, \"%s\")");


const OJString UpdateUserSolved3 = OJStr("UPDATE `users` SET `solved`=")
    OJStr("(SELECT count(DISTINCT `problem_id`) FROM `solution` ")
    OJStr("WHERE `user_id`=\'%s\' AND `result`=%d) ")
    OJStr("WHERE `user_id`=\'%s\'");

const OJString UpdateUserSubmit2 = OJStr("UPDATE `users` SET `submit`=")
    OJStr("(SELECT count(*) FROM `solution` WHERE `user_id`=\'%s\') ")
    OJStr("WHERE `user_id`=\'%s\'");

const OJString UpdateProblemAccept3 = OJStr("UPDATE `problem` SET `accepted`=")
    OJStr("(SELECT count(*) FROM `solution` WHERE `problem_id`=\'%d\' AND `result`=\'%d\') ")
    OJStr("WHERE `problem_id`=\'%d\'");

const OJString UpdateProblemSubmit2 = OJStr("UPDATE `problem` SET `submit`=")
    OJStr("(SELECT count(*) FROM `solution` WHERE `problem_id`=\'%d\')")
    OJStr("WHERE `problem_id`=\'%d\'");


}

using namespace AppConfig;

DBManager::DBManager(SqlDriverPtr sqlDriver, 
    TaskManagerPtr working, 
    TaskManagerPtr finished,
    TaskFactoryPtr factory)
    : sqlDriver_(sqlDriver)
    , workingTaskMgr_(working)
    , finishedTaskMgr_(finished)
    , taskFactory_(factory)
{
}


DBManager::~DBManager(void)
{
}

bool DBManager::run()
{
    assert(sqlDriver_->valid() && "sql driver not valid!");

    if(!readTasks())
    {
        OJCout<<OJStr("read task faild!")<<sqlDriver_->getErrorString()<<std::endl;
        return false;
    }

    if(!writeFinishedTask())
    {
        OJCout<<OJStr("write task faild!")<<sqlDriver_->getErrorString()<<std::endl;
        return false;
    }

    return true;
}

OJInt32_t readOneRow(SqlRowPtr & row, SqlDriverPtr driver, const OJString & sql)
{
    //OJCout<<sql<<std::endl;

    if(!driver->query(sql))
    {
        return -1;
    }
    SqlResultPtr result = driver->storeResult();
    if(!result)
    {
        return 1;
    }
    row = result->fetchRow();
    if(!row)
    {
        return 2;
    }
    return 0;
}

bool DBManager::readTasks()
{
    bool result = true;;

    workingTaskMgr_->lock();
    if(!workingTaskMgr_->hasTask())
    {
        result = readDB();
    }
    workingTaskMgr_->unlock();

    return result;
}


bool DBManager::readDB()
{
    OJChar_t buffer[MaxBufferSize];

    //读取结果为待定和等待重判的提交
    OJSprintf(buffer, Statement::SelectSolution2.c_str(), JudgeCode::Pending, JudgeCode::Rejudge);

    if(!sqlDriver_->query(buffer))
    {
        return false;
    }
    SqlResultPtr result = sqlDriver_->storeResult();
    if(!result)
    {
        return true;
    }

    TaskInputData taskData;

    SqlRowPtr row(NULL), tempRow(NULL);
    while(row = result->fetchRow())
    {
        taskData.SolutionID = (*row)[0].getInt32();
        taskData.ProblemID = (*row)[1].getInt32();
        taskData.UserName = (*row)[2].getString();
        taskData.Language = (*row)[3].getInt32();

        OJInt32_t r = readTaskData(taskData);
        if(r == 0 )
        {
            ITask* pTask = taskFactory_->create(taskData);
            workingTaskMgr_->addTask(pTask);
            OJCout<<OJStr("add task:")<<taskData.SolutionID<<std::endl;
        }
        else if(r < 0)
        {
            return false;
        }
    }

    return true;
}

//读取任务数据。
//返回值=0：为正确； <0：异常； >0：此次读取被忽略。
OJInt32_t DBManager::readTaskData(TaskInputData & taskData)
{
    OJChar_t buffer[MaxBufferSize];
    SqlRowPtr tempRow(NULL);

#if 1
    //修改记录的状态为编译中，防止重复读取
    OJSprintf(buffer, Statement::UpdateSolutionCompiling2.c_str(), 
        JudgeCode::Compiling, taskData.SolutionID);
    if(!sqlDriver_->query(buffer))
    {
        return -1;
    }
#endif

    //读取题目限制时间和内存
    if(taskData.ProblemID == 0) //IDE测试功能
    {
        taskData.LimitTime = 5; //s
        taskData.LimitMemory = 10;//m

        //读取用户输入的测试数据
        OJSprintf(buffer, Statement::SelectCustomInput1.c_str(), taskData.SolutionID);
        OJInt32_t r = readOneRow(tempRow, sqlDriver_, buffer);
        if(r != 0)
        {
            return r;
        }

        taskData.UserInput = tempRow->getVar(0).getString();

        //TODO: 增加对IDE测试功能的支持
        
        //return 1;
    }
    else
    {
        OJSprintf(buffer, Statement::SelectProblem1.c_str(), taskData.ProblemID);
        OJInt32_t r = readOneRow(tempRow, sqlDriver_, buffer);
        if(r != 0)
        {
            return r;
        }

        taskData.LimitTime = (*tempRow)[0].getInt32();
        taskData.LimitMemory = (*tempRow)[1].getInt32();
    }

    //读取代码
    OJSprintf(buffer, Statement::SelectCode1.c_str(), taskData.SolutionID);
    OJInt32_t r = readOneRow(tempRow, sqlDriver_, buffer);
    if(r != 0)
    {
        return r;
    }
    taskData.UserCode = (*tempRow)[0].getString();

    return 0;
}

bool DBManager::writeFinishedTask()
{
    ITask* pTask = NULL;
    finishedTaskMgr_->lock();
    if(finishedTaskMgr_->hasTask())
    {
        pTask = finishedTaskMgr_->popTask();
    }
    finishedTaskMgr_->unlock();

    if(NULL == pTask)
    {
        return true;
    }

    if(pTask->input().ProblemID == 0)//IDE测试功能，不写数据库
    {
        taskFactory_->destroy(pTask);
        return true;
    }
    
    if(!writeToDB(pTask))
    {
        taskFactory_->destroy(pTask);
        return false;
    }

    OJCout<<GetOJString("write finished:")<<pTask->input().SolutionID<<std::endl;
    taskFactory_->destroy(pTask);
    return true;
}

bool DBManager::writeToDB(const ITask* pTask)
{
    assert(pTask);

    OJChar_t buffer[MaxBufferSize];

    const TaskInputData & input = pTask->input();
    const TaskOutputData & output = pTask->output();


    //更新结果
    OJSprintf(buffer, Statement::UpdateSolutionResult5.c_str(), output.Result, output.RunTime, 
        output.RunMemory, output.PassRate, input.SolutionID);
    if(!sqlDriver_->query(buffer))
    {
        return false;
    }

    //更新用户提交数量
    OJSprintf(buffer, Statement::UpdateUserSubmit2.c_str(), 
        input.UserName.c_str(), input.UserName.c_str());
    if(!sqlDriver_->query(buffer))
    {
        return false;
    }
    
    //用户已解决的。不管答案是否正确都执行此操作，防止是重判，而导致信息不及时刷新。
    OJSprintf(buffer, Statement::UpdateUserSolved3.c_str(), 
        input.UserName.c_str(), JudgeCode::Accept, input.UserName.c_str());
    if(!sqlDriver_->query(buffer))
    {
        return false;
    }

    //题目提交数量
    OJSprintf(buffer, Statement::UpdateProblemSubmit2.c_str(), input.ProblemID, input.ProblemID);
    if(!sqlDriver_->query(buffer))
    {
        return false;
    }

    //题目通过的数量
    OJSprintf(buffer, Statement::UpdateProblemAccept3.c_str(), 
        input.ProblemID, JudgeCode::Accept, input.ProblemID);
    if(!sqlDriver_->query(buffer))
    {
        return false;
    }
    
    //如果未通过，写错误原因

    if(output.Result == JudgeCode::CompileError)//如果编译错误
    {
        OJSprintf(buffer, Statement::DeleteCompile1.c_str(), input.SolutionID);
        if(!sqlDriver_->query(buffer))
        {
            return false;
        }

        OJSprintf(buffer, Statement::InsertCompile2.c_str(), 
            input.SolutionID, output.CompileError.c_str());
        if(!sqlDriver_->query(buffer))
        {
            return false;
        }
    }
    else if(output.Result == JudgeCode::RuntimeError)//运行时错误
    {
        OJSprintf(buffer, Statement::DeleteRuntime1.c_str(), input.SolutionID);
        if(!sqlDriver_->query(buffer))
        {
            return false;
        }

        OJSprintf(buffer, Statement::InsertRuntime2.c_str(), 
            input.SolutionID, output.RunTimeError.c_str());
        if(!sqlDriver_->query(buffer))
        {
            return false;
        }
    }

    return true;
}

}//namespace IMUST

#pragma warning(pop)
