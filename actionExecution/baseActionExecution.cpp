#include "baseActionExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actionData.hpp"

#include "textQtso/text.hpp"

void baseActionExecution_c::setExecutionError_f(const executionMessage_c* messagePtr_par_con)
{
    if (messagePtr_par_con->type_f() == executionMessage_c::type_ec::error)
    {
        executionError_pri = true;
    }
}

baseActionExecution_c::baseActionExecution_c(actionExecutionResult_c* actionExecutionResultObj_par_con)//, QObject* parent_par)
    : QObject(nullptr), actionExecutionResultObj_pri(actionExecutionResultObj_par_con)
{
    QObject::connect(this, &baseActionExecution_c::executionStateChange_signal, actionExecutionResultObj_pri, &actionExecutionResult_c::trySetExecutionState_f);
    QObject::connect(this, &baseActionExecution_c::addExecutionMessage_signal, this, &baseActionExecution_c::setExecutionError_f);
    QObject::connect(this, &baseActionExecution_c::addExecutionMessage_signal, actionExecutionResultObj_pri, &actionExecutionResult_c::appendMessage_f);
    QObject::connect(this, &baseActionExecution_c::anyFinish_signal, actionExecutionResultObj_pri, &actionExecutionResult_c::trySetFinished_f);
    QObject::connect(actionExecutionResultObj_pri, &actionExecutionResult_c::finished_signal, this, &QObject::deleteLater);
}

void baseActionExecution_c::emitExecutionMessage_f(const textCompilation_c& message_par_con, const executionMessage_c::type_ec type_par_con)
{
    //message lifetime created using this funcion is tied to the actionDataExecutionResult_c object
    executionMessage_c* msgTmp(new executionMessage_c(actionExecutionResultObj_pri, actionExecutionResultObj_pri->action_ptr_f()->reference_f(), message_par_con, type_par_con));
    Q_EMIT addExecutionMessage_signal(msgTmp);
}

void baseActionExecution_c::emitExecutionMessage_f(const text_c& message_par_con, const executionMessage_c::type_ec type_par_con)
{
    //message lifetime created using this funcion is tied to the actionDataExecutionResult_c object
    executionMessage_c* msgTmp(new executionMessage_c(
                                   actionExecutionResultObj_pri
                                   , actionExecutionResultObj_pri->action_ptr_f()->reference_f()
                                   , textCompilation_c(message_par_con)
                                   , type_par_con)
                               );
#ifdef DEBUGJOUVEN
    //qDebug() << "message_par_con.text() " << message_par_con.rawReplace_f() << Qt::endl;
    //qDebug() << "msgTmp.text() " << msgTmp->text_f().toRawReplace_f() << Qt::endl;
    //qDebug() << "type_par_con " << executionMessage_c::executionMessageTypeToString_f(type_par_con) << Qt::endl;
    //qDebug() << "msgTmp->type_f() " << executionMessage_c::executionMessageTypeToString_f(msgTmp->type_f()) << Qt::endl;
#endif
    Q_EMIT addExecutionMessage_signal(msgTmp);
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
    Q_EMIT executionStateChange_signal(actionExecutionState_ec::executing);
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
