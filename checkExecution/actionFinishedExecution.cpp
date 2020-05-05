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

void actionFinishedCheckExecution_c::derivedStop_f()
{
    Q_EMIT anyFinish_signal(false);
}

actionFinishedCheckExecution_c::actionFinishedCheckExecution_c(
        checkDataExecutionResult_c* checkExecutionResultObj_par_con
        , actionFinishedCheck_c* actionFinishedCheckPtr_par)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
      , actionFinishedCheckPtr_pri(actionFinishedCheckPtr_par)
{}

void actionFinishedCheckExecution_c::derivedExecute_f()
{
    //bool anyErrorTmp(false);
    while (true)
    {
        QString actionStringIdTmp(actionFinishedCheckPtr_pri->actionStringId_f());
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
            //actionFinishedExecution is one of the check execution classes exceptions that lives in the main thread
            //and all the action result objs live on the main thread, there will be no races,
            //so it can be checked first if the action obj result exists and is finished
            //and if that isn't the case, connect to know when that happens
            if (actionResultObjPtrTmp->finished_f())
            {
#ifdef DEBUGJOUVEN
                //qDebug() << "actionResultObjPtrTmp->finished_f()" << endl;
#endif
                actionFinished_f(actionPtrTmp);
            }
            else
            {
                QObject::connect(actionResultObjPtrTmp, &actionDataExecutionResult_c::finished_signal, this, &actionFinishedCheckExecution_c::actionFinished_f);
            }
        }
        else
        {
            Q_EMIT addError_signal({"Could not get the action result object (action not enabled?), the actionDataStringId: {0}", actionStringIdTmp});
            break;
        }

        //Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
        break;
    }
//    if (anyErrorTmp)
//    {
//        Q_EMIT finishedFalse_signal();
//    }
}

void actionFinishedCheckExecution_c::actionFinished_f(action_c* const action_par_ptr_con)
{
    while (true)
    {
        bool finishComputedResultTmp(actionFinishedCheckPtr_pri->successOnActionSuccess_f() ?
                    action_par_ptr_con->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::success
                  : true);
        if (finishComputedResultTmp)
        {
            finishedCount_pri = finishedCount_pri + 1;
        }
        text_c textTmp("(Check) Action stringId \"{0}\" finished result: {1}, count: {2}"
                       , action_par_ptr_con->stringId_f()
                       , QSTRINGBOOL(finishComputedResultTmp)
                       , finishedCount_pri);
        MACRO_ADDACTONQTSOLOG(textTmp, actionFinishedCheckPtr_pri, logItem_c::type_ec::info);
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
                        valueTmp.append(action_par_ptr_con->actionDataExecutionResult_ptr_f()->errors_f().toRawReplace_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::outputStr)
                    {
                        valueTmp.append(action_par_ptr_con->actionDataExecutionResult_ptr_f()->output_f().toRawReplace_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::externalErrorStr)
                    {
                        valueTmp = action_par_ptr_con->actionDataExecutionResult_ptr_f()->externalErrorOutput_f();
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::externalOutputStr)
                    {
                        valueTmp = action_par_ptr_con->actionDataExecutionResult_ptr_f()->externalOutput_f();
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::endState)
                    {
                        valueTmp = actionExecutionStateToStrUMap_ext_con.at(action_par_ptr_con->actionDataExecutionResult_ptr_f()->lastState_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::returnCode)
                    {
                        valueTmp = QString::number(action_par_ptr_con->actionDataExecutionResult_ptr_f()->returnCode_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::startDateTime)
                    {
                        valueTmp = QString::number(action_par_ptr_con->actionDataExecutionResult_ptr_f()->startTime_f());
                        break;
                    }
                    if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::endDateTime)
                    {
                        valueTmp = QString::number(action_par_ptr_con->actionDataExecutionResult_ptr_f()->finishedTime_f());
                        break;
                    }

                    break;
                }
                //the parsers created here during execution are removed when the execution stops (successfully or not)
                stringReplacer_c* stringReplacerPtrTmp(new stringReplacer_c(pair_ite_con.second, stringReplacer_c::replaceType_ec::string, valueTmp));
                actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->addParser_f(stringReplacerPtrTmp);
                if (actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->anyError_f())
                {
                    for (int_fast32_t i = 0, l = actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->getErrors_f().size_f(); i < l; ++i)
                    {
                        Q_EMIT addError_signal(actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->getErrors_f().text_f(i));
                    }
                    break;
                }
            }
        }
        break;
    }
}

