#include "baseActionExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actionData.hpp"

baseActionExecution_c::baseActionExecution_c(actionDataExecutionResult_c* actionExecutionResultObj_par_con)
    : actionExecutionResultObj_pri(actionExecutionResultObj_par_con)
{
    QObject::connect(this, &baseActionExecution_c::executionStateChange_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::trySetExecutionState_f);
    QObject::connect(this, &baseActionExecution_c::addError_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::appendError_f);
    QObject::connect(this, &baseActionExecution_c::addOutput_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::appendOutput_f);
    QObject::connect(this, &baseActionExecution_c::anyFinish_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::trySetFinished_f);
    QObject::connect(actionExecutionResultObj_pri, &actionDataExecutionResult_c::finished_signal, this, &QObject::deleteLater);
}

void baseActionExecution_c::execute_f()
{
    //this is here because if an execution is waiting for a thread to start and execution stops/kill
    //it might go through here
    if (actionExecutionResultObj_pri->lastState_f() == actionExecutionState_ec::stoppingByUser
        or actionExecutionResultObj_pri->lastState_f() == actionExecutionState_ec::killingByUser)
    {
        //try to finish
        Q_EMIT anyFinish_signal();
    }
    else
    {
        derivedExecute_f();
    }
}

void baseActionExecution_c::stop_f()
{
    derivedStop_f();
}

void baseActionExecution_c::kill_f()
{
    derivedKill_f();
}
