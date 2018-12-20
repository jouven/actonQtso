#include "actionFinishedExecution.hpp"

#include "../actonDataHub.hpp"
#include "../actionDataExecutionResult.hpp"


void actionFinishedCheckExecution_c::derivedStop_f()
{
    Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedFalse);
}

actionFinishedCheckExecution_c::actionFinishedCheckExecution_c(
        checkDataExecutionResult_c* checkExecutionResultObj_par_con
        , const actionFinishedCheck_c& actionFinishedCheck_par_con)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
      , actionFinishedCheck_c(actionFinishedCheck_par_con)
{}

void actionFinishedCheckExecution_c::derivedExecute_f()
{
    //bool anyErrorTmp(false);
    while (true)
    {
        int_fast64_t actionDataIdTmp(actonDataHub_ptr_ext->actionDataStringIdToActionDataId_f(actionStringId_f()));
        if (actionDataIdTmp <= 0)
        {
            Q_EMIT addError_signal("No actionDataId could be retrieved from the actionDataStringId: " + actionStringId_f());
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            //anyErrorTmp = true;
            break;
        }
        actionData_c* actionDataPtrTmp(actonDataHub_ptr_ext->actionData_ptr_f(actionDataIdTmp));
        if (actionDataPtrTmp == nullptr)
        {
            Q_EMIT addError_signal("No actionData object could be retrieved from the actionDataId: " + QString::number(actionDataIdTmp));
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            //anyErrorTmp = true;
            break;
        }

        QObject::connect(actionDataPtrTmp->createGetActionDataExecutionResult_ptr_f(), &actionDataExecutionResult_c::finished_signal, this, &actionFinishedCheckExecution_c::actionFinished_f);

        Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
        break;
    }
//    if (anyErrorTmp)
//    {
//        Q_EMIT finishedFalse_signal();
//    }
}

void actionFinishedCheckExecution_c::actionFinished_f(actionData_c* const actionData_par_ptr_con)
{
    if (failCheckOnNotSuccessfulActionFinish_f())
    {
        if (actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::success)
        {
            Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedTrue);
        }
        else
        {
            Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedFalse);
        }
    }
    else
    {
        Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedTrue);
    }
}

