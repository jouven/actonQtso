#include "checksDataHub.hpp"

#include "checkData.hpp"
#include "checkDataExecutionResult.hpp"
#include "actonDataHub.hpp"

#include "essentialQtso/macros.hpp"

#include "comuso/practicalTemplates.hpp"

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

bool checksDataHub_c::moveCheckRow_f(
    const int sourceRow_par_con
    , const int destinationRow_par_con)
{
    bool resultTmp(false);

    int_fast64_t checkDataIdTmp(rowToCheckDataId_f(sourceRow_par_con));
    //qInfo() << "actionDataIdTmp " << actionDataIdTmp << endl;
    if (checkDataIdTmp > 0)
    {
        //save the actionData stuff that is going to get moved
        //no ref because... first it's removed (removeActionDataUsingRow_f)
        //so copy
        check_c* checkCopyTmp(check_ptr_f(checkDataIdTmp));

        //qInfo() << "sourceActionDataTmp.id_f() " << sourceActionDataTmp.id_f() << endl;

        //remove it using source row
        bool removeResult(removeCheckUsingRow_f(sourceRow_par_con, checkDataIdTmp));

        if (removeResult)
        {
            //insert it in the destination row
            resultTmp = insertCheck_f(checkCopyTmp, destinationRow_par_con);
        }
        else
        {
            //otherwise there would be duplicates
        }
    }
    MACRO_ADDACTONQTSOLOG("Check moved? " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
    return resultTmp;
}

checksDataHubProxyQObj_c* checksDataHub_c::proxyQObj_f()
{
    if (proxyQObj_pri == nullptr)
    {
        proxyQObj_pri = new checksDataHubProxyQObj_c;
    }
    return proxyQObj_pri;
}

void checksDataHub_c::executeCheckDataRows_f(std::vector<int> rows_par)
{
	MACRO_ADDACTONQTSOLOG("Execute Checks from rows", logItem_c::type_ec::debug);
	if (executingChecks_pri)
	{
		//do nothing if already executing
	}
	else
	{
		bool somethingIsExecuting(false);
		checksToRun_pri.clear();
		if (rows_par.empty())
		{
			for (int i = 0, l = checkDataIdToRow_pri.size(); i < l; ++i)
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
			int_fast64_t checkDataIdTmp(rowToCheckDataId_f(rows_ite_con));
			if (checkDataIdTmp not_eq -1)
			{
				check_c* checkPtrTmp(check_ptr_f(checkDataIdTmp));
				if (checkPtrTmp->isExecuting_f())
				{
					MACRO_ADDACTONQTSOLOG("Something is executing", logItem_c::type_ec::warning);
					somethingIsExecuting = true;
					break;
				}
				checksToRun_pri.emplace_back(checkPtrTmp);
			}
		}
		if (not somethingIsExecuting)
		{
			executeChecks_f();
		}
	}
}

bool checksDataHub_c::executingChecks_f() const
{
	return executingChecks_pri;
}

bool checksDataHub_c::stoppingChecksExecution_f() const
{
	return stoppingChecksExecution_pri;
}

void checksDataHub_c::stopExecutingChecks_f()
{
    if (executingChecks_pri and not stoppingChecksExecution_pri)
    {
        stoppingChecksExecution_pri = true;
        proxyQObj_pri->stoppingChecksExecution_signal();
        for (check_c* check_ite_ptr : checksToRun_pri)
        {
            //revisar stop de checks, perque stop de checks es simplement para sense sopcio de resumir
            check_ite_ptr->stopExecution_f();
        }
    }
    else
    {
        MACRO_ADDACTONQTSOLOG("Stopping execution while nothing is executing", logItem_c::type_ec::debug);
    }
}

void checksDataHub_c::executeNextSeqCheck_f()
{
    MACRO_ADDACTONQTSOLOG("Execute next check", logItem_c::type_ec::debug);
    if (stoppingChecksExecution_pri)
    {

    }
    else
    {
        check_c* checkDataTmp(checksToRunSeq_pri.front());
        checksToRunSeq_pri.pop_front();
        //if there are >0 checks to run, make it so when checkDataTmp finishes,
        //calls this function again to run the next one
        if (checksToRunSeq_pri.size() > 0)
        {
            checkDataExecutionResult_c* checkDataExecutionResultTmp_ptr(checkDataTmp->createGetCheckDataExecutionResult_ptr_f());

            //explanation:
            //1 checksDataHub_c is not qobject
            //2 there is more than one signal connected to checkDataExecutionResult_c::finished_signal
            //  using lambdas
            //3 given 1 and 2 disconnect can't be used
            //so this "hack" is used to make it work
            //the other connect that uses this "hack" is in executeChecks_f
            QObject* discObj(new QObject(checkDataExecutionResultTmp_ptr));
            QObject::connect(
                        checkDataExecutionResultTmp_ptr
                        , &checkDataExecutionResult_c::finished_signal
                        , discObj
                        //, std::bind(&checksDataHub_c::executeNextSeqCheck_f, this)
            , [this, discObj]()
            {
                discObj->deleteLater();
                executeNextSeqCheck_f();
            }
            );
        }
        checkDataTmp->tryExecute_f();
//        threadedFunction_c* threadedFunction_ptr = new threadedFunction_c(std::bind(&checkData_c::tryExecute_f, checkDataTmp), true);
//        QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &QObject::deleteLater);
//        QObject::connect(checkDataExecutionResultTmp_ptr, &checkDataExecutionResult_c::finished_signal, threadedFunction_ptr, &threadedFunction_c::quit);

//        threadedFunction_ptr->start();
    }
}

checksDataHub_c::checksDataHub_c(action_c* parentAction_par)
    : parentAction_pri(parentAction_par)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub constructed", logItem_c::type_ec::debug);
}

