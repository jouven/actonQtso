#include "baseCheckExecution.hpp"

#include "../checkDataExecutionResult.hpp"
#include "../checkData.hpp"

baseCheckExecution_c::baseCheckExecution_c(checkDataExecutionResult_c* checkExecutionResultObj_par_con)
    : checkExecutionResultObj_pri(checkExecutionResultObj_par_con)
{
    QObject::connect(this, &baseCheckExecution_c::executionStateChange_signal, checkExecutionResultObj_pri, &checkDataExecutionResult_c::trySetExecutionState_f);
    QObject::connect(this, &baseCheckExecution_c::addError_signal, checkExecutionResultObj_pri, &checkDataExecutionResult_c::appendError_f);
    QObject::connect(checkExecutionResultObj_pri, &checkDataExecutionResult_c::finished_signal, this, &QObject::deleteLater);
}

void baseCheckExecution_c::execute_f()
{
    //this is here because if an execution is waiting for a thread to start and execution stops/kill
    //it might go through here
    if (checkExecutionResultObj_pri->lastState_f() == checkExecutionState_ec::stoppingByUser)
    {
        //try to finish
        Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedFalse);
    }
    else
    {
        derivedExecute_f();
    }
}

void baseCheckExecution_c::stop_f()
{
    derivedStop_f();
}
