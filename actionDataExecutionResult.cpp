#include "actionDataExecutionResult.hpp"

#include "actionData.hpp"
#include "actonDataHub.hpp"
#include "actionMappings/actionExecutionStateStrMapping.hpp"

#include "comuso/practicalTemplates.hpp"

#define MACRO_ADDLOG(...) \
if (action_ptr_pri->actonDataHubParent_f() not_eq nullptr) \
{ \
    MACRO_ADDACTONDATAHUBLOG(action_ptr_pri->actonDataHubParent_f(),__VA_ARGS__); \
}

int actionExecutionResult_c::returnCode_f() const
{
    return returnCode_pri;
}

actionExecutionState_ec actionExecutionResult_c::lastState_f() const
{
    return executionStateVector_pri.back();
}

bool actionExecutionResult_c::returnCodeSet_f() const
{
    return returnCodeSet_pri;
}

std::vector<actionExecutionState_ec> actionExecutionResult_c::executionStateVector_f() const
{
    return executionStateVector_pri;
}

action_c* actionExecutionResult_c::action_ptr_f() const
{
    return action_ptr_pri;
}

//void actionExecutionResult_c::derivedTryClear_f()
//{
//    //clear only the pri variables since the pro one are cleared with tryClear_f
//    returnCode_pri = -1;
//    returnCodeSet_pri = false;

//    executionStateVector_pri = { actionExecutionState_ec::initial };

//    killedByUser_pri = false;

//    Q_EMIT resultsCleared_signal(action_ptr_pri);
//}

actionExecutionResult_c::actionExecutionResult_c(
        action_c* action_par_ptr_con
        , QObject* parent_par)
    : executionResult_c(parent_par)
    , action_ptr_pri(action_par_ptr_con)
{}

void actionExecutionResult_c::derivedAppendMessage_f(executionMessage_c* message_par_con)
{
    //qDebug() << "message_par_con " << message_par_con->text_f().toRawReplace_f() << Qt::endl;
    while (not finished_pro)
    {
        if (message_par_con->type_f() == executionMessage_c::type_ec::error)
        {
            for (size_t i = 0, l = message_par_con->text_f().size_f(); i < l; ++i)
            {
                MACRO_ADDLOG(message_par_con->text_f().text_f(i), action_ptr_pri, messageType_ec::warning);
            }
            trySetExecutionState_f(actionExecutionState_ec::error);
            break;
        }
        if (message_par_con->type_f() == executionMessage_c::type_ec::externalsdout)
        {
            Q_EMIT externalStdoutAdded_signal(action_ptr_pri, message_par_con);
            break;
        }
        if (message_par_con->type_f() == executionMessage_c::type_ec::externalstderr)
        {
            Q_EMIT externalStdErrAdded_signal(action_ptr_pri, message_par_con);
            break;
        }
        break;
    }
}

void actionExecutionResult_c::setReturnCode_f(const int returnCode_par_con)
{
    if (not finished_pro and not returnCodeSet_pri)
    {
        returnCode_pri = returnCode_par_con;
        returnCodeSet_pri = true;
        Q_EMIT returnCodeSet_signal(action_ptr_pri);
        Q_EMIT anyChange_signal(this);
    }
}

bool actionExecutionResult_c::trySetExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con)
{
#ifdef DEBUGJOUVEN
    //qDebug() << "current stateString_f() " << stateString_f() << Qt::endl;
    //qDebug() << "new stateString_f() " << actionExecutionStateToString_f(actionExecutionState_par_con) << Qt::endl;
#endif
    bool validExecutionStateChangeTmp(false);
    bool emitPreparingTmp(false);
    bool emitExecutingChecksTmp(false);
    bool emitExecutingTmp(false);
    bool emitStoppingTmp(false);
    bool emitKillingTmp(false);
    //verify if is on a "final" state
    while (not finished_pro and (lastState_f() not_eq actionExecutionState_par_con))
    {
        //states that can't be set outside of this class
        if (equalOnce_ft(actionExecutionState_par_con
                         , actionExecutionState_ec::initial
                         , actionExecutionState_ec::stopped
                         , actionExecutionState_ec::killed)
        )
        {
            break;
        }

        //from initial it can only go to preparing, executing checks
        if (lastState_f() == actionExecutionState_ec::initial
            and equalOnce_ft(actionExecutionState_par_con
                              , actionExecutionState_ec::preparing
                              , actionExecutionState_ec::executingChecks)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::preparing)
            {
                emitPreparingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::executingChecks)
            {
                emitExecutingChecksTmp = true;
                break;
            }
        }

        //from executhing checks it can only go to preparing, stopping/killing and checks failed
        if (lastState_f() == actionExecutionState_ec::executingChecks
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::preparing
                             , actionExecutionState_ec::stopping
                             , actionExecutionState_ec::killing
                             , actionExecutionState_ec::checksFailed)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::preparing)
            {
                emitPreparingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::stopping
                or actionExecutionState_par_con == actionExecutionState_ec::killing)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from preparing it can only go to error, executing, stopping/killing and success.
        //Allowing success has to do with some actions checking if what the action is going to do has already happened
        //and usually there is a setting that treats that as an error or success
        //skipping the running part
        if (lastState_f() == actionExecutionState_ec::preparing
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::executing
                             , actionExecutionState_ec::stopping
                             , actionExecutionState_ec::killing
                             , actionExecutionState_ec::success)
            )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::executing)
            {
                emitExecutingTmp = true;
                break;
            }
            if (actionExecutionState_par_con == actionExecutionState_ec::stopping
                or actionExecutionState_par_con == actionExecutionState_ec::killing)
            {
                emitStoppingTmp = true;
                break;
            }
            break;
        }

        //from executing it can only go to error, success, stopping
        if (lastState_f() == actionExecutionState_ec::executing
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::stopping
                             , actionExecutionState_ec::success)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::stopping)
            {
                emitStoppingTmp = true;
            }
            break;
        }

        //from stoppingByUser it can only go to error or killing
        if (lastState_f() == actionExecutionState_ec::stopping
            and equalOnce_ft(actionExecutionState_par_con
                             , actionExecutionState_ec::error
                             , actionExecutionState_ec::killing)
        )
        {
            validExecutionStateChangeTmp = true;
            if (actionExecutionState_par_con == actionExecutionState_ec::killing)
            {
                emitKillingTmp = true;
            }
            break;
        }

        break;
    }
    while (validExecutionStateChangeTmp)
    {
        executionStateVector_pri.emplace_back(actionExecutionState_par_con);
        //setFinished removed from here because an action might emit a finishing state like error
        //but still need to end its functions calls. The finished signal is connected
        //to deletion of the action execution but then again the action execution object is in another
        //thread so the deletion should wait until the thread is back to its execution loop.
        //Still more stuff that can be signaled from the execution object might happen, more errors or info,
        //and finishing prevents any modification of the results
        Q_EMIT executionStateUpdated_signal(action_ptr_pri, actionExecutionState_par_con);
        if (emitPreparingTmp)
        {
            Q_EMIT preparing_signal(this);
            break;
        }
        if (emitExecutingChecksTmp)
        {
            Q_EMIT executingChecks_signal(action_ptr_pri);
            break;
        }
        if (emitExecutingTmp)
        {
            setStarted_f();
            Q_EMIT executing_signal(this);
            break;
        }
        if (emitStoppingTmp)
        {
            Q_EMIT stopping_signal(this);
            break;
        }
        if (emitKillingTmp)
        {
            Q_EMIT killing_signal(action_ptr_pri);
            break;
        }
        break;
    }
    if (validExecutionStateChangeTmp)
    {
        Q_EMIT anyChange_signal(this);
    }
    return validExecutionStateChangeTmp;
}

