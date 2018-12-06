#include "checksDataHub.hpp"

#include "actonDataHub.hpp"

#include "checkDataExecutionResult.hpp"

#include "essentialQtso/macros.hpp"

#include "comuso/practicalTemplates.hpp"

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

bool checksDataHub_c::moveRowCheckData_f(
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
        checkData_c checkDataCopyTmp(checkData_f(checkDataIdTmp));

        //qInfo() << "sourceActionDataTmp.id_f() " << sourceActionDataTmp.id_f() << endl;

        //remove it using source row
        bool removeResult(removeCheckDataUsingRow_f(sourceRow_par_con, checkDataIdTmp));

        if (removeResult)
        {
            //insert it in the destination row
            resultTmp = insertCheckData_f(checkDataCopyTmp, destinationRow_par_con);
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
				checkData_c* checkDataPtrTmp(checkData_ptr_f(checkDataIdTmp));
				if (checkDataPtrTmp->isExecuting_f())
				{
					MACRO_ADDACTONQTSOLOG("Something is executing", logItem_c::type_ec::warning);
					somethingIsExecuting = true;
					break;
				}
				checksToRun_pri.emplace_back(checkDataPtrTmp);
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
        for (checkData_c* checkData_ite_ptr : checksToRun_pri)
        {
            //revisar stop de checks, perque stop de checks es simplement para sense sopcio de resumir
            checkData_ite_ptr->stopExecution_f();
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
        checkData_c* checkDataTmp(checksToRunSeq_pri.front());

        checkDataExecutionResult_c* checkDataExecutionResultTmp_ptr(checkDataTmp->createGetCheckDataExecutionResult_ptr_f());
        if (checksToRunSeq_pri.size() > 1)
        {
            QObject::connect(checkDataExecutionResultTmp_ptr, &checkDataExecutionResult_c::finished_signal, proxyQObj_pri, std::bind(&checksDataHub_c::executeNextSeqCheck_f, this));
        }

        checkDataTmp->tryExecute_f();
//        threadedFunction_c* threadedFunction_ptr = new threadedFunction_c(std::bind(&checkData_c::tryExecute_f, checkDataTmp), true);
//        QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &QObject::deleteLater);
//        QObject::connect(checkDataExecutionResultTmp_ptr, &checkDataExecutionResult_c::finished_signal, threadedFunction_ptr, &threadedFunction_c::quit);

//        threadedFunction_ptr->start();

        checksToRunSeq_pri.pop_front();
    }
}

checksDataHub_c::checksDataHub_c(actionData_c* parentAction_par)
    : parentAction_pri(parentAction_par)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub constructed", logItem_c::type_ec::debug);
}

checksDataHub_c::checksDataHub_c(const checksDataHub_c& from_par_con)
    : checkDataIdToRow_pri(from_par_con.checkDataIdToRow_pri)
    , rowToCheckDataId_pri(from_par_con.rowToCheckDataId_pri)
    , checkDataIdToCheckDataUMap_pri(from_par_con.checkDataIdToCheckDataUMap_pri)
    , parentAction_pri(from_par_con.parentAction_pri)
{
    //setParentAction_f(parentAction_pri);
    MACRO_ADDACTONQTSOLOG("checksDataHub copied (const)", logItem_c::type_ec::debug);
}

checksDataHub_c::checksDataHub_c(checksDataHub_c& from_par)
    : checkDataIdToRow_pri(from_par.checkDataIdToRow_pri)
    , rowToCheckDataId_pri(from_par.rowToCheckDataId_pri)
    , checkDataIdToCheckDataUMap_pri(from_par.checkDataIdToCheckDataUMap_pri)
    , parentAction_pri(from_par.parentAction_pri)
{
    //setParentAction_f(parentAction_pri);
    MACRO_ADDACTONQTSOLOG("checksDataHub copied (no const)", logItem_c::type_ec::debug);
}

