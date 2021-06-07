#include "checkDataExecutionResult.hpp"

#include "actionData.hpp"
#include "checkData.hpp"
#include "checkMappings/checkExecutionStateStrMapping.hpp"
#include "actonDataHub.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QDateTime>

#define MACRO_ADDLOG(...) \
if (check_ptr_pri->parentAction_f() not_eq nullptr) \
{ \
    MACRO_ADDACTONDATAHUBLOG(check_ptr_pri->parentAction_f()->actonDataHubParent_f(),__VA_ARGS__); \
}


std::vector<checkExecutionState_ec> checkExecutionResult_c::executionStateVector_f() const
{
    return executionStateVector_pri;
}

checkExecutionState_ec checkExecutionResult_c::lastState_f() const
{
    return executionStateVector_pri.back();
}

check_c* checkExecutionResult_c::check_ptr_f() const
{
    return check_ptr_pri;
}

bool checkExecutionResult_c::result_f() const
{
    return result_pri;
}

bool checkExecutionResult_c::logicResult_f() const
{
    bool resultTmp(false);
    while (finished_pro)
    {
        if (check_ptr_pri->resultLogic_f() == checkData_c::resultLogic_ec::trueOnSuccess
            and lastState_f() == checkExecutionState_ec::success
            and result_pri)
        {
            resultTmp = true;
            break;
        }
        if (check_ptr_pri->resultLogic_f() == checkData_c::resultLogic_ec::trueOnFailure
            and lastState_f() == checkExecutionState_ec::success
            and not result_pri)
        {
            resultTmp = true;
            break;
        }
        if (check_ptr_pri->resultLogic_f() == checkData_c::resultLogic_ec::trueOnError
            and lastState_f() == checkExecutionState_ec::error)
        {
            resultTmp = true;
            break;
        }
        if (check_ptr_pri->resultLogic_f() == checkData_c::resultLogic_ec::trueAlwaysExceptOnError
            and lastState_f() not_eq checkExecutionState_ec::error)
        {
            resultTmp = true;
            break;
        }
        break;
    }
    return resultTmp;
}

//void checkExecutionResult_c::derivedTryClear_f()
//{
//    executionStateVector_pri = { checkExecutionState_ec::initial };

//    result_pri = false;
//    resultSet_pri = false;

//    Q_EMIT resultsCleared_signal(check_ptr_pri);
//}

checkExecutionResult_c::checkExecutionResult_c(
        check_c* const check_par_ptr_con
        , QObject* parent_par)
    : executionResult_c(parent_par)
    , check_ptr_pri(check_par_ptr_con)
{}

void checkExecutionResult_c::derivedAppendMessage_f(executionMessage_c* message_par_con)
{
    while (not finished_pro)
    {
        if (message_par_con->type_f() == executionMessage_c::type_ec::error)
        {
            for (size_t i = 0, l = message_par_con->text_f().size_f(); i < l; ++i)
            {
                MACRO_ADDLOG(message_par_con->text_f().text_f(i), check_ptr_pri, messageType_ec::warning);
            }
            trySetExecutionState_f(checkExecutionState_ec::error);
            Q_EMIT errorMessageAdded_signal(this, message_par_con);
            break;
        }
        break;
    }
}

