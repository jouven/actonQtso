#include "actionStartedExecutingExecution.hpp"

#include "../checks/actionStartedExecuting.hpp"
#include "../actonDataHub.hpp"
#include "../actionDataExecutionResult.hpp"


actionStartedExecutingCheckExecution_c::actionStartedExecutingCheckExecution_c(
        checkDataExecutionResult_c* checkExecutionResultObj_par_con
        , actionStartedExecutingCheck_c* actionStartedExecutingCheckPtr_par)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
    , actionStartedExecutingCheckPtr_pri(actionStartedExecutingCheckPtr_par)
{
}

void actionStartedExecutingCheckExecution_c::finishCheck_f()
{
    Q_EMIT anyFinish_signal(true);
}

void actionStartedExecutingCheckExecution_c::actionStarted_f(action_c* const action_par_ptr_con)
{
    bool finishedCheckTmp(false);
    if (actionStartedExecutingCheckPtr_pri->waitFinishMs_f() > 0)
    {
        actionDataExecutionResult_c* actionResultObjPtrTmp(action_par_ptr_con->createGetActionDataExecutionResult_ptr_f());
        auto currentTimeTmp(QDateTime::currentMSecsSinceEpoch());
        auto waitTimeTmp((actionResultObjPtrTmp->startTime_f() + actionStartedExecutingCheckPtr_pri->waitFinishMs_f()) - currentTimeTmp);
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
        QString actionStringIdTmp(actionStartedExecutingCheckPtr_pri->actionStringId_f());
        int_fast64_t actionDataIdTmp(actonDataHub_ptr_ext->actionDataStringIdToActionDataId_f(actionStringIdTmp));
        if (actionDataIdTmp <= 0)
        {
            Q_EMIT addError_signal({"No actionDataId could be retrieved from the actionDataStringId: {0}", actionStringIdTmp});
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            //anyErrorTmp = true;
            break;
        }
        action_c* actionPtrTmp(actonDataHub_ptr_ext->action_ptr_f(actionDataIdTmp));
        if (actionPtrTmp == nullptr)
        {
            Q_EMIT addError_signal({"No action object could be retrieved from the actionDataId: {0}", actionDataIdTmp});
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
                //qDebug() << "actionResultObjPtrTmp->finished_f()" << endl;
#endif
                actionStarted_f(actionPtrTmp);
            }
            else
            {
                QObject::connect(actionResultObjPtrTmp, &actionDataExecutionResult_c::started_signal, this, &actionStartedExecutingCheckExecution_c::actionStarted_f);
            }
        }
        else
        {
            Q_EMIT addError_signal({"Could not get the action result object (action not enabled?), the actionDataStringId: {0}", actionStringIdTmp});
            break;
        }

        break;
    }
}

void actionStartedExecutingCheckExecution_c::derivedStop_f()
{
    Q_EMIT anyFinish_signal(false);
}