checksDataHub_c& checksDataHub_c::operator=(const checksDataHub_c& from_par_con)
{
    checkDataIdToRow_pri = from_par_con.checkDataIdToRow_pri;
    rowToCheckDataId_pri = from_par_con.rowToCheckDataId_pri;
    checkDataIdToCheckDataUMap_pri = from_par_con.checkDataIdToCheckDataUMap_pri;
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
    checkDataIdToCheckDataUMap_pri = from_par_con.checkDataIdToCheckDataUMap_pri;
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
    checkDataIdToCheckDataUMap_pri = std::move(from_par.checkDataIdToCheckDataUMap_pri);
    parentAction_pri = from_par.parentAction_pri;
    proxyQObj_pri = from_par.proxyQObj_pri;
    lastRunChecks_pri = std::move(from_par.lastRunChecks_pri);

    //setParentAction_f(parentAction_pri);

    from_par.parentAction_pri = nullptr;
    from_par.proxyQObj_pri = nullptr;
    MACRO_ADDACTONQTSOLOG("checksDataHub assigned-moved", logItem_c::type_ec::debug);
    return *this;
}

checksDataHub_c::checksDataHub_c(checksDataHub_c&& from_par) noexcept
    : checkDataIdToRow_pri(std::move(from_par.checkDataIdToRow_pri))
    , rowToCheckDataId_pri(std::move(from_par.rowToCheckDataId_pri))
    , checkDataIdToCheckDataUMap_pri(std::move(from_par.checkDataIdToCheckDataUMap_pri))
    , parentAction_pri(from_par.parentAction_pri)
    , proxyQObj_pri(from_par.proxyQObj_pri)
    , lastRunChecks_pri(std::move(from_par.lastRunChecks_pri))
{
    //setParentAction_f(parentAction_pri);

    from_par.parentAction_pri = nullptr;
    from_par.proxyQObj_pri = nullptr;
    MACRO_ADDACTONQTSOLOG("checksDataHub constructed-moved", logItem_c::type_ec::debug);
}

void checksDataHub_c::setParentAction_f(actionData_c* parentAction_par)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub set parent action, parentAction is nullptr? " +  QSTRINGBOOL(parentAction_par not_eq nullptr), logItem_c::type_ec::debug);
    //if it's null or (it's not and it's not running)
    if (parentAction_pri == nullptr or not parentAction_pri->isExecuting_f())
    {
        parentAction_pri = parentAction_par;
        for (std::pair<const int_fast64_t, checkData_c>& pair_ite : checkDataIdToCheckDataUMap_pri)
        {
            pair_ite.second.setParentAction_f(parentAction_par);
        }
    }
}

bool checksDataHub_c::checksExecutionFinished_f() const
{
    return checksExecutionFinished_pri;
}

std::vector<checkData_c*> checksDataHub_c::lastRunChecks_f() const
{
    return lastRunChecks_pri;
}

actionData_c* checksDataHub_c::parentAction_f() const
{
    return parentAction_pri;
}

int_fast32_t checksDataHub_c::updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub update stringId dependencies, old: " + oldStringId_par_con + " new: " + newStringId_par_con, logItem_c::type_ec::debug);
    int_fast32_t updateCountTmp(0);
    for (std::pair<const int_fast64_t, checkData_c>& pair_ite : checkDataIdToCheckDataUMap_pri)
    {
        if (pair_ite.second.updateStringIdDependencies_f(newStringId_par_con, oldStringId_par_con))
        {
            updateCountTmp = updateCountTmp + 1;
        }
    }
    return updateCountTmp;
}

