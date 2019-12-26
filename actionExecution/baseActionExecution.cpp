#include "baseActionExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actionData.hpp"

#include "textQtso/text.hpp"

void baseActionExecution_c::setExecutionError_f()
{
    executionError_pri = true;
}

baseActionExecution_c::baseActionExecution_c(actionDataExecutionResult_c* actionExecutionResultObj_par_con)
    : actionExecutionResultObj_pri(actionExecutionResultObj_par_con)
{
    QObject::connect(this, &baseActionExecution_c::executionStateChange_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::trySetExecutionState_f);
    QObject::connect(this, &baseActionExecution_c::addError_signal, this, &baseActionExecution_c::setExecutionError_f);
    QObject::connect(this, &baseActionExecution_c::addError_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::appendError_f);
    QObject::connect(this, &baseActionExecution_c::addErrors_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::appendErrors_f);
    QObject::connect(this, &baseActionExecution_c::addOutput_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::appendOutput_f);
    QObject::connect(this, &baseActionExecution_c::anyFinish_signal, actionExecutionResultObj_pri, &actionDataExecutionResult_c::trySetFinished_f);
    QObject::connect(actionExecutionResultObj_pri, &actionDataExecutionResult_c::finished_signal, this, &QObject::deleteLater);
}

void baseActionExecution_c::execute_f()
{
    //20191102 this is not necessary because it's done on action_c::execute_f() (which call this)
//    //this is here because if an execution is waiting for a thread to start and execution stops/"gets killed"
//    //it will go through here first after threads are available from other action stopping/"getting killed"
//    if (actionExecutionResultObj_pri->lastState_f() == actionExecutionState_ec::stopping
//        or actionExecutionResultObj_pri->lastState_f() == actionExecutionState_ec::killing)
//    {
//        //try to finish
//        Q_EMIT anyFinish_signal();
//    }
//    else
//    {
        derivedExecute_f();
//    }
}

void baseActionExecution_c::stop_f()
{
    derivedStop_f();
}

void baseActionExecution_c::kill_f()
{
    derivedKill_f();
}

bool baseActionExecution_c::executionError_f() const
{
    return executionError_pri;
}
