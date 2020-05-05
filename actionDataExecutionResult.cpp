#include "actionDataExecutionResult.hpp"

#include "actionData.hpp"
#include "actonDataHub.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QDateTime>

textCompilation_c actionDataExecutionResult_c::errors_f() const
{
    return errors_pri;
}

QString actionDataExecutionResult_c::externalOutput_f() const
{
    return externalOutput_pri;
}

QString actionDataExecutionResult_c::externalErrorOutput_f() const
{
    return externalErrorOutput_pri;
}

int actionDataExecutionResult_c::returnCode_f() const
{
    return returnCode_pri;
}

actionExecutionState_ec actionDataExecutionResult_c::lastState_f() const
{
    return executionStateVector_pri.back();
}

bool actionDataExecutionResult_c::returnCodeSet_f() const
{
    return returnCodeSet_pri;
}

std::vector<actionExecutionState_ec> actionDataExecutionResult_c::executionStateVector_f() const
{
    return executionStateVector_pri;
}

action_c* actionDataExecutionResult_c::parentAction_ptr_f() const
{
    return parentAction_ptr_pri;
}

bool actionDataExecutionResult_c::started_f() const
{
    return started_pri;
}

bool actionDataExecutionResult_c::finished_f() const
{
    return finished_pri;
}


qint64 actionDataExecutionResult_c::startTime_f() const
{
    return startTime_pri;
}

qint64 actionDataExecutionResult_c::finishedTime_f() const
{
    return finishedTime_pri;
}

bool actionDataExecutionResult_c::tryClear_f()
{
    bool resultTmp(false);
    if (finished_pri)
    {
        output_pri.clear_f();

        externalOutput_pri.clear();

        externalErrorOutput_pri.clear();

        errors_pri.clear_f();

        returnCode_pri = -1;
        returnCodeSet_pri = false;

        executionStateVector_pri = { actionExecutionState_ec::initial };

        started_pri = false;
        finished_pri = false;
        stoppedByUser_pri = false;

        startTime_pri = 0;
        finishedTime_pri = 0;

        resultTmp = true;
        Q_EMIT resultsCleared_signal(parentAction_ptr_pri);
    }
    return resultTmp;
}

bool actionDataExecutionResult_c::stoppedByUser_f() const
{
    return stoppedByUser_pri;
}

actionDataExecutionResult_c::actionDataExecutionResult_c(
        action_c* const parentAction_par_ptr_con
        )
    : parentAction_ptr_pri(parentAction_par_ptr_con)
{}

textCompilation_c actionDataExecutionResult_c::output_f() const
{
    return output_pri;
}

void actionDataExecutionResult_c::appendOutput_f(const text_c& output_par_con)
{
    if (not finished_pri)
    {
        output_pri.append_f(output_par_con);
        Q_EMIT outputUpdated_signal(parentAction_ptr_pri, output_par_con);
    }
}

void actionDataExecutionResult_c::appendError_f(const text_c& error_par_con)
{
    if (not finished_pri)
    {
        MACRO_ADDACTONQTSOLOG(error_par_con, parentAction_ptr_pri, logItem_c::type_ec::warning);
        errors_pri.append_f(error_par_con);
        trySetExecutionState_f(actionExecutionState_ec::error);
        Q_EMIT error_signal(parentAction_ptr_pri, error_par_con);
    }
}

void actionDataExecutionResult_c::appendErrors_f(const textCompilation_c& errors_par_con)
{
    if (not finished_pri)
    {
        errors_pri.append_f(errors_par_con);
        trySetExecutionState_f(actionExecutionState_ec::error);
#ifdef DEBUGJOUVEN
        for (int_fast32_t i = 0, l = errors_par_con.size_f(); i < l; ++i)
        {
            MACRO_ADDACTONQTSOLOG(errors_par_con.text_f(i), parentAction_ptr_pri, logItem_c::type_ec::debug);
        }
#endif
        Q_EMIT errors_signal(parentAction_ptr_pri, errors_par_con);
    }
}

void actionDataExecutionResult_c::appendExternalOutput_f(const QString& actionOutput_par_con)
{
    if (not finished_pri)
    {
        externalOutput_pri.append(actionOutput_par_con);
        Q_EMIT externalOutputUpdated_signal(parentAction_ptr_pri, actionOutput_par_con);
    }
}

void actionDataExecutionResult_c::appendExternalError_f(const QString& actionError_par_con)
{
    if (not finished_pri)
    {
        externalErrorOutput_pri.append(actionError_par_con);
        Q_EMIT externalErrorUpdated_signal(parentAction_ptr_pri, actionError_par_con);
    }
}

void actionDataExecutionResult_c::setReturnCode_f(const int returnCode_par_con)
{
    if (not finished_pri and not returnCodeSet_pri)
    {
        returnCode_pri = returnCode_par_con;
        returnCodeSet_pri = true;
        Q_EMIT returnCodeSet_signal(parentAction_ptr_pri);
    }
}