checksDataHub_c::checksDataHub_c(const checksDataHub_c& from_par_con)
    : checkDataIdToRow_pri(from_par_con.checkDataIdToRow_pri)
    , rowToCheckDataId_pri(from_par_con.rowToCheckDataId_pri)
    , checkDataIdToCheckUMap_pri(from_par_con.checkDataIdToCheckUMap_pri)
    , parentAction_pri(from_par_con.parentAction_pri)
{
    //setParentAction_f(parentAction_pri);
    MACRO_ADDACTONQTSOLOG("checksDataHub copied (const)", logItem_c::type_ec::debug);
}

checksDataHub_c::checksDataHub_c(checksDataHub_c& from_par)
    : checkDataIdToRow_pri(from_par.checkDataIdToRow_pri)
    , rowToCheckDataId_pri(from_par.rowToCheckDataId_pri)
    , checkDataIdToCheckUMap_pri(from_par.checkDataIdToCheckUMap_pri)
    , parentAction_pri(from_par.parentAction_pri)
{
    //setParentAction_f(parentAction_pri);
    MACRO_ADDACTONQTSOLOG("checksDataHub copied (no const)", logItem_c::type_ec::debug);
}

checksDataHub_c& checksDataHub_c::operator=(const checksDataHub_c& from_par_con)
{
    checkDataIdToRow_pri = from_par_con.checkDataIdToRow_pri;
    rowToCheckDataId_pri = from_par_con.rowToCheckDataId_pri;
    checkDataIdToCheckUMap_pri = from_par_con.checkDataIdToCheckUMap_pri;
    parentAction_pri = from_par_con.parentAction_pri;

    //setParentAction_f(parentAction_pri);

    if (proxyQObj_pri not_eq nullptr)
    {
        proxyQObj_pri->deleteLater();
    }
    MACRO_ADDACTONQTSOLOG("checksDataHub assigned= (const)", logItem_c::type_ec::debug);
    return *this;
}

