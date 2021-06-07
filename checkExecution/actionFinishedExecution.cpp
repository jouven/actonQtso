#include "actionFinishedExecution.hpp"

#include "../checks/actionFinished.hpp"
#include "../actonDataHub.hpp"
#include "../actionDataExecutionResult.hpp"
#include "../checkDataExecutionResult.hpp"
#include "../actionMappings/actionExecutionStateStrMapping.hpp"

#include "stringParserMapQtso/parsers/stringReplacer.hpp"
#include "stringParserMapQtso/stringParserMap.hpp"

#include "textQtso/text.hpp"

#include "essentialQtso/macros.hpp"

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

//this check can't work in the void, theoretically it should always be a child of an action and the action of a actonDataHub
#define MACRO_ADDLOG(...) \
if (actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f() not_eq nullptr) \
{ \
    MACRO_ADDACTONDATAHUBLOG(actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f(),__VA_ARGS__); \
}

void actionFinishedCheckExecution_c::derivedStop_f()
{
    Q_EMIT anyFinish_signal(false);
}

actionFinishedCheckExecution_c::actionFinishedCheckExecution_c(
        checkExecutionResult_c* checkExecutionResultObj_par_con
        , actionFinishedCheck_c* actionFinishedCheckPtr_par)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
      , actionFinishedCheckPtr_pri(actionFinishedCheckPtr_par)
{}

void actionFinishedCheckExecution_c::derivedExecute_f()
{
    //bool anyErrorTmp(false);
    while (true)
    {
        if (not actionFinishedCheckPtr_pri->parentAction_f()->parentIsActonDataHubObj_f())
        {
            emitExecutionMessage_f(
            {
                            "actionFinished check with description {0} has no actonDataHub parent object"
                            , actionFinishedCheckPtr_pri->description_f()
            }, executionMessage_c::type_ec::error);
            break;
        }

        QString actionStringIdTmp(actionFinishedCheckPtr_pri->actionStringId_f());
        int_fast64_t actionDataIdTmp(actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->actionDataStringIdToActionDataId_f(actionStringIdTmp));
        if (actionDataIdTmp <= 0)
        {
            emitExecutionMessage_f({"No actionDataId could be retrieved from the actionDataStringId: {0}", actionStringIdTmp}, executionMessage_c::type_ec::error);
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            //anyErrorTmp = true;
            break;
        }
        action_c* actionPtrTmp(actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->action_ptr_f(actionDataIdTmp));
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
            //actionFinishedExecution is one of the check execution classes exceptions that lives in the main thread
            //and all the action result objs live on the main thread, there will be no races,
            //so it can be checked first if the action obj result exists and is finished
            //and if that isn't the case, connect to know when that happens
            if (actionResultObjPtrTmp->finished_f())
            {
#ifdef DEBUGJOUVEN
                //qDebug() << "actionResultObjPtrTmp->finished_f()" << Qt::endl;
#endif
                actionFinished_f(actionResultObjPtrTmp);
            }
            else
            {
                QObject::connect(actionResultObjPtrTmp, &actionExecutionResult_c::finished_signal, this, &actionFinishedCheckExecution_c::actionFinished_f);
            }
        }
        else
        {
            emitExecutionMessage_f({"Could not get the action result object (action not enabled?), the actionDataStringId: {0}", actionStringIdTmp}, executionMessage_c::type_ec::error);
            break;
        }

        //Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
        break;
    }
    if (executionError_f())
    {
        Q_EMIT anyFinish_signal(false);
    }
}