bool actionExecutionResult_c::killedByUser_f() const
{
    return killedByUser_pri;
}

//void actionExecutionResult_c::derivedSetStarted_f()
//{
//    Q_EMIT started_signal(this);
//}

executionResult_c::type_ec actionExecutionResult_c::type_f() const
{
    return executionResult_c::type_ec::action;
}

QString actionExecutionResult_c::stateString_f() const
{
    return actionExecutionStateToString_f(lastState_f());
}

QString actionExecutionResult_c::derivedElementTypeString_f() const
{
    return action_ptr_pri->typeStr_f();
}

QString actionExecutionResult_c::derivedElementDescription_f() const
{
    return action_ptr_pri->description_f();
}

bool actionExecutionResult_c::derivedTrySetFinished_f()
{
    bool resultTmp(false);
#ifdef DEBUGJOUVEN
    //qDebug() << "derivedTrySetFinished_f stateString_f() " << stateString_f() << Qt::endl;
#endif
    if (equalOnce_ft(lastState_f()
                         , actionExecutionState_ec::executing
                         //error is special because it's a final state already
                         , actionExecutionState_ec::error
                         , actionExecutionState_ec::killing
                         , actionExecutionState_ec::stopping
                         //executing checks finishing means the check/s logic result is false
                         //and the action won't be executed after
                         , actionExecutionState_ec::executingChecks
                         )
    )
    {
        if (lastState_f() == actionExecutionState_ec::error)
        {
            resultTmp = true;
        }
        if (equalOnce_ft(lastState_f()
                         , actionExecutionState_ec::executing
                         , actionExecutionState_ec::killing
                         , actionExecutionState_ec::stopping
                         , actionExecutionState_ec::executingChecks
                         ))
        {
            actionExecutionState_ec finishedStateTmp;
            if (lastState_f() == actionExecutionState_ec::executing)
            {
                finishedStateTmp = actionExecutionState_ec::success;
            }
            if (lastState_f() == actionExecutionState_ec::stopping)
            {
                stoppedByUser_pro = true;
                finishedStateTmp = actionExecutionState_ec::stopped;
            }
            if (lastState_f() == actionExecutionState_ec::killing)
            {
                killedByUser_pri = true;
                finishedStateTmp = actionExecutionState_ec::killed;
            }
            if (lastState_f() == actionExecutionState_ec::executingChecks)
            {
                finishedStateTmp = actionExecutionState_ec::checksFailed;
            }
            resultTmp = true;
            executionStateVector_pri.emplace_back(finishedStateTmp);
            Q_EMIT executionStateUpdated_signal(action_ptr_pri, finishedStateTmp);
            if (lastState_f() == actionExecutionState_ec::success)
            {
                Q_EMIT success_signal(action_ptr_pri);
            }
            if (lastState_f() == actionExecutionState_ec::stopped)
            {
                Q_EMIT stopped_signal(this);
            }
            if (lastState_f() == actionExecutionState_ec::killed)
            {
                Q_EMIT killed_signal(action_ptr_pri);
            }
        }
    }
    if (not resultTmp)
    {
        MACRO_ADDLOG(
                    {"Error finishing action execution couldn't finish from {0} state"
                     , stateString_f() }
                    , messageType_ec::error);
    }
    return resultTmp;
}