bool actionDataExecutionResult_c::trySetExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con)
{
    bool validExecutionStateChangeTmp(false);
    bool emitPreparingTmp(false);
    bool emitExecutingChecksTmp(false);
    bool emitExecutingTmp(false);
    bool emitStoppingTmp(false);
    bool emitKillingTmp(false);
    //verify if is on a "final" state
    while (not finished_pri and (lastState_f() not_eq actionExecutionState_par_con))
    {
        //states that can't be set outside of this class
        if (equalOnce_ft(actionExecutionState_par_con
                         , actionExecutionState_ec::initial
                         , actionExecutionState_ec::stopped
                         , actionExecutionState_ec::killed)
        )
        {
            break;
        }

        //from initial it can only go to preparing, executing checks
        if (lastState_f() == actionExecutionState_ec::initial
            and equalOnce_ft(actionExecutionState_par_con
                              , actionExecutionState_ec::preparing
                              , actionExecutionState_ec::executingChecks)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::preparing)
            {
                emitPreparingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::executingChecks)
            {
                emitExecutingChecksTmp = true;
                break;
            }
        }

        //from executhing checks it can only go to preparing, stopping/killing and checks failed
        if (lastState_f() == actionExecutionState_ec::executingChecks
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::preparing
                             , actionExecutionState_ec::stopping
                             , actionExecutionState_ec::killing
                             , actionExecutionState_ec::checksFailed)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::preparing)
            {
                emitPreparingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::stopping
                or actionExecutionState_par_con == actionExecutionState_ec::killing)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from preparing it can only go to error, executing, stopping/killing and success.
        //Allowing success has to do with some actions checking if what the action is going to do has already happened
        //and usually there is a setting that treats that as an error or success
        //skipping the running part
        if (lastState_f() == actionExecutionState_ec::preparing
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::executing
                             , actionExecutionState_ec::stopping
                             , actionExecutionState_ec::killing
                             , actionExecutionState_ec::success)
            )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::executing)
            {
                emitExecutingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::stopping
                or actionExecutionState_par_con == actionExecutionState_ec::killing)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from executing it can only go to error, success, stopping
        if (lastState_f() == actionExecutionState_ec::executing
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::stopping
                             , actionExecutionState_ec::success)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::stopping)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from stoppingByUser it can only go to error or killing
        if (lastState_f() == actionExecutionState_ec::stopping
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::killing)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::killing)
            {
                emitKillingTmp = true;
                break;
            }
            break;
        }

        break;
    }
    while (validExecutionStateChangeTmp)
    {
        executionStateVector_pri.emplace_back(actionExecutionState_par_con);
        //setFinished removed from here because an action might emit a finishing state like error
        //but still need to end its functions calls. The finished signal is connected
        //to deletion of the action execution but then again the action execution object is in another
        //thread so the deletion should wait until the thread is back to its execution loop.
        //Still more stuff that can be signaled from the execution object might happen, more errors or info,
        //and finishing prevents any modification of the results
        Q_EMIT executionStateUpdated_signal(parentAction_ptr_pri, actionExecutionState_par_con);
        if (emitPreparingTmp)
        {
            Q_EMIT preparing_signal(parentAction_ptr_pri);
            break;
        }
        if (emitExecutingChecksTmp)
        {
            Q_EMIT executingChecks_signal(parentAction_ptr_pri);
            break;
        }
        if (emitExecutingTmp)
        {
            setStarted_f();
            Q_EMIT executing_signal(parentAction_ptr_pri);
            break;
        }
        if (emitStoppingTmp)
        {
            Q_EMIT stopping_signal(parentAction_ptr_pri);
            break;
        }
        if (emitKillingTmp)
        {
            Q_EMIT killing_signal(parentAction_ptr_pri);
            break;
        }
        break;
    }
    return validExecutionStateChangeTmp;
}

bool actionDataExecutionResult_c::killedByUser_f() const
{
    return killedByUser_pri;
}

void actionDataExecutionResult_c::setStarted_f()
{
    if (not started_pri)
    {
        started_pri = true;
        startTime_pri = QDateTime::currentMSecsSinceEpoch();

        Q_EMIT started_signal(parentAction_ptr_pri);
    }
}

void actionDataExecutionResult_c::trySetFinished_f()
{
    if (not finished_pri
        and equalOnce_ft(lastState_f()
                         , actionExecutionState_ec::executing
                         //error is special because it's a final state already
                         , actionExecutionState_ec::error
                         , actionExecutionState_ec::killing
                         , actionExecutionState_ec::stopping
                         //executing checks finishing means the check/s logic result is false
                         //and the action won't be executed after
                         , actionExecutionState_ec::executingChecks
                         )
    )
    {
        finished_pri = true;
        //in some cases the action can end before "properly" starting
        if (startTime_pri not_eq 0)
        {
            finishedTime_pri = QDateTime::currentMSecsSinceEpoch();
        }

        if (equalOnce_ft(lastState_f()
                         , actionExecutionState_ec::executing
                         , actionExecutionState_ec::killing
                         , actionExecutionState_ec::stopping
                         , actionExecutionState_ec::executingChecks
                         ))
        {
            actionExecutionState_ec finishedStateTmp;
            if (lastState_f() == actionExecutionState_ec::executing)
            {
                finishedStateTmp = actionExecutionState_ec::success;
            }
            if (lastState_f() == actionExecutionState_ec::stopping)
            {
                stoppedByUser_pri = true;
                finishedStateTmp = actionExecutionState_ec::stopped;
            }
            if (lastState_f() == actionExecutionState_ec::killing)
            {
                killedByUser_pri = true;
                finishedStateTmp = actionExecutionState_ec::killed;
            }
            if (lastState_f() == actionExecutionState_ec::executingChecks)
            {
                finishedStateTmp = actionExecutionState_ec::checksFailed;
            }
            executionStateVector_pri.emplace_back(finishedStateTmp);
            Q_EMIT executionStateUpdated_signal(parentAction_ptr_pri, finishedStateTmp);
            if (lastState_f() == actionExecutionState_ec::success)
            {
                Q_EMIT success_signal(parentAction_ptr_pri);
            }
            if (lastState_f() == actionExecutionState_ec::stopped)
            {
                Q_EMIT stopped_signal(parentAction_ptr_pri);
            }
            if (lastState_f() == actionExecutionState_ec::killed)
            {
                Q_EMIT killed_signal(parentAction_ptr_pri);
            }
        }
        Q_EMIT finished_signal(parentAction_ptr_pri);
    }
}