bool checkExecutionResult_c::trySetExecutionState_f(const checkExecutionState_ec checkExecutionState_par_con)
{
    bool resultTmp(false);
    bool emitPreparingTmp(false);
    bool emitExecutingTmp(false);
    bool emitStoppingTmp(false);
    //verify if is on a "final" state
    while (not finished_pro and (lastState_f() not_eq checkExecutionState_par_con))
    {
        //states that can't be set outside of this class
        if (equalOnce_ft(checkExecutionState_par_con
                         , checkExecutionState_ec::initial
                         , checkExecutionState_ec::stopped)
        )
        {
            break;
        }

        //from initial it can only go to preparing
        if (lastState_f() == checkExecutionState_ec::initial
            and equalOnce_ft(checkExecutionState_par_con
                              , checkExecutionState_ec::preparing)
        )
        {
            resultTmp = true;
            if (checkExecutionState_par_con == checkExecutionState_ec::preparing)
            {
                emitPreparingTmp = true;
                break;
            }
        }

        //from preparing it can only go to error, executing, stopping
        if (lastState_f() == checkExecutionState_ec::preparing
            and equalOnce_ft(checkExecutionState_par_con
                             , checkExecutionState_ec::error
                             , checkExecutionState_ec::executing
                             , checkExecutionState_ec::stopping)
            )
        {
            resultTmp = true;
            if (checkExecutionState_par_con == checkExecutionState_ec::executing)
            {
                emitExecutingTmp = true;
                break;
            }
            if (checkExecutionState_par_con == checkExecutionState_ec::stopping)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from executing it can only go to error, success, stopping
        if (lastState_f() == checkExecutionState_ec::executing
            and equalOnce_ft(checkExecutionState_par_con
                             , checkExecutionState_ec::error
                             , checkExecutionState_ec::stopping
                             , checkExecutionState_ec::success)
        )
        {
            resultTmp = true;
            if (checkExecutionState_par_con == checkExecutionState_ec::stopping)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from stopping it can only go to error
        if (lastState_f() == checkExecutionState_ec::stopping
            and equalOnce_ft(checkExecutionState_par_con
                             , checkExecutionState_ec::error)
        )
        {
            resultTmp = true;
            if (checkExecutionState_par_con == checkExecutionState_ec::error)
            {
                break;
            }
            break;
        }
        break;
    }
    while (resultTmp)
    {
        executionStateVector_pri.emplace_back(checkExecutionState_par_con);
        //setFinished removed from here because a check might emit a finishing state like error
        //but still need to end its functions calls. The finished signal is connected
        //to deletion of the action execution but then again the check execution object is in another
        //thread so the deletion should wait until the thread is back to its execution loop.
        //Still more stuff that can be signaled from the execution object might happen, more errors or info,
        //and finishing prevents any modification of the results
        Q_EMIT executionStateUpdated_signal(check_ptr_pri);
        if (emitPreparingTmp)
        {
            Q_EMIT preparing_signal(this);
            break;
        }
        if (emitExecutingTmp)
        {
            setStarted_f();
            Q_EMIT executing_signal(this);
            break;
        }
        if (emitStoppingTmp)
        {
            Q_EMIT stopping_signal(this);
            break;
        }
        break;
    }
    return resultTmp;
}

void checkExecutionResult_c::setResult_f(const bool result_par_con)
{
    if (not finished_pro
        and not resultSet_pri
        and lastState_f() == checkExecutionState_ec::executing)
    {
        result_pri = result_par_con;
        resultSet_pri = true;
    }
}

//void checkExecutionResult_c::derivedSetStarted_f()
//{
//    Q_EMIT started_signal(this);
//}

executionResult_c::type_ec checkExecutionResult_c::type_f() const
{
    return executionResult_c::type_ec::check;
}

QString checkExecutionResult_c::stateString_f() const
{
    return checkExecutionStateToString_f(lastState_f());
}

QString checkExecutionResult_c::derivedElementTypeString_f() const
{
    return check_ptr_pri->typeStr_f();
}

QString checkExecutionResult_c::derivedElementDescription_f() const
{
    return check_ptr_pri->description_f();
}

bool checkExecutionResult_c::derivedTrySetFinished_f()
{
    bool resultTmp(false);
    if (resultSet_pri
        and equalOnce_ft(lastState_f()
                         , checkExecutionState_ec::executing
                         , checkExecutionState_ec::error
                         , checkExecutionState_ec::stopping
                         )
    )
    {
        if (equalOnce_ft(lastState_f()
                         , checkExecutionState_ec::executing
                         , checkExecutionState_ec::stopping
                         ))
        {
            if (lastState_f() == checkExecutionState_ec::executing)
            {
                executionStateVector_pri.emplace_back(checkExecutionState_ec::success);
            }
            if (lastState_f() == checkExecutionState_ec::stopping)
            {
                stoppedByUser_pro = true;
                executionStateVector_pri.emplace_back(checkExecutionState_ec::stopped);
            }
            resultTmp = true;
            Q_EMIT executionStateUpdated_signal(check_ptr_pri);
            if (lastState_f() == checkExecutionState_ec::success)
            {
                //finished is always at the end
            }
            if (lastState_f() == checkExecutionState_ec::stopped)
            {
                Q_EMIT stopped_signal(this);
            }
        }
    }
    if (not resultTmp)
    {
        MACRO_ADDLOG(
                    {"Error finishing check execution couldn't finish from {0} state"
                     , stateString_f() }
                    , messageType_ec::error);
    }
    return resultTmp;
}
