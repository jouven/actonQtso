#include "checkDataExecutionResult.hpp"

#include "actionData.hpp"

#include <QDateTime>

QString checkDataExecutionResult_c::error_f() const
{
    return errors_pri;
}

checkExecutionState_ec checkDataExecutionResult_c::state_f() const
{
    return executionState_pri;
}

checkData_c* checkDataExecutionResult_c::parent_ptr_f() const
{
    return parent_ptr_pri;
}

bool checkDataExecutionResult_c::started_f() const
{
    return started_pri;
}

bool checkDataExecutionResult_c::finished_f() const
{
    return finished_pri;
}


qint64 checkDataExecutionResult_c::startTime_f() const
{
    return startTime_pri;
}

qint64 checkDataExecutionResult_c::finishedTime_f() const
{
    return finishedTime_pri;
}

bool checkDataExecutionResult_c::result_f() const
{
    return result_pri;
}

void checkDataExecutionResult_c::clear_f()
{
    errors_pri.clear();

    executionState_pri = checkExecutionState_ec::initial;

    result_pri = false;

    started_pri = false;
    finished_pri = false;
    stoppedByUser_pri = false;

    startTime_pri = 0;
    finishedTime_pri = 0;

    Q_EMIT resultsCleared_signal(parent_ptr_pri);
}

bool checkDataExecutionResult_c::stoppedByUser_f() const
{
    return stoppedByUser_pri;
}

checkDataExecutionResult_c::checkDataExecutionResult_c(
        checkData_c* const parentActionData_par_ptr_con
        )
    : parent_ptr_pri(parentActionData_par_ptr_con)
{}

void checkDataExecutionResult_c::appendError_f(const QString& error_par_con)
{
    errors_pri.append(error_par_con);
    Q_EMIT errorUpdated_signal(parent_ptr_pri);
}

void checkDataExecutionResult_c::setExecutionState_f(const checkExecutionState_ec checkExecutionState_par_con)
{
    if (not finished_pri)
    {
        executionState_pri = checkExecutionState_par_con;
        if (checkExecutionState_par_con == checkExecutionState_ec::stoppingByUser)
        {
            Q_EMIT stopping_signal(parent_ptr_pri);
        }
        Q_EMIT executionStateUpdated_signal(parent_ptr_pri);
    }
}

void checkDataExecutionResult_c::setStarted_f()
{
    started_pri = true;
    startTime_pri = QDateTime::currentMSecsSinceEpoch();

    Q_EMIT started_signal(parent_ptr_pri);
}

void checkDataExecutionResult_c::setFinished_f(const bool checkResult_par_con)
{
    if (executionState_pri == checkExecutionState_ec::stoppingByUser)
    {
        stoppedByUser_pri = true;
        setExecutionState_f(checkExecutionState_ec::stoppedByUser);
        Q_EMIT stopped_signal(parent_ptr_pri);
    }

    finished_pri = true;
    finishedTime_pri = QDateTime::currentMSecsSinceEpoch();
    result_pri = checkResult_par_con;

    Q_EMIT finished_signal(parent_ptr_pri);
}
