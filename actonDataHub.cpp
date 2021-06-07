#include "actonDataHub.hpp"

#include "essentialQtso/macros.hpp"

#include "checkData.hpp"
#include "checks/actionFinished.hpp"
#include "checks/actionStartedExecuting.hpp"
#include "checkMappings/checkExecutionStates.hpp"
#include "actionDataExecutionResult.hpp"
#include "actions/folderChangeReaction.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "essentialQtso/macros.hpp"
#include "textQtso/text.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QCoreApplication>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

#include <execution>

//don't use in actonDataHub_c ctor/s
#define MACRO_ADDLOG(...) addLogMessage_f(__VA_ARGS__, MACRO_FILEMETA)

bool actonDataHub_c::moveRowActionData_f(
    const int sourceRow_par_con
    , const int destinationRow_par_con)
{
    bool resultTmp(false);
    int_fast64_t actionDataIdTmp(rowToActionDataId_f(sourceRow_par_con));
    action_c* actionPtrTmp(nullptr);
    //qInfo() << "actionDataIdTmp " << actionDataIdTmp << Qt::endl;
    while (actionDataIdTmp > 0)
    {
        if (executingActions_pri)
        {
            break;
        }
        //save the actionData ptr that is going to be moved
        actionPtrTmp = action_ptr_f(actionDataIdTmp);

        //qInfo() << "sourceActionDataTmp.id_f() " << sourceActionDataTmp.id_f() << Qt::endl;

        //remove it, this will always work here otherwise it wouldn't fetch the actionDataId to begin with
        bool removeResult(removeActionDataUsingRow_f(sourceRow_par_con, actionDataIdTmp));

        if (removeResult)
        {
            //insert it in the destination row, this can fail if the destination row is not "in range"
            resultTmp = insertActionData_f(actionPtrTmp, destinationRow_par_con);
        }
        else
        {
            //otherwise there would be duplicates
        }
        break;
    }
    text_c logTextTmp("Action moved from row {0} to row {1} result: {2}", sourceRow_par_con, destinationRow_par_con, QSTRINGBOOL(resultTmp));
    MACRO_ADDLOG(logTextTmp, actionPtrTmp, messageType_ec::debug);
    return resultTmp;
}

////for the drag and drop
//void actionDataController_c::moveRowActionData(
//    const int sourceRow_par_con
//    , const int destinationRow_par_con)
//{
//    if (sourceRow_par_con == destinationRow_par_con or sourceRow_par_con < 0 or destinationRow_par_con < 0
//        or sourceRow_par_con > rowToActionDataId_pri.size() or destinationRow_par_con > rowToActionDataId_pri.size())
//    {
//        return;
//    }
//    if (sourceRow_par_con > destinationRow_par_con)
//    {
//        //the source is further down than the destination, so move all the rows from source to destination+1 and then replace the destination
//        //to lose only one value, start from the source and move "up"

//        //back source id since it will be replaced by the first moved row (the one above)
//        auto sourceDataIdBackup(rowToActionDataId_pri.at(sourceRow_par_con));

//        auto indexTmp(sourceRow_par_con);
//        while (indexTmp > destinationRow_par_con)
//        {
//            //replace the row with the one above, current row = index, row above = index -1
//            auto movedActionIdTmp(rowToActionDataId_pri[indexTmp - 1]);
//            rowToActionDataId_pri[indexTmp] = movedActionIdTmp;
//            //for this actionId update the row value
//            actionDataIdToRow_pri[movedActionIdTmp] = indexTmp;
//            indexTmp = indexTmp - 1;
//        }
//        //replace the destination with the source (old destination has been moved +1 index)
//        rowToActionDataId_pri[destinationRow_par_con] = sourceDataIdBackup;
//        actionDataIdToRow_pri[sourceDataIdBackup] = destinationRow_par_con;
//    }
//    else
//    {
//        //the source is further up than destination, so move all the rows from the source to destination-1 and then replace destination
//        //to lose only one value, start from the source and move "down"

//        //back source id since it will be replaced by the first moved row (the one below)
//        auto sourceDataIdBackup(rowToActionDataId_pri.at(sourceRow_par_con));

//        auto indexTmp(sourceRow_par_con);
//        while (indexTmp < destinationRow_par_con)
//        {
//            //replace the row with the one below, current row = index, row below = index +1
//            auto movedActionIdTmp(rowToActionDataId_pri[indexTmp + 1]);
//            rowToActionDataId_pri[indexTmp] = movedActionIdTmp;
//            //for this actionId update the row value
//            actionDataIdToRow_pri[movedActionIdTmp] = indexTmp;
//            indexTmp = indexTmp + 1;
//        }
//        //replace the destination with the source (old destination has been moved -1 index)
//        rowToActionDataId_pri[destinationRow_par_con] = sourceDataIdBackup;
//        actionDataIdToRow_pri[sourceDataIdBackup] = destinationRow_par_con;

//    }
//}


executionOptions_c actonDataHub_c::executionOptions_f() const
{
	return executionOptions_pri;
}

void actonDataHub_c::setExecutionOptions_f(const executionOptions_c& executionOptions_par_con)
{
	if (executingActions_pri)
	{
		//do nothing if already executing
	}
	else
	{
		executionOptions_pri = executionOptions_par_con;
	}
}

//executionOptions_c& actonDataHub_c::executionOptionsRef_f()
//{
//	return executionOptions_pri;
//}

bool actonDataHub_c::executeActionDataRows_f(std::vector<int> rows_par, textCompilation_c* errors_par)
{
	bool resulTmp(false);
	MACRO_ADDLOG("Execute Actions from rows", messageType_ec::debug);
	while (true)
	{
		if (executingActions_pri)
		{
			//do nothing if already executing
			break;
		}

		if (areCheckActionDependenciesValid_f(errors_par))
		{
			//good
		}
		else
		{
			break;
		}

		bool somethingIsExecuting(false);
		bool actionsToRunValidTmp(true);
		resumingExecution_pri = false;
		actionsToRun_pri.clear();
		executionLoopCount_pri = 0;
		if (rows_par.empty())
		{
			for (size_t i = 0; i < actionDataIdToRow_pri.size(); ++i)
			{
				rows_par.emplace_back(i);
			}
		}
		else
		{
			removeNegative_ft(rows_par);
			removeDuplicates_ft(rows_par);
		}
		for (const int row_ite_con : rows_par)
		{
			int_fast64_t actionIdTmp(rowToActionDataId_f(row_ite_con));
			if (actionIdTmp not_eq -1)
			{
				action_c* actionPtrTmp(action_ptr_f(actionIdTmp));
				if (actionPtrTmp->isExecuting_f())
				{
					MACRO_ADDLOG("Already Executing", actionPtrTmp, messageType_ec::warning);
					somethingIsExecuting = true;
					break;
				}
				if (not actionPtrTmp->enabled_f())
				{
					MACRO_ADDLOG("Action is not enabled", actionPtrTmp, messageType_ec::warning);
					continue;
				}
				if (not actionPtrTmp->isFieldsActionValid_f(errors_par))
				{
					MACRO_ADDLOG("Action to execute is not valid", actionPtrTmp, messageType_ec::error);
					actionsToRunValidTmp = false;
					continue;
				}
				actionsToRun_pri.emplace_back(actionPtrTmp);
			}
		}
		if (not somethingIsExecuting and actionsToRunValidTmp)
		{
			executeActions_f();
		}

		resulTmp = true;
		break;
	}
	return resulTmp;
}

