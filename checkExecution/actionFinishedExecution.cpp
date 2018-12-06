#include "actionFinishedExecution.hpp"

#include "../actonDataHub.hpp"
#include "../actionDataExecutionResult.hpp"


actionFinishedCheckExecution_c::actionFinishedCheckExecution_c(
        const actionFinishedCheck_c& actionFinishedCheck_par_con)
    : actionFinishedCheck_c(actionFinishedCheck_par_con)
{
    //can be deleted at any time no problem
    QObject::connect(this, &baseCheckExecution_c::executionStateChange_signal, this, &actionFinishedCheckExecution_c::verifyStopped_f);
}

void actionFinishedCheckExecution_c::execute_f()
{
    bool anyErrorTmp(false);
    Q_EMIT executionStateChange_signal(checkExecutionState_ec::preparing);
    while (true)
    {
        int_fast64_t actionDataIdTmp(actonDataHub_ptr_ext->actionDataStringIdToActionDataId_f(actionStringId_f()));
        if (actionDataIdTmp <= 0)
        {
            Q_EMIT addError_signal("No actionDataId could be retrieved from the actionDataStringId: " + actionStringId_f());
            Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            anyErrorTmp = true;
            break;
        }
        actionData_c* actionDataPtrTmp(actonDataHub_ptr_ext->actionData_ptr_f(actionDataIdTmp));
        if (actionDataPtrTmp == nullptr)
        {
            Q_EMIT addError_signal("No actionData object could be retrieved from the actionDataId: " + QString::number(actionDataIdTmp));
            Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            anyErrorTmp = true;
            break;
        }

        QObject::connect(actionDataPtrTmp->createGetActionDataExecutionResult_ptr_f(), &actionDataExecutionResult_c::finished_signal, this, &actionFinishedCheckExecution_c::actionFinished_f);

        Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);

        break;
    }
    if (anyErrorTmp)
    {
        Q_EMIT anyCheckResult_signal(false);
        Q_EMIT anyCheckFinish_signal();
    }
}

void actionFinishedCheckExecution_c::actionFinished_f(actionData_c* const actionData_par_ptr_con)
{
    Q_EMIT executionStateChange_signal(checkExecutionState_ec::finished);
    if (failCheckOnNotSuccessfulActionFinish_f())
    {
        if (actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->state_f() == actionExecutionState_ec::success)
        {
            Q_EMIT anyCheckResult_signal(true);
        }
        else
        {
            Q_EMIT anyCheckResult_signal(false);
        }
    }
    else
    {
        Q_EMIT anyCheckResult_signal(true);
    }
    Q_EMIT anyCheckFinish_signal();
}

//could have added the argument (stopped state) but since there is also the stop property...
void actionFinishedCheckExecution_c::verifyStopped_f()
{
    if (stopping_f())
    {
        Q_EMIT anyCheckResult_signal(false);
        Q_EMIT anyCheckFinish_signal();
    }
}


