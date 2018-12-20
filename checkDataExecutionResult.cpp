#include "checkDataExecutionResult.hpp"

#include "actionData.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QDateTime>

QString checkDataExecutionResult_c::error_f() const
{
    return errors_pri;
}

std::vector<checkExecutionState_ec> checkDataExecutionResult_c::executionStateVector_f() const
{
    return executionStateVector_pri;
}

checkExecutionState_ec checkDataExecutionResult_c::lastState_f() const
{
    return executionStateVector_pri.back();
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

bool checkDataExecutionResult_c::tryClear_f()
{
    bool resultTmp(false);
    if (finished_pri)
    {
        errors_pri.clear();

        executionStateVector_pri = { checkExecutionState_ec::initial };

        result_pri = false;

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
    Q_EMIT error_signal(parent_ptr_pri);
}

bool checkDataExecutionResult_c::trySetExecutionState_f(const checkExecutionState_ec checkExecutionState_par_con)
{
    bool resultTmp(false);
    bool finishItTmp(false);
    bool emitPreparingTmp(false);
    bool emitExecutingTmp(false);
    bool emitStoppingTmp(false);
    bool emitErrorTmp(false);
    //verify if is on a "final" state
    while (not finished_pri and (lastState_f() not_eq checkExecutionState_par_con))
    {
        //actions that can't be set outside of this class
        if (equalOnce_ft(checkExecutionState_par_con
                         , checkExecutionState_ec::initial
                         , checkExecutionState_ec::stoppedByUser)
        )
        {
            break;
        }

        //from initial it can only go to preparing, executing checks
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

        //from preparing it can only go to executing, stopping
        if (lastState_f() == checkExecutionState_ec::preparing
            and equalOnce_ft(checkExecutionState_par_con
                             , checkExecutionState_ec::error
                             , checkExecutionState_ec::executing
                             , checkExecutionState_ec::stoppingByUser)
            )
        {
            resultTmp = true;
            if (checkExecutionState_par_con == checkExecutionState_ec::error)
            {
                emitErrorTmp = true;
                finishItTmp = true;
                break;
            }
            if (checkExecutionState_par_con == checkExecutionState_ec::executing)
            {
                emitExecutingTmp = true;
                break;
            }
            if (checkExecutionState_par_con == checkExecutionState_ec::stoppingByUser)
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
                             , checkExecutionState_ec::stoppingByUser
                             , checkExecutionState_ec::finishedFalse
                             , checkExecutionState_ec::finishedTrue)
        )
        {
            resultTmp = true;
            if (checkExecutionState_par_con == checkExecutionState_ec::error)
            {
                emitErrorTmp = true;
                finishItTmp = true;
                break;
            }
            if (checkExecutionState_par_con == checkExecutionState_ec::stoppingByUser)
            {
                emitStoppingTmp = true;
                break;
            }
            if (equalOnce_ft(checkExecutionState_par_con
                             , checkExecutionState_ec::finishedFalse
                             , checkExecutionState_ec::finishedTrue))
            {
                finishItTmp = true;
                break;
            }
            break;
        }

        //from stopping it can only go to error, success
        if (lastState_f() == checkExecutionState_ec::stoppingByUser
            and equalOnce_ft(checkExecutionState_par_con
                             , checkExecutionState_ec::error
                             , checkExecutionState_ec::finishedFalse
                             , checkExecutionState_ec::finishedTrue)
        )
        {
            resultTmp = true;
            if (checkExecutionState_par_con == checkExecutionState_ec::error)
            {
                emitErrorTmp = true;
                finishItTmp = true;
                break;
            }
            if (equalOnce_ft(checkExecutionState_par_con
                             , checkExecutionState_ec::finishedFalse
                             , checkExecutionState_ec::finishedTrue))
            {
                finishItTmp = true;
                break;
            }
            break;
        }
        break;
    }
    while (resultTmp)
    {
        executionStateVector_pri.emplace_back(checkExecutionState_par_con);
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
        if (emitExecutingTmp)
        {
            setStarted_f();
            Q_EMIT executing_signal(parent_ptr_pri);
            break;
        }
        if (emitStoppingTmp)
        {
            Q_EMIT stopping_signal(parent_ptr_pri);
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

void checkDataExecutionResult_c::setStarted_f()
{
    if (not started_pri)
    {
        started_pri = true;
        startTime_pri = QDateTime::currentMSecsSinceEpoch();

        Q_EMIT started_signal(parent_ptr_pri);
    }
}

void checkDataExecutionResult_c::trySetFinished_f()
{
    if (not finished_pri
        and equalOnce_ft(lastState_f()
                         , checkExecutionState_ec::finishedTrue
                         , checkExecutionState_ec::finishedFalse
                         , checkExecutionState_ec::error
                         , checkExecutionState_ec::stoppingByUser
                         )
    )
    {
        finished_pri = true;
        if (startTime_pri not_eq 0)
        {
            finishedTime_pri = QDateTime::currentMSecsSinceEpoch();
        }
        result_pri = lastState_f() == checkExecutionState_ec::finishedTrue;

        if (lastState_f() == checkExecutionState_ec::stoppingByUser)
        {
            stoppedByUser_pri = true;
            executionStateVector_pri.emplace_back(checkExecutionState_ec::stoppedByUser);
            Q_EMIT stopped_signal(parent_ptr_pri);
        }

        Q_EMIT finished_signal(parent_ptr_pri);
    }
}