bool actonDataHub_c::actionsExecutionFinished_f() const
{
	return actionsExecutionFinished_pri;
}

bool actonDataHub_c::killingActionsExecution_f() const
{
	return killingActionsExecution_pri;
}

bool actonDataHub_c::actionsExecutionKilled_f() const
{
	return actionsExecutionKilled_pri;
}

bool actonDataHub_c::logDataHubSet_f() const
{
	return logDataHub_pri not_eq nullptr;
}

void actonDataHub_c::setLogDataHub_f(logDataHub_c* logDataHub_par)
{
	if (logDataHub_par not_eq nullptr)
	{
		logDataHub_pri = logDataHub_par;
	}
}

void actonDataHub_c::executeActions_f(const bool loop_par_con)
{
    text_c textTmp("Execute actions, loop: {0}", QSTRINGBOOL(loop_par_con));
    MACRO_ADDLOG(textTmp, messageType_ec::debug);
    if (not actionsToRun_pri.empty())
    {
        actionsExecutionFinished_pri = false;
        actionsExecutionStopped_pri = false;
        actionsExecutionKilled_pri = false;
        executingActions_pri = true;
        Q_EMIT actionsExecutionStarted_signal();

        for (action_c* action_ite_ptr : actionsToRun_pri)
        {
            if (resumingExecution_pri
                and action_ite_ptr->actionDataExecutionResult_ptr_f() not_eq nullptr
                and action_ite_ptr->actionDataExecutionResult_ptr_f()->finished_f()
                and action_ite_ptr->actionDataExecutionResult_ptr_f()->lastState_f() not_eq actionExecutionState_ec::stopped)
            {
                //when resuming ignore finished actions that aren't stopped
            }
            else
            {
                //this can return nullptr if the action is not enabled, but actionsToRun_pri only contains enabled action ptrs
                actionExecutionResult_c* actionDataExecutionResultTmp(action_ite_ptr->regenerateGetActionDataExecutionResult_ptr_f());
                executionResults_pri.emplace_back(actionDataExecutionResultTmp);
    //            if (actionDataExecutionResultTmp->lastState_f() == actionExecutionState_ec::initial)
    //            {
    //                //initial state requires no changes
    //            }
    //            else
    //            {
    //                actionDataExecutionResultTmp->tryClear_f();
    //            }

            }
        }
        for (action_c* action_ite_ptr : actionsToRun_pri)
        {
            if (resumingExecution_pri
                and action_ite_ptr->actionDataExecutionResult_ptr_f() not_eq nullptr
                and action_ite_ptr->actionDataExecutionResult_ptr_f()->finished_f()
                and action_ite_ptr->actionDataExecutionResult_ptr_f()->lastState_f() not_eq actionExecutionState_ec::stopped)
            {
                //when resuming ignore finished actions that aren't stopped
            }
            else
            {
                actionExecutionResult_c* actionDataExecutionResultTmp(action_ite_ptr->actionDataExecutionResult_ptr_f());
                //when an action finishes verify if the overall execution has finished,
                QObject::connect(
                            actionDataExecutionResultTmp
                            , &actionExecutionResult_c::finished_signal
                            , this
                            , &actonDataHub_c::verifyExecutionFinished_f
                            , Qt::UniqueConnection
                            );

                //when an action signals it's killing, signal this object, actonDataHub_c that killing is going on
                //also disconnect the "connect" that did it
                QObject::connect(
                            actionDataExecutionResultTmp
                            , &actionExecutionResult_c::killing_signal
                            , this
                            , &actonDataHub_c::killingStarted_f
                            , Qt::UniqueConnection
                            );
                //collect the execution messages
                QObject::connect(
                            actionDataExecutionResultTmp
                            , &actionExecutionResult_c::messageAdded_signal
                            , this
                            , &actonDataHub_c::addActionExecutionResultMessage_f
                            , Qt::UniqueConnection
                            );
                action_ite_ptr->tryExecute_f();
            }
        }
    }
}

std::deque<executionResult_c*> actonDataHub_c::executionResults_f() const
{
    return executionResults_pri;
}

std::deque<executionMessage_c*> actonDataHub_c::executionMessages_f() const
{
    return executionMessages_pri;
}

void actonDataHub_c::addExecutionMessage_f(executionMessage_c* executionMessage_par)
{
    executionMessages_pri.emplace_back(executionMessage_par);
    Q_EMIT executionMessageAdded_signal(executionMessage_par);
#ifdef DEBUGJOUVEN
    //qDebug() << "executionMessages_pri.size() " << executionMessages_pri.size() << Qt::endl;
#endif
    //sort by id should be fast if the deque is already sorted
    std::sort(std::execution::par_unseq, executionMessages_pri.begin(), executionMessages_pri.end(),
              [](executionMessage_c* a_par, executionMessage_c* b_par) -> bool
    {
        return a_par->id_f() < b_par->id_f();
    });
    //clear cache
    //the main problem here is not deleting the execution message objects but removing all the ptrs pointing to them
    //they are in 2 places with ptrs:
    //1 in executionMessages_pri
    //2 in the executionResults_c messages_pro variable, getting there can be done checking the parents of the each message which is a executionResults_c obj
    //  ptrs from executionResults_c can be removed using deleteMessages_f
    if (executionMessages_pri.size() == executionMessageMaxDequeSize_pri)
    {
        std::unordered_set<executionResult_c*> parentsTmp;
        std::vector<executionMessage_c*> messagesToRemoveTmp;
        messagesToRemoveTmp.reserve(executionMessageClearCacheAmount_pri);

        //do a container with the execution results this messages reference
        for (uint_fast64_t i = 0; i < executionMessageClearCacheAmount_pri; ++i)
        {
            parentsTmp.emplace(static_cast<executionResult_c*>(executionMessages_pri.at(i)->objThatHasExecutionMessagesPtr_f()));
            messagesToRemoveTmp.emplace_back(executionMessages_pri.at(i));
        }
        //unreference-remove the messages from the execution results message vectors
        for (executionResult_c* executionResult_ite : parentsTmp)
        {
            executionResult_ite->unreferenceMessages_f(messagesToRemoveTmp);
        }
        //remove them from executionMessages_pri
        executionMessages_pri.erase(executionMessages_pri.begin(), executionMessages_pri.begin() + executionMessageClearCacheAmount_pri);
        //destroy the actual objects them
        for (executionMessage_c* messageToDestroy_ite : messagesToRemoveTmp)
        {
            messageToDestroy_ite->deleteLater();
        }
    }
}