checksDataHub_c& checksDataHub_c::operator=(checksDataHub_c& from_par_con)
{
    checkDataIdToRow_pri = from_par_con.checkDataIdToRow_pri;
    rowToCheckDataId_pri = from_par_con.rowToCheckDataId_pri;
    checkDataIdToCheckUMap_pri = from_par_con.checkDataIdToCheckUMap_pri;
    parentAction_pri = from_par_con.parentAction_pri;

    //setParentAction_f(parentAction_pri);

    if (proxyQObj_pri not_eq nullptr)
    {
        proxyQObj_pri->deleteLater();
    }
    MACRO_ADDACTONQTSOLOG("checksDataHub assigned= (no const)", logItem_c::type_ec::debug);
    return *this;
}

checksDataHub_c& checksDataHub_c::operator=(checksDataHub_c&& from_par) noexcept
{
    checkDataIdToRow_pri = std::move(from_par.checkDataIdToRow_pri);
    rowToCheckDataId_pri = std::move(from_par.rowToCheckDataId_pri);
    checkDataIdToCheckUMap_pri = std::move(from_par.checkDataIdToCheckUMap_pri);
    parentAction_pri = from_par.parentAction_pri;
    proxyQObj_pri = from_par.proxyQObj_pri;
    //lastRunChecks_pri = std::move(from_par.lastRunChecks_pri);

    //setParentAction_f(parentAction_pri);

    from_par.parentAction_pri = nullptr;
    from_par.proxyQObj_pri = nullptr;
    MACRO_ADDACTONQTSOLOG("checksDataHub assigned-moved", logItem_c::type_ec::debug);
    return *this;
}

checksDataHub_c::checksDataHub_c(checksDataHub_c&& from_par) noexcept
    : checkDataIdToRow_pri(std::move(from_par.checkDataIdToRow_pri))
    , rowToCheckDataId_pri(std::move(from_par.rowToCheckDataId_pri))
    , checkDataIdToCheckUMap_pri(std::move(from_par.checkDataIdToCheckUMap_pri))
    , parentAction_pri(from_par.parentAction_pri)
    , proxyQObj_pri(from_par.proxyQObj_pri)
    //, lastRunChecks_pri(std::move(from_par.lastRunChecks_pri))
{
    //setParentAction_f(parentAction_pri);

    from_par.parentAction_pri = nullptr;
    from_par.proxyQObj_pri = nullptr;
    MACRO_ADDACTONQTSOLOG("checksDataHub constructed-moved", logItem_c::type_ec::debug);
}

void checksDataHub_c::setParentAction_f(action_c* parentAction_par)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub set parent action, parentAction is nullptr? " +  QSTRINGBOOL(parentAction_par not_eq nullptr), logItem_c::type_ec::debug);
    //if it's null or (it's not and it's not running)
    if (parentAction_pri == nullptr or not parentAction_pri->isExecuting_f())
    {
        parentAction_pri = parentAction_par;
        for (std::pair<const int_fast64_t, check_c*>& pair_ite : checkDataIdToCheckUMap_pri)
        {
            pair_ite.second->setParentAction_f(parentAction_par);
        }
    }
}

bool checksDataHub_c::checksExecutionFinished_f() const
{
    return checksExecutionFinished_pri;
}

action_c* checksDataHub_c::parentAction_f() const
{
    return parentAction_pri;
}

int_fast32_t checksDataHub_c::updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub update stringId dependencies, old: " + oldStringId_par_con + " new: " + newStringId_par_con, logItem_c::type_ec::debug);
    int_fast32_t updateCountTmp(0);
    for (std::pair<const int_fast64_t, check_c*>& pair_ite : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite.second->updateStringIdDependencies_f(newStringId_par_con, oldStringId_par_con))
        {
            updateCountTmp = updateCountTmp + 1;
        }
    }
    return updateCountTmp;
}

bool checksDataHub_c::hasStringIdAnyDependency_f(const QString& stringId_par_con) const
{
    bool resultTmp(false);
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite_con.second->hasStringIdAnyDependency_f(stringId_par_con))
        {
            resultTmp = true;
            break;
        }
    }
    return resultTmp;
}

