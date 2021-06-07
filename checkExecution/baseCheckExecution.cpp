#include "baseCheckExecution.hpp"

#include "../checkDataExecutionResult.hpp"
#include "../checkData.hpp"

void baseCheckExecution_c::setExecutionError_f(const executionMessage_c* messagePtr_par_con)
{
    if (messagePtr_par_con->type_f() == executionMessage_c::type_ec::error)
    {
        executionError_pri = true;
    }
}

baseCheckExecution_c::baseCheckExecution_c(checkExecutionResult_c* checkExecutionResultObj_par_con)
    : checkExecutionResultObj_pro(checkExecutionResultObj_par_con)
{
    QObject::connect(this, &baseCheckExecution_c::executionStateChange_signal, checkExecutionResultObj_pro, &checkExecutionResult_c::trySetExecutionState_f);
    QObject::connect(this, &baseCheckExecution_c::addExecutionMessage_signal, this, &baseCheckExecution_c::setExecutionError_f);
    QObject::connect(this, &baseCheckExecution_c::addExecutionMessage_signal, checkExecutionResultObj_pro, &checkExecutionResult_c::appendMessage_f);
    QObject::connect(this, &baseCheckExecution_c::anyFinish_signal, checkExecutionResultObj_pro, &checkExecutionResult_c::trySetFinished_f);
    QObject::connect(checkExecutionResultObj_pro, &checkExecutionResult_c::finished_signal, this, &QObject::deleteLater);
}

void baseCheckExecution_c::emitExecutionMessage_f(const textCompilation_c& message_par_con, const executionMessage_c::type_ec type_par_con)
{
    //message lifetime created using this funcion is tied to the actionDataExecutionResult_c object
    executionMessage_c* msgTmp(new executionMessage_c(checkExecutionResultObj_pro, checkExecutionResultObj_pro->check_ptr_f()->reference_f(), message_par_con, type_par_con));
    Q_EMIT addExecutionMessage_signal(msgTmp);
}

void baseCheckExecution_c::emitExecutionMessage_f(const text_c& message_par_con, const executionMessage_c::type_ec type_par_con)
{
    //message lifetime created using this funcion is tied to the actionDataExecutionResult_c object
    executionMessage_c* msgTmp(new executionMessage_c(checkExecutionResultObj_pro, checkExecutionResultObj_pro->check_ptr_f()->reference_f(), textCompilation_c(message_par_con), type_par_con));
    Q_EMIT addExecutionMessage_signal(msgTmp);
}

void baseCheckExecution_c::execute_f()
{
    //this is here because if an execution is waiting for a thread to start and execution stops/kill
    //it might go through here
    if (checkExecutionResultObj_pro->lastState_f() == checkExecutionState_ec::stopping)
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