void actonDataHub_c::verifyExecutionFinished_f(executionResult_c* executionResultPtr_par)
{
    bool allFinishedTmp(true);
    bool somethingStoppedTmp(false);
    bool somethingKilledTmp(false);
    for (action_c* action_ite_ptr : actionsToRun_pri)
    {
//#ifdef DEBUGJOUVEN
//        if (action_ite_ptr->actionDataExecutionResult_ptr_f() == nullptr)
//        {
//            qDebug() << "action_ite_ptr->actionDataExecutionResult_ptr_f() == nullptr " << Qt::endl;
//        }
//#endif
        if (not action_ite_ptr->actionDataExecutionResult_ptr_f()->finished_f())
        {
            allFinishedTmp = false;
            break;
        }
        if (action_ite_ptr->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::stopped)
        {
            somethingStoppedTmp = true;
        }
        if (action_ite_ptr->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::killed)
        {
            somethingKilledTmp = true;
        }
    }
    while (true)
    {
        const actionExecutionResult_c* const actionExecutionResultPtrTmp(static_cast<actionExecutionResult_c*>(executionResultPtr_par));
        action_c* const actionPtrTmp(actionExecutionResultPtrTmp->action_ptr_f());
        if (actionPtrTmp->repeatExecution_f()
            and not stopWhenCurrentExecutionCycleFinishes_pri
            and not stoppingActionsExecution_pri)
        {
            actionPtrTmp->tryExecute_f();
            allFinishedTmp = false;
        }

        if (allFinishedTmp)
        {
            MACRO_ADDLOG("All actions finished executing", messageType_ec::information);
            //remove parser configs created during execution
            for (const QString& str_ite_con : stringTriggerCreationCollection_f())
            {
                executionOptions_pri.stringParserMap_f()->removeParser_f(executionOptions_pri.stringParserMap_f()->stringTriggerIndex_f(str_ite_con));
            }

            MACRO_ADDLOG("After remove parser configs created during execution", messageType_ec::debug);
            if (stoppingActionsExecution_pri)
            {
                stoppingActionsExecution_pri = false;
                MACRO_ADDLOG("Execution finished, after stopping", messageType_ec::information);
                if (somethingStoppedTmp)
                {
                    MACRO_ADDLOG("Execution finished, one or more actions were stopped", messageType_ec::information);
                    actionsExecutionStopped_pri = true;
                    Q_EMIT actionsExecutionStopped_signal();
                }
            }

            if (killingActionsExecution_pri)
            {
                MACRO_ADDLOG("Execution finished, after killing", messageType_ec::information);
                killingActionsExecution_pri = false;
                if (somethingKilledTmp)
                {
                    MACRO_ADDLOG("Execution finished, one or more actions were killed", messageType_ec::information);
                    actionsExecutionKilled_pri = true;
                    Q_EMIT actionsExecutionKilled_signal();
                }
            }

            executingActions_pri = false;
            actionsExecutionFinished_pri = true;
            Q_EMIT actionsExecutionFinished_signal(actionsToRun_pri);

            if (not actionsExecutionKilled_pri and not actionsExecutionStopped_pri)
            {
                {
                    text_c textTmp("Execution cycle finished, stop on this execution cycle? {0}", QSTRINGBOOL(stopWhenCurrentExecutionCycleFinishes_pri));
                    MACRO_ADDLOG(textTmp, messageType_ec::information);
                }
                //niche case, if the last action "errors", all actions have finished and they can't be stopped,
                //but if that action stopExecutionOnError_f is true, loopExecution can't happen either
                //so overall actions execution can't be stopped but also it can't be looped
                //it's the "else" case from "if (allFinishedTmp)" but without doing "tryStopExecutingActions_f();"
                if (actionPtrTmp->stopAllExecutionOnError_f() and actionPtrTmp->actionDataExecutionResult_ptr_f()->hasErrorMessages_f())
                {
                    stopWhenCurrentExecutionCycleFinishes_pri = true;
                }
                if ((executionOptions_pri.executionLoopType_f() == executionOptions_c::executionLoopType_ec::forever
                    or (executionOptions_pri.executionLoopType_f() == executionOptions_c::executionLoopType_ec::loopXTimes
                        and executionLoopCount_pri < executionOptions_f().loopXTimesCount_f()))
                    and not stopWhenCurrentExecutionCycleFinishes_pri)
                {
                    executionLoopCount_pri = executionLoopCount_pri + 1;
                    text_c textTmp("Execution finished, looping execution, loop number {0}", executionLoopCount_pri);
                    MACRO_ADDLOG(textTmp, messageType_ec::information);
                    executeActions_f(true);
                }
                if (stopWhenCurrentExecutionCycleFinishes_pri)
                {
                    stopWhenCurrentExecutionCycleFinishes_pri = false;
                }
            }
        }
        else
        {
            //if the action has the option stop the execution on error
            //and it has errors stop the execution
            if (actionPtrTmp->stopAllExecutionOnError_f()
                and actionPtrTmp->actionDataExecutionResult_ptr_f()->hasErrorMessages_f())
            {
                tryStopExecutingActions_f();
            }
        }
        break;
    }
}

void actonDataHub_c::killingStarted_f()
{
    if (killingActionsExecution_pri)
    {
        MACRO_ADDLOG("Killing execution while already killing", messageType_ec::debug);
    }
    else
    {
        MACRO_ADDLOG("Killing execution", messageType_ec::information);
        killingActionsExecution_pri = true;
        Q_EMIT killingActionsExecution_signal();
    }
}



void actonDataHub_c::addActionExecutionResultMessage_f(executionResult_c* , executionMessage_c* message_par)
{
    addExecutionMessage_f(message_par);
}

void actonDataHub_c::addCheckExecutionResultMessage_f(executionResult_c*, executionMessage_c* message_par)
{
    addExecutionMessage_f(message_par);
}

void actonDataHub_c::tryStopExecutingActions_f(const bool killAfterTimeout_par_con)
{
    if (executingActions_pri)
    {
        stoppingActionsExecution_pri = true;
        Q_EMIT stoppingActionsExecution_signal();
        text_c textTmp("Stopping {0} actions", actionsToRun_pri.size());
        MACRO_ADDLOG(textTmp, messageType_ec::information);
        for (action_c* action_ite_ptr : actionsToRun_pri)
        {
            action_ite_ptr->tryStopExecution_f(killAfterTimeout_par_con);
        }
    }
    else
    {
        MACRO_ADDLOG("Stopping execution while nothing is executing", messageType_ec::debug);
    }
}

