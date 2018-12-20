#include "actionDataExecutionResult.hpp"

#include "actionData.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QDateTime>

QString actionDataExecutionResult_c::error_f() const
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

actionData_c* actionDataExecutionResult_c::parent_ptr_f() const
{
    return parent_ptr_pri;
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
        output_pri.clear();

        externalOutput_pri.clear();

        externalErrorOutput_pri.clear();

        errors_pri.clear();

        returnCode_pri = -1;
        returnCodeSet_pri = false;

        executionStateVector_pri = { actionExecutionState_ec::initial };

        started_pri = false;
        finished_pri = false;
        stoppedByUser_pri = false;

        startTime_pri = 0;
        finishedTime_pri = 0;

        resultTmp = true;
        Q_EMIT resultsCleared_signal(parent_ptr_pri);
    }
    return resultTmp;
}

bool actionDataExecutionResult_c::stoppedByUser_f() const
{
    return stoppedByUser_pri;
}

actionDataExecutionResult_c::actionDataExecutionResult_c(
        actionData_c* const parentActionData_par_ptr_con
        )
    : parent_ptr_pri(parentActionData_par_ptr_con)
{}

QString actionDataExecutionResult_c::output_f() const
{
    return output_pri;
}

void actionDataExecutionResult_c::appendOutput_f(const QString& output_par_con)
{
    output_pri.append(output_par_con);
    Q_EMIT outputUpdated_signal(parent_ptr_pri);
}

void actionDataExecutionResult_c::appendError_f(const QString& error_par_con)
{
    trySetExecutionState_f(actionExecutionState_ec::error);
    errors_pri.append(error_par_con);
    Q_EMIT error_signal(parent_ptr_pri);
}

void actionDataExecutionResult_c::appendExternalOutput_f(const QString& actionOutput_par_con)
{
    externalOutput_pri.append(actionOutput_par_con);
    Q_EMIT externalOutputUpdated_signal(parent_ptr_pri);
}

void actionDataExecutionResult_c::appendExternalError_f(const QString& actionError_par_con)
{
    externalErrorOutput_pri.append(actionError_par_con);
    Q_EMIT externalErrorUpdated_signal(parent_ptr_pri);
}

void actionDataExecutionResult_c::setReturnCode_f(const int returnCode_par_con)
{
    if (not returnCodeSet_pri)
    {
        returnCode_pri = returnCode_par_con;
        returnCodeSet_pri = true;
        Q_EMIT returnCodeSet_signal(parent_ptr_pri);
    }
}

bool actionDataExecutionResult_c::trySetExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con)
{
    bool resultTmp(false);
    bool finishItTmp(false);
    bool emitPreparingTmp(false);
    bool emitExecutingChecksTmp(false);
    bool emitExecutingTmp(false);
    bool emitStoppingTmp(false);
    bool emitKillingTmp(false);
    bool emitSuccessTmp(false);
    bool emitErrorTmp(false);
    //verify if is on a "final" state
    while (not finished_pri and (lastState_f() not_eq actionExecutionState_par_con))
    {
        //actions that can't be set outside of this class
        if (equalOnce_ft(actionExecutionState_par_con
                         , actionExecutionState_ec::initial
                         , actionExecutionState_ec::stoppedByUser
                         , actionExecutionState_ec::killedByUser)
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
            resultTmp = true;
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

        //from executhing checks it can only go to preparing, stopping/killing
        if (lastState_f() == actionExecutionState_ec::executingChecks
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::preparing
                             , actionExecutionState_ec::stoppingByUser
                             , actionExecutionState_ec::killingByUser)
        )
        {
            resultTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::preparing)
            {
                emitPreparingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::stoppingByUser
                or actionExecutionState_par_con == actionExecutionState_ec::killingByUser)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from preparing it can only go to executing, stopping/killing
        if (lastState_f() == actionExecutionState_ec::preparing
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::executing
                             , actionExecutionState_ec::stoppingByUser
                             , actionExecutionState_ec::killingByUser)
            )
        {
            resultTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::error)
            {
                emitErrorTmp = true;
                finishItTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::executing)
            {
                emitExecutingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::stoppingByUser
                or actionExecutionState_par_con == actionExecutionState_ec::killingByUser)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from executing it can only go to error, success, stopping/killing
        if (lastState_f() == actionExecutionState_ec::executing
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::stoppingByUser
                             , actionExecutionState_ec::killingByUser
                             , actionExecutionState_ec::success)
        )
        {
            resultTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::success)
            {
                finishItTmp = true;
                emitSuccessTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::error)
            {
                emitErrorTmp = true;
                finishItTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::stoppingByUser)
            {
                emitStoppingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::killingByUser)
            {
                emitKillingTmp = true;
                break;
            }
            break;
        }
        break;
    }
    while (resultTmp)
    {
        executionStateVector_pri.emplace_back(actionExecutionState_par_con);
        if (finishItTmp)
        {
            trySetFinished_f();
        }
        Q_EMIT executionStateUpdated_signal(parent_ptr_pri);
        if (emitPreparingTmp)
        {
            Q_EMIT preparing_signal(parent_ptr_pri);
            break;
        }
        if (emitExecutingChecksTmp)
        {
            Q_EMIT executingChecks_signal(parent_ptr_pri);
            break;
        }
        if (emitExecutingTmp)
        {
            setStarted_f();
            Q_EMIT executing_signal(parent_ptr_pri);
            break;
        }
        if (emitSuccessTmp)
        {
            Q_EMIT success_signal(parent_ptr_pri);
            break;
        }
        if (emitStoppingTmp)
        {
            Q_EMIT stopping_signal(parent_ptr_pri);
            break;
        }
        if (emitKillingTmp)
        {
            Q_EMIT killing_signal(parent_ptr_pri);
            break;
        }
        if (emitErrorTmp)
        {
            Q_EMIT error_signal(parent_ptr_pri);
            break;
        }
        break;
    }
    return resultTmp;
}

void actionDataExecutionResult_c::setStarted_f()
{
    if (not started_pri)
    {
        started_pri = true;
        startTime_pri = QDateTime::currentMSecsSinceEpoch();

        Q_EMIT started_signal(parent_ptr_pri);
    }
}

void actionDataExecutionResult_c::trySetFinished_f()
{
    if (not finished_pri
        and equalOnce_ft(lastState_f()
                         , actionExecutionState_ec::success
                         , actionExecutionState_ec::error
                         , actionExecutionState_ec::killingByUser
                         , actionExecutionState_ec::stoppingByUser
                         )
    )
    {
        finished_pri = true;
        if (startTime_pri not_eq 0)
        {
            finishedTime_pri = QDateTime::currentMSecsSinceEpoch();
        }

        if (lastState_f() == actionExecutionState_ec::stoppingByUser)
        {
            stoppedByUser_pri = true;
            executionStateVector_pri.emplace_back(actionExecutionState_ec::stoppedByUser);
            Q_EMIT stopped_signal(parent_ptr_pri);
        }
        if (lastState_f() == actionExecutionState_ec::killingByUser)
        {
            killedByUser_pri = true;
            executionStateVector_pri.emplace_back(actionExecutionState_ec::killedByUser);
            Q_EMIT killed_signal(parent_ptr_pri);
        }

        Q_EMIT finished_signal(parent_ptr_pri);
    }
}