int_fast32_t checksDataHub_c::updateStringTriggerParserDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub update string trigger parser dependencies, old: " + oldStringTrigger_par_con + " new: " + newStringTrigger_par_con, logItem_c::type_ec::debug);
    int_fast32_t updateCountTmp(0);
    for (std::pair<const int_fast64_t, check_c*>& pair_ite : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite.second->updateStringTriggerDependecies_f(newStringTrigger_par_con, oldStringTrigger_par_con))
        {
            updateCountTmp = updateCountTmp + 1;
        }
    }
    return updateCountTmp;
}

bool checksDataHub_c::hasStringTriggerAnyDependency_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par) const
{
    bool resultTmp(false);
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite_con.second == objectToIgnore_par)
        {
            continue;
        }
        if (pair_ite_con.second->hasStringTriggerAnyDependency_f(stringTrigger_par_con))
        {
            resultTmp = true;
            break;
        }
    }
    return resultTmp;
}

std::vector<QString> checksDataHub_c::stringTriggersInUseByChecks_f() const
{
    std::vector<QString> resultTmp;
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        std::vector<QString> vectorTmp(pair_ite_con.second->stringTriggersInUse_f());
        resultTmp.insert(resultTmp.end(), vectorTmp.begin(), vectorTmp.end());
    }
    return resultTmp;
}

void checksDataHub_c::executeChecks_f()
{
    MACRO_ADDACTONQTSOLOG("Execute checks, checksToRun_pri.empty()? " +  QSTRINGBOOL(checksToRun_pri.empty()), logItem_c::type_ec::debug);
    if (not checksToRun_pri.empty())
    {
        executingChecks_pri = true;
        checksToRunSeq_pri.clear();
        proxyQObj_pri->checksExecutionStarted_signal();
        for (check_c* check_ite_ptr : checksToRun_pri)
        {
            checkDataExecutionResult_c* checkDataExecutionResultPtrTmp(check_ite_ptr->createGetCheckDataExecutionResult_ptr_f());
            QObject* discObj(new QObject(checkDataExecutionResultPtrTmp));
            QObject::connect(
                        checkDataExecutionResultPtrTmp
                        , &checkDataExecutionResult_c::finished_signal
                        , discObj
            , [this, discObj]()
            {
                discObj->deleteLater();
                verifyExecutionFinished_f();
            }
            );

            if (check_ite_ptr->threaded_f())
            {
                check_ite_ptr->tryExecute_f();
            }
            else
            {
                checksToRunSeq_pri.emplace_back(check_ite_ptr);
            }
        }
        if (not checksToRunSeq_pri.empty())
        {
            executeNextSeqCheck_f();
        }
    }
}

void checksDataHub_c::verifyExecutionFinished_f()
{
    bool allFinishedTmp(true);
    bool somethingStoppedTmp(false);
    for (check_c* check_ite_ptr : checksToRun_pri)
    {
        if (not check_ite_ptr->checkDataExecutionResult_ptr_f()->finished_f())
        {
            allFinishedTmp = false;
            break;
        }
        if (check_ite_ptr->checkDataExecutionResult_ptr_f()->lastState_f() == checkExecutionState_ec::stoppedByUser)
        {
            somethingStoppedTmp = true;
        }
    }
    if (allFinishedTmp)
    {
        MACRO_ADDACTONQTSOLOG("All checks finished executing", logItem_c::type_ec::info);

        if (stoppingChecksExecution_pri)
        {
            stoppingChecksExecution_pri = false;
            MACRO_ADDACTONQTSOLOG("Execution finished, after stopping", logItem_c::type_ec::info);
            if (somethingStoppedTmp)
            {
                MACRO_ADDACTONQTSOLOG("Execution finished, one or more checks were stopped", logItem_c::type_ec::info);
                checksExecutionStopped_pri = true;
                proxyQObj_pri->checksExecutionStopped_signal();
            }
        }
        else
        {
            //no looping like the acions
//            if (executionOptions_pri.loopExecution_f())
//            {
//                executeChecks_f(true);
//            }
        }

        executingChecks_pri = false;
        checksExecutionFinished_pri = true;
        proxyQObj_pri->checksExecutionFinished_signal(checksToRun_pri);
    }
}