void actonDataHub_c::stopWhenCurrentExecutionCycleFinishes_f()
{
    stopWhenCurrentExecutionCycleFinishes_pri = true;
}

void actonDataHub_c::tryResumeActionsExecution_f()
{
    MACRO_ADDLOG("Try resume actions execution", messageType_ec::debug);
    if (actionsExecutionStopped_pri)
    {
        resumingExecution_pri = true;
        executeActions_f();
    }
    else
    {
        MACRO_ADDLOG("Trying to resume when previous execution wasn't stopped", messageType_ec::information);
    }
}

bool actonDataHub_c::executingActions_f() const
{
    return executingActions_pri;
}

//bool actonDataHub_c::isAnyActionExecutingChecks_f() const
//{
//    bool resultTmp(false);
//    MACRO_ADDACTONQTSOLOG("Check if any action executing checks", messageType_ec::debug);
//    for (const std::pair<const int_fast64_t, actionData_c>& actionDataIdObjectPair_ite_con : actionDataIdToActionDataUMap_pri)
//    {
//        if (actionDataIdObjectPair_ite_con.second.checkDataHub_f().executingChecks_f())
//        {
//            MACRO_ADDACTONQTSOLOG("An action is executing checks", messageType_ec::information);
//            resultTmp = true;
//            break;
//        }
//    }

//    return resultTmp;
//}

//void actonDataHub_c::stopAnyExecutingChecks_f()
//{

//    for (std::pair<const int_fast64_t, actionData_c>& actionDataIdObjectPair_ite_con : actionDataIdToActionDataUMap_pri)
//    {
//        if (actionDataIdObjectPair_ite_con.second.checkDataHub_f().executingChecks_f())
//        {
//            QObject::connect(
//                        actionDataIdObjectPair_ite_con.second.checkDataHub_ptr_f()->proxyQObj_f()
//                        , &checksDataHubProxyQObj_c::checksExecutionFinished_signal
//                        , proxyQObj_pri
//                        , &actonDataHubProxyQObj_c::anyExecutingChecksStopped_signal
//            );
//            actionDataIdObjectPair_ite_con.second.checkDataHub_ptr_f()->stopExecutingChecks_f();
//        }
//    }
//}

//checked now in executeActionDataRows_f when each action is checked for is executing_f
//QString actonDataHub_c::actionStringIdExecutingChecks_f() const
//{
//    QString resultTmp;
//    for (const std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite_con : actionDataIdToActionPtrUMap_pri)
//    {
//        if (actionIdObjectPair_ite_con.second->checkDataHub_f().executingChecks_f())
//        {
//            text_c textTmp("An action is executing checks, action stringId: ", actionIdObjectPair_ite_con.second->stringId_f());
//            MACRO_ADDACTONQTSOLOG(textTmp, messageType_ec::debug);
//            resultTmp = actionIdObjectPair_ite_con.second->stringId_f();
//            break;
//        }
//    }
//    return resultTmp;
//}

bool actonDataHub_c::stoppingActionsExecution_f() const
{
    return stoppingActionsExecution_pri;
}

bool actonDataHub_c::actionsExecutionStopped_f() const
{
    return actionsExecutionStopped_pri;
}

uint_fast64_t actonDataHub_c::updateStringIdDependencies_f(
        const QString& newActionStringId_par_con
        , const QString& oldActionStringId_par_con)
{
    uint_fast64_t updateCountTmp(0);
    if (executingActions_pri or (newActionStringId_par_con == oldActionStringId_par_con))
    {
        //don't update stringId dependencies while executing
    }
    else
    {
        for (std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite : actionDataIdToActionPtrUMap_pri)
        {
            updateCountTmp = updateCountTmp + actionIdObjectPair_ite.second->updateActionStringIdDependencies_f(newActionStringId_par_con, oldActionStringId_par_con);
        }
        if (updateCountTmp > 0)
        {
            text_c textTmp("Updated {0} action/checks with action stringId dependencies", updateCountTmp);
            MACRO_ADDLOG(textTmp, messageType_ec::information);
        }
    }
    return updateCountTmp;
}

uint_fast64_t actonDataHub_c::actionStringIdDependencyCount_f(const QString& actionStringId_par_con) const
{
    uint_fast64_t resultTmp(0);
    for (const std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite_con : actionDataIdToActionPtrUMap_pri)
    {
        resultTmp = resultTmp + actionIdObjectPair_ite_con.second->actionStringIdDependencyCount_f(actionStringId_par_con);
    }
    if (resultTmp > 0)
    {
        text_c textTmp("Action stringId \"{0}\" has dependencies: {1}", actionStringId_par_con, resultTmp);
        MACRO_ADDLOG(textTmp, messageType_ec::information);
    }
    return resultTmp;
}

uint_fast64_t actonDataHub_c::updateStringTriggerDependencies_f(
        const QString& newStringTrigger_par_con
        , const QString& oldStringTrigger_par_con)
{
    int_fast32_t updateCountTmp(0);
    if (executingActions_pri or (newStringTrigger_par_con == oldStringTrigger_par_con))
    {
        //don't while executing or it's the same
    }
    else
    {
        for (std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite : actionDataIdToActionPtrUMap_pri)
        {
            updateCountTmp = updateCountTmp + actionIdObjectPair_ite.second->updateStringTriggerDependencies_f(newStringTrigger_par_con, oldStringTrigger_par_con);
        }
        if (updateCountTmp > 0)
        {
            text_c textTmp("Updated {0} checks with action key string parser dependencies", updateCountTmp);
            MACRO_ADDLOG(textTmp, messageType_ec::information);
        }
    }
    return updateCountTmp;
}

bool actonDataHub_c::changeActionStringId_f(const QString& oldActionStringId_par_con, const QString& newActionStringId_par_con)
{
    bool resulTmp(false);
    if (executingActions_pri or (oldActionStringId_par_con == newActionStringId_par_con))
    {
        //don't while executing or it's the same
    }
    else
    {
        int_fast64_t actionIdTmp(actionDataStringIdToActionDataId_pri.value(oldActionStringId_par_con));
        if (actionIdTmp > 0)
        {
            actionDataStringIdToActionDataId_pri.remove(oldActionStringId_par_con);
            actionDataStringIdToActionDataId_pri.insert(newActionStringId_par_con, actionIdTmp);
            resulTmp = true;
        }
    }
    return resulTmp;
}

