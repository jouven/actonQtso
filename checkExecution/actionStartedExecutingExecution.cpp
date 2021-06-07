#include "actionStartedExecutingExecution.hpp"

#include "../checks/actionStartedExecuting.hpp"
#include "../actonDataHub.hpp"
#include "../actionDataExecutionResult.hpp"


actionStartedExecutingCheckExecution_c::actionStartedExecutingCheckExecution_c(
        checkExecutionResult_c* checkExecutionResultObj_par_con
        , actionStartedExecutingCheck_c* actionStartedExecutingCheckPtr_par)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
    , actionStartedExecutingCheckPtr_pri(actionStartedExecutingCheckPtr_par)
{
}

void actionStartedExecutingCheckExecution_c::finishCheck_f()
{
    Q_EMIT anyFinish_signal(true);
}

void actionStartedExecutingCheckExecution_c::actionStarted_f(executionResult_c* const executionResult_par_ptr_con)
{
    bool finishedCheckTmp(false);
    if (actionStartedExecutingCheckPtr_pri->waitFinishMs_f() > 0)
    {
        actionExecutionResult_c* actionExecutionResultPtrTmp(static_cast<actionExecutionResult_c*>(executionResult_par_ptr_con));
        auto currentTimeTmp(QDateTime::currentMSecsSinceEpoch());
        auto waitTimeTmp((actionExecutionResultPtrTmp->startTime_f() + actionStartedExecutingCheckPtr_pri->waitFinishMs_f()) - currentTimeTmp);
        if (waitTimeTmp > 0)
        {
            QTimer::singleShot(waitTimeTmp, this, &actionStartedExecutingCheckExecution_c::finishCheck_f);
        }
        else
        {
            finishedCheckTmp = true;
        }
    }
    else
    {
        finishedCheckTmp = true;
    }
    if (finishedCheckTmp)
    {
        finishCheck_f();
    }
}

void actionStartedExecutingCheckExecution_c::derivedExecute_f()
{
    while (true)
    {
        if (not actionStartedExecutingCheckPtr_pri->parentAction_f()->parentIsActonDataHubObj_f())
        {
            emitExecutionMessage_f(
            {
                            "actionStartedExecuting check with description {0} has no actonDataHub parent object"
                            , actionStartedExecutingCheckPtr_pri->description_f()
            }, executionMessage_c::type_ec::error);
            break;
        }

        QString actionStringIdTmp(actionStartedExecutingCheckPtr_pri->actionStringId_f());
        int_fast64_t actionDataIdTmp(actionStartedExecutingCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->actionDataStringIdToActionDataId_f(actionStringIdTmp));
        if (actionDataIdTmp <= 0)
        {
            emitExecutionMessage_f({"No actionDataId could be retrieved from the actionDataStringId: {0}", actionStringIdTmp}, executionMessage_c::type_ec::error);
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            //anyErrorTmp = true;
            break;
        }
        action_c* actionPtrTmp(actionStartedExecutingCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->action_ptr_f(actionDataIdTmp));
        if (actionPtrTmp == nullptr)
        {
            emitExecutionMessage_f({"No action object could be retrieved from the actionDataId: {0}", actionDataIdTmp}, executionMessage_c::type_ec::error);
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            //anyErrorTmp = true;
            break;
        }

        auto actionResultObjPtrTmp(actionPtrTmp->createGetActionDataExecutionResult_ptr_f());
        if (actionResultObjPtrTmp not_eq nullptr)
        {
            //actionStartedExecuting is one of the check execution classes exceptions that lives in the main thread
            //and all the action result objs live on the main thread, there will be no races,
            //so it can be checked first if the action obj result exists and is finished
            //and if that isn't the case, connect to know when that happens
            if (actionResultObjPtrTmp->started_f())
            {
#ifdef DEBUGJOUVEN
                //qDebug() << "actionResultObjPtrTmp->finished_f()" << Qt::endl;
#endif
                actionStarted_f(actionResultObjPtrTmp);
            }
            else
            {
                QObject::connect(actionResultObjPtrTmp, &actionExecutionResult_c::started_signal, this, &actionStartedExecutingCheckExecution_c::actionStarted_f);
            }
        }
        else
        {
            emitExecutionMessage_f({"Could not get the action result object (action not enabled?), the actionDataStringId: {0}", actionStringIdTmp}, executionMessage_c::type_ec::error);
            break;
        }

        break;
    }
    if (executionError_f())
    {
        Q_EMIT anyFinish_signal(false);
    }
}

void actionStartedExecutingCheckExecution_c::derivedStop_f()
{
    Q_EMIT anyFinish_signal(false);
}