bool checksDataHub_c::hasStringIdAnyDependency_f(const QString& stringId_par_con) const
{
    bool resultTmp(false);
    for (const std::pair<const int_fast64_t, checkData_c>& pair_ite_con : checkDataIdToCheckDataUMap_pri)
    {
        if (pair_ite_con.second.hasStringIdAnyDependency_f(stringId_par_con))
        {
            resultTmp = true;
            break;
        }
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
        for (checkData_c* checkData_ite_ptr : checksToRun_pri)
        {
            checkDataExecutionResult_c* checkDataExecutionResultTmp(checkData_ite_ptr->createGetCheckDataExecutionResult_ptr_f());
            QObject::connect(checkDataExecutionResultTmp, &checkDataExecutionResult_c::finished_signal, proxyQObj_pri, std::bind(&checksDataHub_c::verifyExecutionFinished_f, this));

            if (checkData_ite_ptr->threaded_f())
            {
                checkData_ite_ptr->tryExecute_f();
            }
            else
            {
                checksToRunSeq_pri.emplace_back(checkData_ite_ptr);
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
    for (checkData_c* checkData_ite_ptr : checksToRun_pri)
    {
        if (not checkData_ite_ptr->checkDataExecutionResult_ptr_f()->finished_f())
        {
            allFinishedTmp = false;
            break;
        }
        if (checkData_ite_ptr->checkDataExecutionResult_ptr_f()->state_f() == checkExecutionState_ec::stoppingByUser)
        {
            somethingStoppedTmp = true;
        }
    }
    if (allFinishedTmp)
    {
        MACRO_ADDACTONQTSOLOG("All checks finished executing", logItem_c::type_ec::info);
        executingChecks_pri = false;
        checksExecutionFinished_pri = true;
        lastRunChecks_pri = checksToRun_pri;
        proxyQObj_pri->checksExecutionFinished_signal(checksToRun_pri);
        if (stoppingChecksExecution_pri)
        {
            MACRO_ADDACTONQTSOLOG("Execution finished, after stopping", logItem_c::type_ec::info);
            stoppingChecksExecution_pri = false;
            if (somethingStoppedTmp)
            {
                MACRO_ADDACTONQTSOLOG("Execution finished, one or more checks were stopped", logItem_c::type_ec::info);
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

bool checksDataHub_c::insertCheckData_f(
        const checkData_c& obj_par_con
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

        checkData_c copyTmp(obj_par_con);
        copyTmp.setParentAction_f(parentAction_pri);

        //insert actionId - actionData object mapping
        checkDataIdToCheckDataUMap_pri.emplace(copyTmp.id_f(), copyTmp);

        //new row goes in the last position
        if (row_par_con == size_f())
        {
            //insert the mappings (since it's the last row no need to touch anything else)
            checkDataIdToRow_pri.emplace(copyTmp.id_f(), row_par_con);
            rowToCheckDataId_pri.emplace(row_par_con, copyTmp.id_f());
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
        checkDataIdToRow_pri.emplace(copyTmp.id_f(), row_par_con);
        rowToCheckDataId_pri.at(row_par_con) = copyTmp.id_f();
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
    if (findResult != checkDataIdToRow_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

int_fast64_t checksDataHub_c::rowToCheckDataId_f(const int row_par_con) const
{
    int_fast64_t resultTmp(-1);
    auto findResult(rowToCheckDataId_pri.find(row_par_con));
    if (findResult != rowToCheckDataId_pri.end())
    {
        resultTmp = findResult->second;
    }
    return resultTmp;
}

int_fast32_t checksDataHub_c::size_f() const
{
    return rowToCheckDataId_pri.size();
}

checkData_c* checksDataHub_c::checkData_ptr_f(const int_fast64_t checkDataId_par_con)
{
    checkData_c* checkDataPtrTmp(Q_NULLPTR);
    auto findResult(checkDataIdToCheckDataUMap_pri.find(checkDataId_par_con));
    if (findResult != checkDataIdToCheckDataUMap_pri.end())
    {
        checkDataPtrTmp = std::addressof(findResult->second);
    }
    return checkDataPtrTmp;
}

checkData_c checksDataHub_c::checkData_f(
        const int_fast64_t checkDataId_par_con
        , bool* found_ptr
) const
{
    auto findResult(checkDataIdToCheckDataUMap_pri.find(checkDataId_par_con));
    bool foundTmp(findResult != checkDataIdToCheckDataUMap_pri.cend());
    if (found_ptr not_eq nullptr)
    {
        *found_ptr = foundTmp;
    }
    return (foundTmp ? findResult->second : checkData_c());
}

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

bool checksDataHub_c::removeCheckDataUsingRow_f(
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
        checkDataIdToCheckDataUMap_pri.erase(checkIdToRemoveTmp);

        lastRunChecks_pri.clear();

        resultTmp = true;
        break;
    }
    MACRO_ADDACTONQTSOLOG("Check in row " + QString::number(row_par_con) + " removed: " + QSTRINGBOOL(resultTmp), logItem_c::type_ec::debug);
    return resultTmp;
}

bool checksDataHub_c::removeCheckDataUsingId_f(const int_fast64_t checkDataId_par_con)
{
    bool resultTmp(false);
    auto rowFindResult(checkDataIdToRow_f(checkDataId_par_con));
    if (rowFindResult != -1)
    {
        resultTmp = removeCheckDataUsingRow_f(rowFindResult);
    }
    return resultTmp;
}