void actonDataHub_c::addExecutionResult_f(executionResult_c* executionResult_par)
{
    executionResults_pri.emplace_back(executionResult_par);
    Q_EMIT executionResultAdded_signal(executionResult_par);
    //execution result sorting is not necessary, and they can always be sorted after the fact by start time
    //the take away for now AFAIK there is nothing usefull from knowing if an execution result happened before another (when their start times match)
//    //sort by id should be fast if the deque is already sorted
//    std::sort(executionResults_pri.begin(), executionResults_pri.end(),
//              [](executionResult_c* a_par, executionResult_c* b_par) -> bool
//    {
//        return a_par->id_f() < b_par->id_f();
//    });

    //when clearing the deque cached executions results an extra step is required to deal with the execution messages they have?
    //no because the messages have no "hard reference" to the execution results
    //but when deleting execution messages, in their cache deletion, the ptrs on the execution results must be dealt with
    if (executionResults_pri.size() == executionResultMaxDequeSize_pri)
    {
        for (uint_fast64_t i = 0; i < executionResultClearCacheAmount_pri; ++i)
        {
            //iterate the execution messages and "clear the reference" to this execution result obj
            for (executionMessage_c* executionMessage_ite : executionResults_pri.at(i)->messages_f())
            {
                executionMessage_ite->unreferenceObjThatHasExecutionMessages_f();
            }
            executionResults_pri.at(i)->deleteLater();
        }
        executionResults_pri.erase(executionResults_pri.begin(), executionResults_pri.begin() + executionResultClearCacheAmount_pri);
    }
    //problem 1: when partially clearing the execution result cache
    //right now execution result messages are parented to the execution results so if a execution result is removed, so are it's children but
    //the children are referenced in executionMessages_pri
    //problem 2: when partially clearing the execution message cache
    //the execution messages are referenced in executionMessages_pri and in the executionResult_c objects

    //to solve 1 don't parent the messages and only remove them when clearing the message cache, still the messages need to reference the execution result object
    //so their executions message vector can have the messages going to be deleted removed from that vector,
    //also the messages need to be able to clear that reference when the execution result obj is destroyed but the messages continue to exist
    //to solve 2 use the reference to the execution result to remove the messages from the execution result execution message vector
}

std::pair<int_fast64_t, int_fast64_t> actonDataHub_c::stringTriggerCreationConflict_f(
        const QString& stringTrigger_par_con
        , const void* const objectToIgnore_par
        , const bool includeStringParserMap_par_con) const
{
    std::pair<int_fast64_t, int_fast64_t> resultTmp(-1,-1);
    for (const std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite_con : actionDataIdToActionPtrUMap_pri)
    {
        if (actionIdObjectPair_ite_con.second == objectToIgnore_par)
        {
            continue;
        }
        resultTmp = actionIdObjectPair_ite_con.second->stringTriggerCreationConflict_f(stringTrigger_par_con, objectToIgnore_par);
    }
    if (includeStringParserMap_par_con and executionOptions_pri.stringParserMap_f() not_eq nullptr)
    {
        if (executionOptions_pri.stringParserMap_f()->stringTriggerIndex_f(stringTrigger_par_con) > -1)
        {
            resultTmp = std::pair<int_fast64_t, int_fast64_t>(0,0);
        }
    }
    if (resultTmp.first > -1)
    {
        text_c textTmp("Action key string parser {0} first dependency actionId {1}, checkId {2}", stringTrigger_par_con, resultTmp.first, resultTmp.second);
        MACRO_ADDLOG(textTmp, messageType_ec::information);
    }
    return resultTmp;
}

uint_fast64_t actonDataHub_c::stringTriggerDependencyCount_f(
        const QString& stringId_par_con
        , const void* const objectToIgnore_par) const
{
    uint_fast64_t resultTmp(0);
    for (const std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite_con : actionDataIdToActionPtrUMap_pri)
    {
        if (actionIdObjectPair_ite_con.second == objectToIgnore_par)
        {
            continue;
        }
        resultTmp = resultTmp + actionIdObjectPair_ite_con.second->stringTriggerDependencyCount_f(stringId_par_con, objectToIgnore_par);
    }
    return resultTmp;
}

QSet<QString> actonDataHub_c::stringTriggerCreationCollection_f() const
{
    QSet<QString> resultTmp;
    for (const std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite_con : actionDataIdToActionPtrUMap_pri)
    {
        QSet<QString> setTmp(actionIdObjectPair_ite_con.second->stringTriggerCreationCollection_f());
        resultTmp.unite(setTmp);
    }
    return resultTmp;
}

QSet<QString> actonDataHub_c::stringTriggersInUseByActionsOrChecks_f() const
{
    QSet<QString> resultTmp;
    QSet<QString> stringTriggersToSearchTmp;
    if (executionOptions_pri.stringParserMap_f() not_eq nullptr)
    {
        QList<QString> stringTriggerInParserMapTmp(executionOptions_pri.stringParserMap_f()->stringTriggers_f());
        for (const QString& stringTrigger_ite_con : stringTriggerInParserMapTmp)
        {
            stringTriggersToSearchTmp.insert(stringTrigger_ite_con);
        }
    }
    stringTriggersToSearchTmp.unite(stringTriggerCreationCollection_f());
    for (const std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite_con : actionDataIdToActionPtrUMap_pri)
    {
        QSet<QString> setTmp(actionIdObjectPair_ite_con.second->stringTriggersInUse_f(stringTriggersToSearchTmp));
        resultTmp.unite(setTmp);
    }
    return resultTmp;
}

actonDataHub_c::actonDataHub_c(QObject* parent_par)
    : QObject(parent_par)
{
    //Qt checks if a metatype is already declared, so it shouldn't? matter
    //if actonDataHub_c ctor is called more than once
    //qRegisterMetaType<action_c*>("action_c*");
    //qRegisterMetaType<check_c*>("check_c*");
    qRegisterMetaType<actionExecutionState_ec>("actionExecutionState_ec");
    qRegisterMetaType<checkExecutionState_ec>("checkExecutionState_ec");
    qRegisterMetaType<text_c>("text_c");
    qRegisterMetaType<textCompilation_c>("textCompilation_c");
}

actonDataHub_c::~actonDataHub_c()
{
    for (executionMessage_c* executionMessage_par : executionMessages_pri)
    {
        executionMessage_par->deleteLater();
    }
    for (executionResult_c* executionResult_par : executionResults_pri)
    {
        executionResult_par->deleteLater();
    }
}

bool actonDataHub_c::validRow_f(const int row_par_con) const
{
    return row_par_con >= 0 and row_par_con <= size_f();
}

bool actonDataHub_c::validStringId_f(const QString& actionDataStringId_par_con) const
{
    return not actionDataStringId_par_con.isEmpty()
            and not actionDataStringIdToActionDataId_pri.contains(actionDataStringId_par_con);
}