checksDataHub_c::~checksDataHub_c()
{
    MACRO_ADDACTONQTSOLOG("CheckDataHub destroyed", logItem_c::type_ec::debug);
    if (proxyQObj_pri == nullptr)
    {
    }
    else
    {
        proxyQObj_pri->deleteLater();
    }
}

bool checksDataHub_c::validRow_f(const int row_par_con) const
{
    return row_par_con >= 0 or row_par_con <= size_f();
}

bool checksDataHub_c::insertCheck_f(
        check_c* checkPtr_par
        , const int row_par_con)
{
    bool resultTmp(false);
    while (true)
    {
        if (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        {
            break;
        }
        if (not validRow_f(row_par_con))
        {
            break;
        }

        checkPtr_par->setParentAction_f(parentAction_pri);

        //insert actionId - actionData object mapping
        checkDataIdToCheckUMap_pri.emplace(checkPtr_par->id_f(), checkPtr_par);

        //new row goes in the last position
        if (row_par_con == size_f())
        {
            //insert the mappings (since it's the last row no need to touch anything else)
            checkDataIdToRow_pri.emplace(checkPtr_par->id_f(), row_par_con);
            rowToCheckDataId_pri.emplace(row_par_con, checkPtr_par->id_f());
            resultTmp = true;
            break;
        }

        //"else"
        //new row goes in between existing ones (moving them one position and replacing one)
        //create a row at the end and move all the rows before one index
        auto lastRowIndexTmp(size_f());
        {
            //get the previous to last
            auto movedCheckIdTmp(rowToCheckDataId_pri.at(lastRowIndexTmp - 1));
            //insert the actionId from the previous to last into the last
            rowToCheckDataId_pri.emplace(lastRowIndexTmp, movedCheckIdTmp);
            //update the actionId - row mapping
            checkDataIdToRow_pri.at(movedCheckIdTmp) = lastRowIndexTmp;

            lastRowIndexTmp = lastRowIndexTmp - 1;
        }
        //move all the rows from the last to the target row, to row+1 (to make space for the new one)
        while (row_par_con < lastRowIndexTmp)
        {
            //qInfo() << "newRowIndexTmp " << newRowIndexTmp << endl;
            //qInfo() << "row_par_con " << row_par_con << endl;
            //get the actionId of the previous row
            auto movedCheckIdTmp(rowToCheckDataId_pri.at(lastRowIndexTmp - 1));
            //move the mappings to "row+1"
            //insert the actionId to the next row
            rowToCheckDataId_pri.at(lastRowIndexTmp) = movedCheckIdTmp;
            //update the actionId - row mapping with the next row
            checkDataIdToRow_pri.at(movedCheckIdTmp) = lastRowIndexTmp;

            lastRowIndexTmp = lastRowIndexTmp - 1;
        }

        //since all the rows (mappings), target and below, have been moved, it's only necessary to insert
        //the new mapping in the desired row
        checkDataIdToRow_pri.emplace(checkPtr_par->id_f(), row_par_con);
        rowToCheckDataId_pri.at(row_par_con) = checkPtr_par->id_f();
        resultTmp = true;

        break;
    }
    MACRO_ADDACTONQTSOLOG("Check inserted? " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
    return resultTmp;
}


int checksDataHub_c::checkDataIdToRow_f(const int_fast64_t checkDataId_par_con) const
{
    int resultTmp(-1);
    auto findResult(checkDataIdToRow_pri.find(checkDataId_par_con));
    if (findResult not_eq checkDataIdToRow_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

int_fast64_t checksDataHub_c::rowToCheckDataId_f(const int row_par_con) const
{
    int_fast64_t resultTmp(-1);
    auto findResult(rowToCheckDataId_pri.find(row_par_con));
    if (findResult not_eq rowToCheckDataId_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

int_fast32_t checksDataHub_c::size_f() const
{
    return rowToCheckDataId_pri.size();
}

check_c* checksDataHub_c::check_ptr_f(const int_fast64_t checkDataId_par_con)
{
    check_c* checkPtrTmp(Q_NULLPTR);
    auto findResult(checkDataIdToCheckUMap_pri.find(checkDataId_par_con));
    if (findResult not_eq checkDataIdToCheckUMap_pri.end())
    {
        checkPtrTmp = findResult->second;
    }
    return checkPtrTmp;
}

check_c* checksDataHub_c::check_ptr_f(const int_fast64_t checkDataId_par_con) const
{
    check_c* checkPtrTmp(Q_NULLPTR);
    auto findResult(checkDataIdToCheckUMap_pri.find(checkDataId_par_con));
    if (findResult not_eq checkDataIdToCheckUMap_pri.end())
    {
        checkPtrTmp = findResult->second;
    }
    return checkPtrTmp;
}

//checkData_c checksDataHub_c::checkData_f(
//        const int_fast64_t checkDataId_par_con
//        , bool* found_ptr
//) const
//{
//    auto findResult(checkDataIdToCheckDataUMap_pri.find(checkDataId_par_con));
//    bool foundTmp(findResult not_eq checkDataIdToCheckDataUMap_pri.cend());
//    if (found_ptr not_eq nullptr)
//    {
//        *found_ptr = foundTmp;
//    }
//    return (foundTmp ? findResult->second : checkData_c());
//}

//void checksDataHub_c::clearAllCheckData_f()
//{
//    MACRO_ADDACTONQTSOLOG("Clear all checkDataHub data", logItem_c::type_ec::debug);
//    if (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
//    {

//    }
//    else
//    {
//        lastRunChecks_pri.clear();
//        checkDataIdToRow_pri.clear();
//        rowToCheckDataId_pri.clear();
//        checkDataIdToCheckDataUMap_pri.clear();
//    }
//}

bool checksDataHub_c::removeCheckUsingRow_f(
        const int row_par_con
        , const int_fast64_t checkDataId_par_con)
{
    bool resultTmp(false);
    while (true)
    {
        if (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        {
            break;
        }
        //invalid index
        if (row_par_con < 0 or row_par_con >= size_f())
        {
            break;
        }

        int_fast64_t checkIdToRemoveTmp(checkDataId_par_con);
        //if it's not set, get it
        if (checkIdToRemoveTmp == 0)
        {
            checkIdToRemoveTmp = rowToCheckDataId_f(row_par_con);
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
            auto movedCheckIdTmp(rowToCheckDataId_pri.at(removedRowIndexTmp + 1));
            //move the mappings to the row being replaced
            //replace the actionId with the one from the next row
            rowToCheckDataId_pri.at(removedRowIndexTmp) = movedCheckIdTmp;
            //replace the actionId - row mapping with the next row
            checkDataIdToRow_pri.at(movedCheckIdTmp) = removedRowIndexTmp;

            removedRowIndexTmp = removedRowIndexTmp + 1;
        }

        //remove the last "row" - actionId mapping, since it has been moved (but the last one doesn't get replaced by the next one)
        rowToCheckDataId_pri.erase(removedRowIndexTmp);
        //remove the mapping actionId - row mapping of the actionId being deleted
        checkDataIdToRow_pri.erase(checkIdToRemoveTmp);

        //remove actionId - object mapping (removing the object from the data "container")
        checkDataIdToCheckUMap_pri.erase(checkIdToRemoveTmp);

        //lastRunChecks_pri.clear();

        resultTmp = true;
        break;
    }
    MACRO_ADDACTONQTSOLOG("Check in row " + QString::number(row_par_con) + " removed: " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
    return resultTmp;
}

bool checksDataHub_c::removeCheckUsingId_f(const int_fast64_t checkDataId_par_con)
{
    bool resultTmp(false);
    auto rowFindResult(checkDataIdToRow_f(checkDataId_par_con));
    if (rowFindResult not_eq -1)
    {
        resultTmp = removeCheckUsingRow_f(rowFindResult);
    }
    return resultTmp;
}

