#include "actionFinishedExecution.hpp"

#include "../actonDataHub.hpp"
#include "../actionDataExecutionResult.hpp"
#include "../checkDataExecutionResult.hpp"
#include "../actionMappings/actionExecutionStateStrMapping.hpp"

#include "stringParserMapQtso/parsers/stringReplacer.hpp"
#include "stringParserMapQtso/stringParserMap.hpp"

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
        QString actionStringIdTmp(actionStringIdParsed_f());
        int_fast64_t actionDataIdTmp(actonDataHub_ptr_ext->actionDataStringIdToActionDataId_f(actionStringIdTmp));
        if (actionDataIdTmp <= 0)
        {
            Q_EMIT addError_signal("No actionDataId could be retrieved from the actionDataStringId: " + actionStringIdTmp);
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
    if (actionExecutionResultFieldToStringTrigger_f().empty())
    {
        //nothing
    }
    else
    {
        for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& pair_ite_con : actionExecutionResultFieldToStringTrigger_f())
        {
            QString valueTmp;
            while (true)
            {
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::errorStr)
                {
                    valueTmp = actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->error_f();
                    break;
                }
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::outputStr)
                {
                    valueTmp = actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->output_f();
                    break;
                }
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::externalErrorStr)
                {
                    valueTmp = actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->externalErrorOutput_f();
                    break;
                }
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::externalOutputStr)
                {
                    valueTmp = actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->externalOutput_f();
                    break;
                }
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::endState)
                {
                    valueTmp = actionExecutionStateToStrUMap_ext_con.at(actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->lastState_f());
                    break;
                }
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::returnCode)
                {
                    valueTmp = QString::number(actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->returnCode_f());
                    break;
                }
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::startDateTime)
                {
                    valueTmp = QString::number(actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->startTime_f());
                    break;
                }
                if (pair_ite_con.first == actionFinishedCheck_c::actionExecutionResultFields_ec::endDateTime)
                {
                    valueTmp = QString::number(actionData_par_ptr_con->actionDataExecutionResult_ptr_f()->finishedTime_f());
                    break;
                }

                break;
            }
            //the parsers created here during execution are removed when the execution stops (successfully or not)
            stringReplacer_c* stringReplacerPtrTmp(new stringReplacer_c(pair_ite_con.second, stringReplacer_c::replaceType_ec::string, valueTmp));
            actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->addParser_f(stringReplacerPtrTmp);
            if (actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->anyError_f())
            {
                Q_EMIT addError_signal(actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->getError_f());
                break;
            }
        }
    }
}