bool actonDataHub_c::insertActionData_f(
        action_c* const actionPtr_par
        , const int row_par_con
        , textCompilation_c* errorsPtr_par)
{
    bool resultTmp(false);
    while (true)
    {
        if (executingActions_pri)
        {
            APPENDTEXTPTR(errorsPtr_par, "Can't insert action while executing")
            break;
        }

        if (not validRow_f(row_par_con))
        {
            text_c errorTextTmp("Row {0} to insert action is not valid", row_par_con);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        if (not validStringId_f(actionPtr_par->stringId_f()))
        {
            text_c errorTextTmp("Action stringId \"{0}\" is not valid (empty or already in use)", actionPtr_par->stringId_f());
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            break;
        }

        //insert actionId - actionData object mapping
        actionDataIdToActionPtrUMap_pri.emplace(actionPtr_par->id_f(), actionPtr_par);

        //insert actionDataStringId - actionDataId mapping
        actionDataStringIdToActionDataId_pri.insert(actionPtr_par->stringId_f(), actionPtr_par->id_f());

        //new row goes in the last position
        if (row_par_con == size_f())
        {
            //insert the mappings (since it's the last row no need to touch anything else)
            actionDataIdToRow_pri.emplace(actionPtr_par->id_f(), row_par_con);
            rowToActionDataId_pri.emplace(row_par_con, actionPtr_par->id_f());
        }
        else
        {
            //new row goes in between existing ones (moving them one position and replacing one)
            //create a row at the end and move all the rows before one index
            auto newLastRowIndexTmp(size_f());
            {
                //get the current last row actionId
                auto movedActionIdTmp(rowToActionDataId_pri.at(newLastRowIndexTmp - 1));

                //insert this actionId into a new last row (to make space for the new row that will go in-between)
                rowToActionDataId_pri.emplace(newLastRowIndexTmp, movedActionIdTmp);

                //update the actionId - row mapping
                actionDataIdToRow_pri.at(movedActionIdTmp) = newLastRowIndexTmp;
                //decrease the index (to deal with the previous to last)
                newLastRowIndexTmp = newLastRowIndexTmp - 1;
            }
            //move all the rows from the previous to last row to the target row, to row+1 (to leave space for the new one)
            while (row_par_con < newLastRowIndexTmp)
            {
                //qInfo() << "newRowIndexTmp " << newRowIndexTmp << Qt::endl;
                //qInfo() << "row_par_con " << row_par_con << Qt::endl;
                //get the actionId of the previous row
                auto movedActionIdTmp(rowToActionDataId_pri.at(newLastRowIndexTmp - 1));
                //move the mappings to "row+1"
                //insert the actionId to the next row
                rowToActionDataId_pri.at(newLastRowIndexTmp) = movedActionIdTmp;
                //update the actionId - row mapping with the next row
                actionDataIdToRow_pri.at(movedActionIdTmp) = newLastRowIndexTmp;

                newLastRowIndexTmp = newLastRowIndexTmp - 1;
            }

            //since all the rows (mappings), target and below, have been moved, it's only necessary to insert
            //the new mapping in the desired row
            actionDataIdToRow_pri.emplace(actionPtr_par->id_f(), row_par_con);
            rowToActionDataId_pri.at(row_par_con) = actionPtr_par->id_f();
        }
        //this way there is no need to delete all the action/checks on the destruction of the actonHub_c object
        actionPtr_par->setActonDataHubParent_f(this);
        resultTmp = true;

        QObject::connect(actionPtr_par, &action_c::actionStringIdChanged_signal, this, &actonDataHub_c::changeActionStringId_f, Qt::UniqueConnection);
        break;
    }
    text_c textTmp("Action inserted? {0}", QSTRINGBOOL(resultTmp));
    MACRO_ADDLOG(textTmp, messageType_ec::information);
    return resultTmp;
}


int actonDataHub_c::actionDataIdToRow_f(const int_fast64_t actionDataId_par_con) const
{
    int resultTmp(-1);
    auto findResult(actionDataIdToRow_pri.find(actionDataId_par_con));
    if (findResult not_eq actionDataIdToRow_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

QString actonDataHub_c::actionDataIdToActionDataStringId_f(const int_fast64_t actionDataId_par_con) const
{
    QString resultTmp;
    auto findResult(actionDataIdToActionPtrUMap_pri.find(actionDataId_par_con));
    if (findResult not_eq actionDataIdToActionPtrUMap_pri.end())
    {
        resultTmp = findResult->second->stringId_f();
    }
    return resultTmp;
}

int_fast64_t actonDataHub_c::rowToActionDataId_f(const int row_par_con) const
{
    int_fast64_t resultTmp(-1);
    auto findResult(rowToActionDataId_pri.find(row_par_con));
    if (findResult not_eq rowToActionDataId_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

int_fast64_t actonDataHub_c::actionDataStringIdToActionDataId_f(const QString& actionDataStringId_par_con) const
{
    int_fast64_t resultTmp(0);
    QHash<QString, int_fast64_t>::const_iterator findResult(actionDataStringIdToActionDataId_pri.find(actionDataStringId_par_con));
    if (findResult not_eq actionDataStringIdToActionDataId_pri.end())
    {
        resultTmp = findResult.value();
    }
    return resultTmp;
}

int_fast32_t actonDataHub_c::size_f() const
{
    return rowToActionDataId_pri.size();
}

action_c* actonDataHub_c::action_ptr_f(const int_fast64_t actionDataId_par_con)
{
    action_c* actionTmp_ptr(nullptr);
    auto findResult(actionDataIdToActionPtrUMap_pri.find(actionDataId_par_con));
    if (findResult not_eq actionDataIdToActionPtrUMap_pri.end())
    {
        actionTmp_ptr = findResult->second;
    }
    return actionTmp_ptr;
}

action_c*actonDataHub_c::action_ptr_f(const int_fast64_t actionDataId_par_con) const
{
    action_c* actionTmp_ptr(nullptr);
    auto findResult(actionDataIdToActionPtrUMap_pri.find(actionDataId_par_con));
    if (findResult not_eq actionDataIdToActionPtrUMap_pri.end())
    {
        actionTmp_ptr = findResult->second;
    }
    return actionTmp_ptr;
}

void actonDataHub_c::clearAllActionData_f()
{
    text_c textTmp("Clear all actionDataHub data, executing actions? {0}", QSTRINGBOOL(executingActions_pri));
    MACRO_ADDLOG(textTmp, messageType_ec::debug);
    if (executingActions_pri)
    {
    }
    else
    {
        //delete the action objects first, then clear the containers
        for (std::pair<const int_fast64_t, action_c*>& pair_ite : actionDataIdToActionPtrUMap_pri)
        {
            pair_ite.second->deleteLater();
        }
        actionsExecutionFinished_pri = false;
        //stoppingActionsExecution_pri = false;
        actionsExecutionStopped_pri = false;
        //killingActionsExecution_pri = false;
        actionsExecutionKilled_pri = false;
        actionsToRun_pri.clear();
        actionDataIdToRow_pri.clear();
        rowToActionDataId_pri.clear();
        actionDataIdToActionPtrUMap_pri.clear();
        actionDataStringIdToActionDataId_pri.clear();
        if (executionOptions_pri.stringParserMap_f() not_eq nullptr)
        {
            executionOptions_pri.stringParserMap_f()->clear_f();
        }
    }
}

QStringList actonDataHub_c::actionStringIdList_f(const bool sorted_par_con) const
{
    QStringList resultTmp;
    resultTmp.reserve(actionDataIdToActionPtrUMap_pri.size());
    for (const std::pair<const int_fast64_t, action_c*>& actionIdObjectPair_ite_con : actionDataIdToActionPtrUMap_pri)
    {
#ifdef DEBUGJOUVEN
        qDebug() << "actionIdObjectPair_ite_con.second " << actionIdObjectPair_ite_con.second << Qt::endl;
#endif
        resultTmp.append(actionIdObjectPair_ite_con.second->stringId_f());
    }
#ifdef DEBUGJOUVEN
    qDebug() << "after actionIdObjectPair_ite_con.second " << Qt::endl;
#endif
    if (sorted_par_con)
    {
        resultTmp.sort();
    }
#ifdef DEBUGJOUVEN
    qDebug() << "after 2 actionIdObjectPair_ite_con.second " << Qt::endl;
#endif
    return resultTmp;
}

//void actionDataController_c::runActionsDataId_f(const std::vector<int_fast64_t>& actionIds_par_con)
//{
//    for (const int_fast64_t actionId_ite_con : actionIds_par_con)
//    {
//        actionData_c& actionDataTmp(actionData_f(actionId_ite_con).first);
//        actionDataTmp.createExecution_f();
//    }

//}

//void actionDataController_c::runActionsFromRows_f(const std::vector<int>& rows_par_con)
//{
//    std::vector<int_fast64_t> dataIdsTmp;
//    dataIdsTmp.reserve(rows_par_con.size());
//    for (const int row_ite_con : rows_par_con)
//    {
//        dataIdsTmp.emplace_back(rowToActionDataId_f(row_ite_con));
//    }
//    runActionsDataId_f(dataIdsTmp);
//}

bool actonDataHub_c::removeActionDataUsingRow_f(
        const int row_par_con
        , const int_fast64_t actionDataId_par_con)
{
    bool resultTmp(false);
    while (true)
    {
        if (executingActions_pri)
        {
            break;
        }
        //invalid index
        if (row_par_con < 0 or row_par_con >= size_f())
        {
            break;
        }
        //no actionStringId check since is not used directly to remove

        int_fast64_t actionIdToRemoveTmp(actionDataId_par_con);
        //if it's not set, get it
        if (actionIdToRemoveTmp == 0)
        {
            actionIdToRemoveTmp = rowToActionDataId_f(row_par_con);
        }

        //the while takes this case into account too
//        //removing the last row
//        if (row_par_con == (size_f() - 1))
//        {
//            rowToActionDataId_pri.erase(row_par_con);
//            actionDataIdToRow_pri.erase(actionIdToRemoveTmp);
//            actionDataIdToActionDataUMap_pri.erase(actionIdToRemoveTmp);
//            resultTmp = true;
//            break;
//        }

        //"else"
        //removing a row with one or more rows below
        //move all the rows below the removed one to "row-1" (replacing in cascade)
        auto removedRowIndexTmp(row_par_con);
        //move all the rows from the "removed row+1" to row-1 (replacing the removed one)
        while ((removedRowIndexTmp + 1) < size_f())
        {
            //qInfo() << "removedRowIndexTmp " << removedRowIndexTmp << Qt::endl;
            //qInfo() << "size_f() " << size_f() << Qt::endl;
            //get the actionId of the next row
            auto movedActionIdTmp(rowToActionDataId_pri.at(removedRowIndexTmp + 1));
            //move the mappings to the row being replaced
            //replace the actionId with the one from the next row
            rowToActionDataId_pri.at(removedRowIndexTmp) = movedActionIdTmp;
            //replace the actionId - row mapping with the next row
            actionDataIdToRow_pri.at(movedActionIdTmp) = removedRowIndexTmp;

            removedRowIndexTmp = removedRowIndexTmp + 1;
        }

        //remove the last "row" - actionId mapping, since it has been moved (but the last one doesn't get replaced by the next one)
        rowToActionDataId_pri.erase(removedRowIndexTmp);
        //remove the mapping actionId - row mapping of the actionId being deleted
        actionDataIdToRow_pri.erase(actionIdToRemoveTmp);
        //get the actionDataStringId from the actionDataId
        QString actionDataStringIdTmp(actionDataIdToActionDataStringId_f(actionIdToRemoveTmp));
        //remove actionStringId - actionId mapping
        actionDataStringIdToActionDataId_pri.remove(actionDataStringIdTmp);
        //remove actionId - object mapping (removing the object from the data "container")
        actionDataIdToActionPtrUMap_pri.erase(actionIdToRemoveTmp);
        resultTmp = true;

        break;
    }
    text_c textTmp("Action in row {0} removed: {1}", row_par_con, QSTRINGBOOL(resultTmp));
    MACRO_ADDLOG(textTmp, messageType_ec::information);
    return resultTmp;
}

bool actonDataHub_c::removeActionDataUsingId_f(const int_fast64_t actionDataId_par_con)
{
    bool resultTmp(false);
    auto rowFindResult(actionDataIdToRow_f(actionDataId_par_con));
    if (rowFindResult not_eq -1)
    {
        resultTmp = removeActionDataUsingRow_f(rowFindResult);
    }
    return resultTmp;
}

bool actonDataHub_c::addLogMessage_f(
        const text_c& message_par_con
        , const messageType_ec logType_par_con
        , const QString& sourceFile_par_con
        , const QString& sourceFunction_par_con
        , const int_fast32_t line_par_con
) const
{
    bool resultTmp(false);
    if (logDataHub_pri not_eq nullptr)
    {
        resultTmp = logDataHub_pri->addMessage_f(message_par_con, logType_par_con, sourceFile_par_con, sourceFunction_par_con, line_par_con);
    }
    return resultTmp;
}

bool actonDataHub_c::addLogMessage_f(
        const text_c& message_par_con
        , const action_c* actionPtr_par
        , const messageType_ec  logType_par_con
        , const QString& sourceFile_par_con
        , const QString& sourceFunction_par_con
        , const int_fast32_t line_par_con
) const
{
    bool resultTmp(false);
    if (logDataHub_pri not_eq nullptr)
    {
        QString referenceTmp;
        if (actionPtr_par not_eq nullptr)
        {
            referenceTmp = "ActionId " + QString::number(actionPtr_par->id_f());
        }
        resultTmp = logDataHub_pri->addMessage_f(message_par_con, referenceTmp, logType_par_con, sourceFile_par_con, sourceFunction_par_con, line_par_con);
    }
    return resultTmp;
}

bool actonDataHub_c::addLogMessage_f(
        const text_c& message_par_con
        , const check_c* checkPtr_par
        , const messageType_ec  logType_par_con
        , const QString& sourceFile_par_con
        , const QString& sourceFunction_par_con
        , const int_fast32_t line_par_con) const
{
    bool resultTmp(false);
    if (logDataHub_pri not_eq nullptr)
    {
        QString referenceTmp;
        if (checkPtr_par not_eq nullptr)
        {
            if (checkPtr_par->parent() not_eq nullptr)
            {
                action_c* actionPtrTmp(static_cast<action_c*>(checkPtr_par->parent()));
                referenceTmp = "ActionId " + QString::number(actionPtrTmp->id_f()) + ", ";
            }
            referenceTmp.append("CheckId " + QString::number(checkPtr_par->id_f()));
        }
        resultTmp = logDataHub_pri->addMessage_f(message_par_con, referenceTmp, logType_par_con, sourceFile_par_con, sourceFunction_par_con, line_par_con);
    }
    return resultTmp;
}

bool actonDataHub_c::areActionsValid_f(textCompilation_c* errors_par) const
{
    bool areValidTmp(true);
    for (unsigned int i = 0; i < rowToActionDataId_pri.size(); ++i)
    {
         auto actionDataIdTmp(rowToActionDataId_pri.at(i));
         action_c* const actionPtrTmp_con(action_ptr_f(actionDataIdTmp));
         areValidTmp = areValidTmp and actionPtrTmp_con->isFieldsActionValid_f(errors_par);
    }
    return areValidTmp;
}

//check for actionFinished, actionStartedExecuting and folderChangeReaction if their dependencies are met
//i.e., actionFinished action should be enabled, same with actionStartedExecuting, on the other hand folderChangeReaction actions should be disabled
//but valid (which they aren't checked for during execution).
//For now don't try to add specific functions in the actions/checks, just do it here even
//if implies including check and action headers because when doing error messages more than the actionStringId will be needed
bool actonDataHub_c::areCheckActionDependenciesValid_f(textCompilation_c* errorsPtr_par) const
{
    bool areValidTmp(true);
    for (size_t i = 0; i < rowToActionDataId_pri.size(); ++i)
    {
         auto actionDataIdTmp(rowToActionDataId_pri.at(i));
         action_c* const actionPtrTmp_con(action_ptr_f(actionDataIdTmp));

         for (size_t j = 0; j < actionPtrTmp_con->checkDataHub_f().size_f(); ++j)
         {
             auto checkDataIdTmp(actionPtrTmp_con->checkDataHub_f().rowToCheckDataId_f(j));
             check_c* const checkPtrTmp_con(actionPtrTmp_con->checkDataHub_f().check_ptr_f(checkDataIdTmp));

             QString checkActionStringIdToExamineTmp;
             int_fast64_t checkIdTmp(0);
             if (checkPtrTmp_con->type_f() == checkType_ec::actionFinished)
             {
                 actionFinishedCheck_c* actionFinishedCheckPtrTmp(static_cast<actionFinishedCheck_c*>(checkPtrTmp_con));
                 checkActionStringIdToExamineTmp = actionFinishedCheckPtrTmp->actionStringId_f();
                 checkIdTmp = actionFinishedCheckPtrTmp->id_f();
             }

             if (checkPtrTmp_con->type_f() == checkType_ec::actionStartedExecuting)
             {
                 actionStartedExecutingCheck_c* actionStartedExecutingCheckPtrTmp(static_cast<actionStartedExecutingCheck_c*>(checkPtrTmp_con));
                 checkActionStringIdToExamineTmp = actionStartedExecutingCheckPtrTmp->actionStringId_f();
                 checkIdTmp = actionStartedExecutingCheckPtrTmp->id_f();
             }

             if (checkDataIdTmp > 0)
             {
                 auto actionIdToExamineTmp(actionDataStringIdToActionDataId_f(checkActionStringIdToExamineTmp));
                 action_c* actionPtrToExamineTmp(action_ptr_f(actionIdToExamineTmp));

                 if (actionPtrToExamineTmp->enabled_f())
                 {
                     //good
                 }
                 else
                 {
                     text_c errorTmp({"Check id {0}, from action id {1}, requires action id {2} to be enabled",
                                      checkIdTmp
                                      , actionDataIdTmp
                                      , actionIdToExamineTmp}
                                     );
                     APPENDTEXTPTR(errorsPtr_par, errorTmp)
                     areValidTmp = false;
                 }

                 //it's not necessary to check if the dependant action is valid because if it must enabled it will be validated then
                 //and it will be validated again if it's going to be executed when the overall execution starts
             }
         }

         if (actionPtrTmp_con->type_f() == actionType_ec::folderChangeReaction)
         {
             folderChangeReactionAction_c* actionFinishedCheckPtrTmp(static_cast<folderChangeReactionAction_c*>(actionPtrTmp_con));
             QString reactionStringIdTmp(actionFinishedCheckPtrTmp->reactionActionStringId_f());

             auto actionIdToExamineTmp(actionDataStringIdToActionDataId_f(reactionStringIdTmp));
             action_c* actionPtrToExamineTmp(action_ptr_f(actionIdToExamineTmp));

             if (actionPtrToExamineTmp->enabled_f())
             {
                 text_c errorTmp({"Action id {0} requires action id {1} to be disabled"
                                  , actionDataIdTmp
                                  , actionIdToExamineTmp}
                                 );
                 APPENDTEXTPTR(errorsPtr_par, errorTmp)
                 areValidTmp = false;
             }
             else
             {
                 //good
             }

             if (actionPtrToExamineTmp->isFieldsActionValid_f())
             {
                 //good
             }
             else
             {
                 text_c errorTmp({"Action id {0} required by action id {1} is not valid"
                                  , actionDataIdTmp
                                  , actionIdToExamineTmp}
                                 );
                 APPENDTEXTPTR(errorsPtr_par, errorTmp)
                 areValidTmp = false;
             }
         }
    }
    return areValidTmp;
}

//actonDataHub_c* actonDataHub_ptr_ext = nullptr;
//actonDataHub_c::actonDataHubQO_c::actonDataHubQO_c(actonDataHub_c* test)
//    : parent_pri(test)
//{}

