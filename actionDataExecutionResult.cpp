#include "actionDataExecutionResult.hpp"

#include "actionData.hpp"

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

actionExecutionState_ec actionDataExecutionResult_c::state_f() const
{
    return executionState_pri;
}

bool actionDataExecutionResult_c::returnCodeSet_f() const
{
    return returnCodeSet_pri;
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

void actionDataExecutionResult_c::clear_f()
{
    output_pri.clear();

    externalOutput_pri.clear();

    externalErrorOutput_pri.clear();

    errors_pri.clear();

    returnCode_pri = -1;
    returnCodeSet_pri = false;

    executionState_pri = actionExecutionState_ec::initial;

    started_pri = false;
    finished_pri = false;
    stoppedByUser_pri = false;

    startTime_pri = 0;
    finishedTime_pri = 0;

    Q_EMIT resultsCleared_signal(parent_ptr_pri);
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
    errors_pri.append(error_par_con);
    Q_EMIT errorUpdated_signal(parent_ptr_pri);
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
        Q_EMIT returnCodeUpdated_signal(parent_ptr_pri);
    }
}

void actionDataExecutionResult_c::setExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con)
{
    //verify if is on a "final" state
    if (not finished_pri)
    {
        executionState_pri = actionExecutionState_par_con;
        if (actionExecutionState_par_con == actionExecutionState_ec::stoppingByUser)
        {
            Q_EMIT stopping_signal(parent_ptr_pri);
        }
        if (actionExecutionState_par_con == actionExecutionState_ec::killingByUser)
        {
            Q_EMIT killing_signal(parent_ptr_pri);
        }
        Q_EMIT executionStateUpdated_signal(parent_ptr_pri);
    }
}

void actionDataExecutionResult_c::setStarted_f()
{
    started_pri = true;
    startTime_pri = QDateTime::currentMSecsSinceEpoch();

    Q_EMIT started_signal(parent_ptr_pri);
}

void actionDataExecutionResult_c::setFinished_f()
{
    if (executionState_pri == actionExecutionState_ec::stoppingByUser)
    {
        stoppedByUser_pri = true;
        setExecutionState_f(actionExecutionState_ec::stoppedByUser);
        Q_EMIT stopped_signal(parent_ptr_pri);
    }
    if (executionState_pri == actionExecutionState_ec::killingByUser)
    {
        killedByUser_pri = true;
        setExecutionState_f(actionExecutionState_ec::killedByUser);
        Q_EMIT killed_signal(parent_ptr_pri);
    }

    finished_pri = true;
    finishedTime_pri = QDateTime::currentMSecsSinceEpoch();

    Q_EMIT finished_signal(parent_ptr_pri);
}