void actionFinishedCheckExecution_c::actionFinished_f(executionResult_c* const executionResult_par_ptr_con)
{
    while (true)
    {
        actionExecutionResult_c* actionExecutionResultPtrTmp(static_cast<actionExecutionResult_c*>(executionResult_par_ptr_con));
        bool finishComputedResultTmp(actionFinishedCheckPtr_pri->successOnActionSuccess_f() ?
                    actionExecutionResultPtrTmp->lastState_f() == actionExecutionState_ec::success
                  : true);
        if (finishComputedResultTmp)
        {
            finishedCount_pri = finishedCount_pri + 1;
        }
        text_c textTmp("(Check) Action stringId \"{0}\" finished result: {1}, count: {2}"
                       , actionExecutionResultPtrTmp->action_ptr_f()->stringId_f()
                       , QSTRINGBOOL(finishComputedResultTmp)
                       , finishedCount_pri);
        MACRO_ADDLOG(textTmp, actionFinishedCheckPtr_pri, messageType_ec::information);
        if (finishedCount_pri >= actionFinishedCheckPtr_pri->finishedCount_f())
        {
            //finish
        }
        else
        {
            //don't finish the check yet, keep counting action finishings
            break;
        }
        Q_EMIT anyFinish_signal(finishComputedResultTmp);

        if (actionFinishedCheckPtr_pri->actionExecutionResultFieldToStringTrigger_f().empty())
        {
            //no action execution results to string trigger configurations
        }
        else
        {
            for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& pair_ite_con : actionFinishedCheckPtr_pri->actionExecutionResultFieldToStringTrigger_f())
            {
                QString valueTmp;
                while (true)
                {
                    //the "non-external" action result fields can-could be translated (outside this lib if they are passed as text_c/textCompilation_c)
                    //the issues are:
                    //1 the stringParser lib should not and can't deal with text_c/textCompilaciton_c
                    //stringParser would need to add its own placeholders to allow future replacing the "text-values" from here
                    //2 because following this path leads to layers of replacing, like a matroska
                    //3 I don't like recursion that would be required to deal with this situation
                    //it's probably easier to brute force
                    //4 important other actions might never need and probably would prefer not to care
                    //if the string trigger replaced value is translated or not
                    //so ...use the rawReplaced, hardcoded is more generic that the different possible translations
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::errorStr)
                    {
                        valueTmp = actionExecutionResultPtrTmp->messagesStr_f({executionMessage_c::type_ec::error});
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::outputStr)
                    {
                        valueTmp = actionExecutionResultPtrTmp->messagesStr_f({executionMessage_c::type_ec::information});
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::externalErrorStr)
                    {
                        valueTmp = actionExecutionResultPtrTmp->messagesStr_f({executionMessage_c::type_ec::externalstderr});
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::externalOutputStr)
                    {
                        valueTmp = actionExecutionResultPtrTmp->messagesStr_f({executionMessage_c::type_ec::externalsdout});
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::endState)
                    {
                        valueTmp = actionExecutionStateToString_f(actionExecutionResultPtrTmp->lastState_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::returnCode)
                    {
                        valueTmp = QString::number(actionExecutionResultPtrTmp->returnCode_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::startDateTime)
                    {
                        valueTmp = QString::number(actionExecutionResultPtrTmp->startTime_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::endDateTime)
                    {
                        valueTmp = QString::number(actionExecutionResultPtrTmp->finishedTime_f());
                        break;
                    }

                    break;
                }
                //the parsers created here during execution are removed when the execution stops (successfully or not)
                stringReplacer_c* stringReplacerPtrTmp(new stringReplacer_c(pair_ite_con.second, stringReplacer_c::replaceType_ec::string, valueTmp));
                actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->executionOptions_f().stringParserMap_f()->addParser_f(stringReplacerPtrTmp);
                if (actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->executionOptions_f().stringParserMap_f()->anyError_f())
                {
                    for (int_fast32_t i = 0, l = actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->executionOptions_f().stringParserMap_f()->getErrors_f().size_f(); i < l; ++i)
                    {
                        emitExecutionMessage_f({actionFinishedCheckPtr_pri->parentAction_f()->actonDataHubParent_f()->executionOptions_f().stringParserMap_f()->getErrors_f().text_f(i)}, executionMessage_c::type_ec::error);
                    }
                    break;
                }
            }
        }
        break;
    }
}

