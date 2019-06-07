#include "baseCheckExecution.hpp"

#include "../checkDataExecutionResult.hpp"
#include "../checkData.hpp"

void baseCheckExecution_c::setExecutionError_f()
{
    executionError_pri = true;
}

baseCheckExecution_c::baseCheckExecution_c(checkDataExecutionResult_c* checkExecutionResultObj_par_con)
    : checkExecutionResultObj_pro(checkExecutionResultObj_par_con)
{
    QObject::connect(this, &baseCheckExecution_c::executionStateChange_signal, checkExecutionResultObj_pro, &checkDataExecutionResult_c::trySetExecutionState_f);
    QObject::connect(this, &baseCheckExecution_c::addError_signal, this, &baseCheckExecution_c::setExecutionError_f);
    QObject::connect(this, &baseCheckExecution_c::addError_signal, checkExecutionResultObj_pro, &checkDataExecutionResult_c::appendError_f);
    QObject::connect(this, &baseCheckExecution_c::anyFinish_signal, checkExecutionResultObj_pro, &checkDataExecutionResult_c::trySetFinished_f);
    QObject::connect(checkExecutionResultObj_pro, &checkDataExecutionResult_c::finished_signal, this, &QObject::deleteLater);
}

void baseCheckExecution_c::execute_f()
{
    //this is here because if an execution is waiting for a thread to start and execution stops/kill
    //it might go through here
    if (checkExecutionResultObj_pro->lastState_f() == checkExecutionState_ec::stoppingByUser)
    {
        //try to finish
        Q_EMIT anyFinish_signal(false);
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

bool baseCheckExecution_c::executionError_f() const
{
    return executionError_pri;
}
