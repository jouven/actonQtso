#include "actonDataHub.hpp"

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

#include "checkMappings/checkExecutionStates.hpp"
#include "actionDataExecutionResult.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "essentialQtso/macros.hpp"

#include "comuso/practicalTemplates.hpp"

#include <QCoreApplication>

#include <functional>

//the qapp instance, QApplication qtapp(argc, argv); in the main,
//can dtor this when it goes out of the scope
actonDataHubProxyQObj_c::actonDataHubProxyQObj_c()
    : QObject(QCoreApplication::instance())
{}

bool actonDataHub_c::moveRowActionData_f(
    const int sourceRow_par_con
    , const int destinationRow_par_con)
{
    bool resultTmp(false);
    int_fast64_t actionDataIdTmp(rowToActionDataId_f(sourceRow_par_con));
    //qInfo() << "actionDataIdTmp " << actionDataIdTmp << endl;
    if (actionDataIdTmp > 0)
    {
        //save the actionData stuff that is going to get moved
        //no ref because... first it's removed (removeActionDataUsingRow_f)
        //so copy
        actionData_c actionDataCopyTmp(*(actionData_ptr_f(actionDataIdTmp)));

        //qInfo() << "sourceActionDataTmp.id_f() " << sourceActionDataTmp.id_f() << endl;

        //remove it, this will always work here otherwise it wouldn't fetch the actionDataId to begin with
        bool removeResult(removeActionDataUsingRow_f(sourceRow_par_con, actionDataIdTmp));

        if (removeResult)
        {
            //insert it in the destination row, this can fail if the destination row is not "in range"
            resultTmp = insertActionData_f(actionDataCopyTmp, destinationRow_par_con);
        }
        else
        {
            //otherwise there would be duplicates
        }
    }
    MACRO_ADDACTONQTSOLOG("Action moved? " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
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

void actonDataHub_c::executeActionDataRows_f(std::vector<int> rows_par)
{
	MACRO_ADDACTONQTSOLOG("Execute Actions from rows", logItem_c::type_ec::debug);
	if (executingActions_pri)
	{
		//do nothing if already executing
	}
	else
	{
		bool somethingIsExecuting(false);
		actionsToRun_pri.clear();
		if (rows_par.empty())
		{
			for (int i = 0, l = actionDataIdToRow_pri.size(); i < l; ++i)
			{
				rows_par.emplace_back(i);
			}
		}
		else
		{
			removeNegative_ft(rows_par);
			removeDuplicates_ft(rows_par);
		}
		for (const int rows_ite_con : rows_par)
		{
			int_fast64_t actionIdTmp(rowToActionDataId_f(rows_ite_con));
			if (actionIdTmp != -1)
			{
				actionData_c* actionDataPtrTmp(actionData_ptr_f(actionIdTmp));
				if (actionDataPtrTmp->isExecuting_f())
				{
					MACRO_ADDACTONQTSOLOG("Something is executing", logItem_c::type_ec::warning);
					somethingIsExecuting = true;
					break;
				}
				actionsToRun_pri.emplace_back(actionDataPtrTmp);
			}
		}
		if (not somethingIsExecuting)
		{
			executeActions_f();
		}
	}
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
    MACRO_ADDACTONQTSOLOG("Execute actions, loop: " + QSTRINGBOOL(loop_par_con), logItem_c::type_ec::debug);
    if (not actionsToRun_pri.empty())
    {
        actionsExecutionStopped_pri = false;
        actionsExecutionKilled_pri = false;
        executingActions_pri = true;
        proxyQObj_pri->actionsExecutionStarted_signal();
        for (actionData_c* actionData_ite_ptr : actionsToRun_pri)
        {
            if (loop_par_con)
            {
                //don't connect again
            }
            else
            {
                actionDataExecutionResult_c* actionDataExecutionResultTmp(actionData_ite_ptr->createGetActionDataExecutionResult_ptr_f());
                QObject::connect(
                            actionDataExecutionResultTmp
                            , &actionDataExecutionResult_c::finished_signal
                            , proxyQObj_pri
                , [this](actionData_c* actionDataPtr_par)
                {
                    QObject::disconnect(actionDataPtr_par->actionDataExecutionResult_ptr_f(), &actionDataExecutionResult_c::finished_signal, proxyQObj_pri, nullptr);
                    verifyExecutionFinished_f(actionDataPtr_par);
                }
                );
                QObject::connect(
                            actionDataExecutionResultTmp
                            , &actionDataExecutionResult_c::killing_signal
                            , proxyQObj_pri
                            //, std::bind(&actonDataHub_c::killingStarted_f, this)
                            , [this](actionData_c* actionDataPtr_par)
                            {
                                QObject::disconnect(actionDataPtr_par->actionDataExecutionResult_ptr_f(), &actionDataExecutionResult_c::killing_signal, proxyQObj_pri, nullptr);
                                killingStarted_f();
                            }
                );
            }

            actionData_ite_ptr->tryExecute_f();
        }
    }
}

void actonDataHub_c::verifyExecutionFinished_f(actionData_c* actionDataPtr_par)
{
    bool allFinishedTmp(true);
    bool somethingStoppedTmp(false);
    bool somethingKilledTmp(false);
    for (actionData_c* actionData_ite_ptr : actionsToRun_pri)
    {
        if (not actionData_ite_ptr->actionDataExecutionResult_ptr_f()->finished_f())
        {
            allFinishedTmp = false;
            break;
        }
        if (actionData_ite_ptr->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::stoppedByUser)
        {
            somethingStoppedTmp = true;
        }
        if (actionData_ite_ptr->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::killedByUser)
        {
            somethingKilledTmp = true;
        }
    }
    while (true)
    {
        if (allFinishedTmp)
        {
            MACRO_ADDACTONQTSOLOG("All actions finished executing", logItem_c::type_ec::info);
            if (stoppingActionsExecution_pri)
            {
                stoppingActionsExecution_pri = false;
                MACRO_ADDACTONQTSOLOG("Execution finished, after stopping", logItem_c::type_ec::info);
                if (somethingStoppedTmp)
                {
                    MACRO_ADDACTONQTSOLOG("Execution finished, one or more actions were stopped", logItem_c::type_ec::info);
                    actionsExecutionStopped_pri = true;
                    proxyQObj_pri->actionsExecutionStopped_signal();
                }
            }

            if (killingActionsExecution_pri)
            {
                MACRO_ADDACTONQTSOLOG("Execution finished, after killing", logItem_c::type_ec::info);
                killingActionsExecution_pri = false;
                if (somethingKilledTmp)
                {
                    MACRO_ADDACTONQTSOLOG("Execution finished, one or more actions were killed", logItem_c::type_ec::info);
                    actionsExecutionKilled_pri = true;
                    proxyQObj_pri->actionsExecutionKilled_signal();
                }
            }

            executingActions_pri = false;
            actionsExecutionFinished_pri = true;
            proxyQObj_pri->actionsExecutionFinished_signal(actionsToRun_pri);

            if (not actionsExecutionKilled_pri and not actionsExecutionStopped_pri)
            {
                //niche case, if the last action "errors", all actions have finished and they can't be stopped,
                //but if that action stopExecutionOnError_f is true, loopExecution can't happen either
                if (actionDataPtr_par->stopExecutionOnError_f() and not actionDataPtr_par->actionDataExecutionResult_ptr_f()->error_f().isEmpty())
                {
                    stopOnThisLoopEnd_pri = true;
                }
                if (executionOptions_pri.loopExecution_f() and not stopOnThisLoopEnd_pri)
                {
                    MACRO_ADDACTONQTSOLOG("Execution finished, looping execution", logItem_c::type_ec::info);
                    executeActions_f(true);
                }
                stopOnThisLoopEnd_pri = false;
            }
        }
        else
        {
            if (actionDataPtr_par->stopExecutionOnError_f()
                and not actionDataPtr_par->actionDataExecutionResult_ptr_f()->error_f().isEmpty())
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
        MACRO_ADDACTONQTSOLOG("Killing execution while already killing", logItem_c::type_ec::info);
    }
    else
    {
        MACRO_ADDACTONQTSOLOG("Killing execution", logItem_c::type_ec::info);
        killingActionsExecution_pri = true;
        proxyQObj_pri->killingActionsExecution_signal();
    }
}

void actonDataHub_c::tryStopExecutingActions_f(const bool killAfterTimeout_par_con)
{
    if (executingActions_pri)
    {
        stoppingActionsExecution_pri = true;
        proxyQObj_pri->stoppingActionsExecution_signal();
        //MACRO_ADDACTONQTSOLOG("Stopping " + QString::number(actionsToRun_pri.size()) + " actions", logItem_c::type_ec::debug);
        for (actionData_c* actionData_ite_ptr : actionsToRun_pri)
        {
            actionData_ite_ptr->tryStopExecution_f(killAfterTimeout_par_con);
        }
    }
    else
    {
        MACRO_ADDACTONQTSOLOG("Stopping execution while nothing is executing", logItem_c::type_ec::debug);
    }
}

void actonDataHub_c::stopWhenLoopFinished_f()
{
    stopOnThisLoopEnd_pri = true;
}

void actonDataHub_c::tryResumeActionsExecution_f()
{
    MACRO_ADDACTONQTSOLOG("Try resume actions execution", logItem_c::type_ec::debug);
    if (actionsExecutionStopped_pri)
    {
        actionsExecutionStopped_pri = false;
        actionsExecutionKilled_pri = false;
        bool stateSetTmp(false);
        for (actionData_c* actionData_ite_ptr : actionsToRun_pri)
        {
            if (actionData_ite_ptr->actionDataExecutionResult_ptr_f() not_eq nullptr and actionData_ite_ptr->actionDataExecutionResult_ptr_f()->lastState_f() == actionExecutionState_ec::stoppedByUser)
            {
                actionData_ite_ptr->tryExecute_f();
                if (not stateSetTmp)
                {
                    MACRO_ADDACTONQTSOLOG("An action resumed", logItem_c::type_ec::info);
                    stateSetTmp = true;
                    executingActions_pri = true;
                    proxyQObj_pri->actionsExecutionStarted_signal();
                }
            }
        }
    }
}

bool actonDataHub_c::executingActions_f() const
{
    return executingActions_pri;
}

//bool actonDataHub_c::isAnyActionExecutingChecks_f() const
//{
//    bool resultTmp(false);
//    MACRO_ADDACTONQTSOLOG("Check if any action executing checks", logItem_c::type_ec::debug);
//    for (const std::pair<const int_fast64_t, actionData_c>& actionDataIdObjectPair_ite_con : actionDataIdToActionDataUMap_pri)
//    {
//        if (actionDataIdObjectPair_ite_con.second.checkDataHub_f().executingChecks_f())
//        {
//            MACRO_ADDACTONQTSOLOG("An action is executing checks", logItem_c::type_ec::info);
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

QString actonDataHub_c::actionStringIdExecutingChecks_f() const
{
    QString resultTmp;
    for (const std::pair<const int_fast64_t, actionData_c>& actionDataIdObjectPair_ite_con : actionDataIdToActionDataUMap_pri)
    {
        if (actionDataIdObjectPair_ite_con.second.checkDataHub_f().executingChecks_f())
        {
            MACRO_ADDACTONQTSOLOG("An action is executing checks, action stringId: " + actionDataIdObjectPair_ite_con.second.stringId_f(), logItem_c::type_ec::debug);
            resultTmp = actionDataIdObjectPair_ite_con.second.stringId_f();
            break;
        }
    }
    return resultTmp;
}

bool actonDataHub_c::stoppingActionsExecution_f() const
{
    return stoppingActionsExecution_pri;
}

bool actonDataHub_c::actionsExecutionStopped_f() const
{
    return actionsExecutionStopped_pri;
}

actonDataHubProxyQObj_c* actonDataHub_c::proxyQObj_f() const
{
    return proxyQObj_pri;
}

int_fast32_t actonDataHub_c::updateStringIdDependencies_f(
        const QString& newStringId_par_con
        , const QString& oldStringId_par_con)
{
    int_fast32_t updateCountTmp(0);
    for (std::pair<const int_fast64_t, actionData_c>& actionDataIdObjectPair_ite : actionDataIdToActionDataUMap_pri)
    {
        updateCountTmp = updateCountTmp + actionDataIdObjectPair_ite.second.updateStringIdDependencies_f(newStringId_par_con, oldStringId_par_con);
    }
    MACRO_ADDACTONQTSOLOG("Updated " + QString::number(updateCountTmp) + " checks with action stringId dependencies", logItem_c::type_ec::debug);
    return updateCountTmp;
}

bool actonDataHub_c::hasStringIdAnyDependency_f(const QString& stringId_par_con) const
{
    bool resultTmp(false);
    for (const std::pair<const int_fast64_t, actionData_c>& actionDataIdObjectPair_ite_con : actionDataIdToActionDataUMap_pri)
    {
        if (actionDataIdObjectPair_ite_con.second.hasStringIdAnyDependency_f(stringId_par_con))
        {
            resultTmp = true;
            break;
        }
    }
    MACRO_ADDACTONQTSOLOG("Action stringId " + stringId_par_con + " has dependencies: " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
    return resultTmp;
}

actonDataHub_c::actonDataHub_c()
    : proxyQObj_pri(new actonDataHubProxyQObj_c)
{
    qRegisterMetaType<actionData_c*>("actionData_c*");
    qRegisterMetaType<checkData_c*>("checkData_c*");
    qRegisterMetaType<actionExecutionState_ec>("actionExecutionState_ec");
    qRegisterMetaType<checkExecutionState_ec>("checkExecutionState_ec");
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
        const actionData_c& obj_par_con
        , const int row_par_con)
{
    bool resultTmp(false);
    while (true)
    {
        if (not validRow_f(row_par_con))
        {
            break;
        }

        if (not validStringId_f(obj_par_con.stringId_f()))
        {
            break;
        }

        //insert actionId - actionData object mapping
        actionDataIdToActionDataUMap_pri.emplace(obj_par_con.id_f(), obj_par_con);

        //insert actionDataStringId - actionDataId mapping
        actionDataStringIdToActionDataId_pri.insert(obj_par_con.stringId_f(), obj_par_con.id_f());

        //new row goes in the last position
        if (row_par_con == size_f())
        {
            //insert the mappings (since it's the last row no need to touch anything else)
            actionDataIdToRow_pri.emplace(obj_par_con.id_f(), row_par_con);
            rowToActionDataId_pri.emplace(row_par_con, obj_par_con.id_f());
            resultTmp = true;
            break;
        }

        //"else"
        //new row goes in between existing ones (moving them one position and replacing one)
        //create a row at the end and move all the rows before one index
        auto lastRowIndexTmp(size_f());
        {
            //get the previous to last
            auto movedActionIdTmp(rowToActionDataId_pri.at(lastRowIndexTmp - 1));
            //insert the actionId from the previous to last into the last
            rowToActionDataId_pri.emplace(lastRowIndexTmp, movedActionIdTmp);
            //update the actionId - row mapping
            actionDataIdToRow_pri.at(movedActionIdTmp) = lastRowIndexTmp;

            lastRowIndexTmp = lastRowIndexTmp - 1;
        }
        //move all the rows from the last to the target row, to row+1 (to make space for the new one)
        while (row_par_con < lastRowIndexTmp)
        {
            //qInfo() << "newRowIndexTmp " << newRowIndexTmp << endl;
            //qInfo() << "row_par_con " << row_par_con << endl;
            //get the actionId of the previous row
            auto movedActionIdTmp(rowToActionDataId_pri.at(lastRowIndexTmp - 1));
            //move the mappings to "row+1"
            //insert the actionId to the next row
            rowToActionDataId_pri.at(lastRowIndexTmp) = movedActionIdTmp;
            //update the actionId - row mapping with the next row
            actionDataIdToRow_pri.at(movedActionIdTmp) = lastRowIndexTmp;

            lastRowIndexTmp = lastRowIndexTmp - 1;
        }

        //since all the rows (mappings), target and below, have been moved, it's only necessary to insert
        //the new mapping in the desired row
        actionDataIdToRow_pri.emplace(obj_par_con.id_f(), row_par_con);
        rowToActionDataId_pri.at(row_par_con) = obj_par_con.id_f();
        resultTmp = true;

        break;
    }
    MACRO_ADDACTONQTSOLOG("Action inserted? " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
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
    auto findResult(actionDataIdToActionDataUMap_pri.find(actionDataId_par_con));
    if (findResult != actionDataIdToActionDataUMap_pri.end())
    {
        resultTmp = findResult->second.stringId_f();
    }
    return resultTmp;
}

int_fast64_t actonDataHub_c::rowToActionDataId_f(const int row_par_con) const
{
    int_fast64_t resultTmp(-1);
    auto findResult(rowToActionDataId_pri.find(row_par_con));
    if (findResult != rowToActionDataId_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

int_fast64_t actonDataHub_c::actionDataStringIdToActionDataId_f(const QString& actionDataStringId_par_con) const
{
    int_fast64_t resultTmp(-1);
    QHash<QString, int_fast64_t>::const_iterator findResult(actionDataStringIdToActionDataId_pri.find(actionDataStringId_par_con));
    if (findResult != actionDataStringIdToActionDataId_pri.end())
    {
        resultTmp = findResult.value();
    }
    return resultTmp;
}

int_fast32_t actonDataHub_c::size_f() const
{
    return rowToActionDataId_pri.size();
}

actionData_c* actonDataHub_c::actionData_ptr_f(const int_fast64_t actionDataId_par_con)
{
    actionData_c* actionDataTmp_ptr(Q_NULLPTR);
    auto findResult(actionDataIdToActionDataUMap_pri.find(actionDataId_par_con));
    if (findResult != actionDataIdToActionDataUMap_pri.end())
    {
        actionDataTmp_ptr = std::addressof(findResult->second);
    }
    return actionDataTmp_ptr;
}

void actonDataHub_c::clearAllActionData_f()
{
    MACRO_ADDACTONQTSOLOG("Clear all actionDataHub data, executing actions? " + QSTRINGBOOL(executingActions_pri), logItem_c::type_ec::debug);
    if (executingActions_pri)
    {
    }
    else
    {

        actionsExecutionFinished_pri = false;
        stoppingActionsExecution_pri = false;
        actionsExecutionStopped_pri = false;
        killingActionsExecution_pri = false;
        actionsExecutionKilled_pri = false;
        actionsToRun_pri.clear();
        actionDataIdToRow_pri.clear();
        rowToActionDataId_pri.clear();
        actionDataIdToActionDataUMap_pri.clear();
        actionDataStringIdToActionDataId_pri.clear();
    }
}

QStringList actonDataHub_c::actionStringIdList_f(const bool sorted_par_con) const
{
    QStringList resultTmp;
    resultTmp.reserve(actionDataIdToActionDataUMap_pri.size());
    for (const std::pair<const int_fast64_t, actionData_c>& actionDataIdObjectPair_ite_con : actionDataIdToActionDataUMap_pri)
    {
        resultTmp.append(actionDataIdObjectPair_ite_con.second.stringId_f());
    }

    if (sorted_par_con)
    {
        resultTmp.sort();
    }
    return resultTmp;
}

void actonDataHub_c::setExecutionOptions(const executionOptions_c& executionOptions_par_con)
{
    if (executingActions_pri)
    {
    }
    else
    {
        executionOptions_pri = executionOptions_par_con;
    }
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
            //qInfo() << "removedRowIndexTmp " << removedRowIndexTmp << endl;
            //qInfo() << "size_f() " << size_f() << endl;
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
        actionDataIdToActionDataUMap_pri.erase(actionIdToRemoveTmp);
        resultTmp = true;

        break;
    }
    MACRO_ADDACTONQTSOLOG("Action in row " + QString::number(row_par_con) + " removed: " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
    return resultTmp;
}

bool actonDataHub_c::removeActionDataUsingId_f(const int_fast64_t actionDataId_par_con)
{
    bool resultTmp(false);
    auto rowFindResult(actionDataIdToRow_f(actionDataId_par_con));
    if (rowFindResult != -1)
    {
        resultTmp = removeActionDataUsingRow_f(rowFindResult);
    }
    return resultTmp;
}

bool actonDataHub_c::addLogMessage_f(
        const QString& message_par_con
        , const logItem_c::type_ec logType_par_con
        , const QString& sourceFile_par_con
        , const QString& sourceFunction_par_con
        , const int_fast32_t line_par_con
)
{
    bool resultTmp(false);
    if (logDataHub_pri not_eq nullptr)
    {
        resultTmp = logDataHub_pri->addMessage_f(message_par_con, logType_par_con, sourceFile_par_con, sourceFunction_par_con, line_par_con);
    }
    return resultTmp;
}

actonDataHub_c* actonDataHub_ptr_ext = nullptr;
//actonDataHub_c::actonDataHubQO_c::actonDataHubQO_c(actonDataHub_c* test)
//    : parent_pri(test)
//{}

