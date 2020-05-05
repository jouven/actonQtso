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
    check_c* checkCopyTmp(nullptr);
    //qInfo() << "actionDataIdTmp " << actionDataIdTmp << endl;
    if (checkDataIdTmp > 0)
    {
        //save the actionData stuff that is going to get moved
        //no ref because... first it's removed (removeActionDataUsingRow_f)
        //so copy
        checkCopyTmp = check_ptr_f(checkDataIdTmp);

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

    text_c textTmp("Check moved from row {0} to row {1} result: {2}", sourceRow_par_con, destinationRow_par_con, QSTRINGBOOL(resultTmp));
    MACRO_ADDACTONQTSOLOG(textTmp, checkCopyTmp, logItem_c::type_ec::debug);
    return resultTmp;
}

void checksDataHub_c::verifyCheckResults_f()
{
    action_c* parentActionPtrTmp(static_cast<action_c*>(parent()));

    MACRO_ADDACTONQTSOLOG("Examining checks results", parentActionPtrTmp, logItem_c::type_ec::debug);

    bool somethingStoppedTmp(false);
    bool allFinishedTmp(true);
    for (check_c* check_ite_ptr : checksToRun_pri)
    {
        if (not check_ite_ptr->checkDataExecutionResult_ptr_f()->finished_f())
        {
            allFinishedTmp = false;
        }
        if (check_ite_ptr->checkDataExecutionResult_ptr_f()->lastState_f() == checkExecutionState_ec::stoppedByUser)
        {
            somethingStoppedTmp = true;
        }
    }

    while (true)
    {
        if (checksExecutionStopped_pri)
        {
            //don't bother
            break;
        }

        //if the check result logic "and" and not all the checks have finished
        if (not allFinishedTmp and parentActionPtrTmp->checkResultLogicAnd_f())
        {
            break;
        }

        decltype(checksToRun_pri.size()) resultSetCountTmp(0);
        //to help the "and" logic
        bool resultTmp(true);
        bool resultSetTmp(false);
        for (check_c* check_ite_ptr : checksToRun_pri)
        {
            checkDataExecutionResult_c* checkDataExecutionResult_ptr_Ref(check_ite_ptr->checkDataExecutionResult_ptr_f());

            if (checkDataExecutionResult_ptr_Ref->finished_f())
            {
                resultSetCountTmp = resultSetCountTmp + 1;
                if (parentActionPtrTmp->checkResultLogicAnd_f())
                {
                    resultTmp = resultTmp and checkDataExecutionResult_ptr_Ref->logicResult_f();
                    if (resultSetCountTmp == checksToRun_pri.size())
                    {
                        resultSetTmp = true;
                    }
                    else
                    {
                        //not the last result
                    }
                }
                else
                {
                    resultTmp = checkDataExecutionResult_ptr_Ref->logicResult_f();
                    resultSetTmp = true;
                    if (resultTmp)
                    {
                        break;
                    }
                }
            }
            else
            {
                //not finished
            }
        }
#ifdef DEBUGJOUVEN
        //    qDebug() << "allFinishedTmp " << QSTRINGBOOL(allFinishedTmp) << endl;
        //    qDebug() << "resultSetTmp " << resultSetTmp << endl;
        //    qDebug() << "resultTmp " << resultTmp << endl;
        //    qDebug() << "checkDataHub_pri.size_f() " << this->size_f() << endl;
#endif
        if (resultSetTmp)
        {
            //if there is one result but not all the check results are in
            //stop any still running/waiting/left check
            //this happens when using "OR" logic on the results (aka checkResultLogicAnd_pro == false)
            if (resultSetCountTmp < size_f())
            {
                stopExecutingChecks_f();
            }
            //        text_c logMessageTmp("Action stringId: \"{0}\" preparing to run Action", static_cast<action_c*>(parent())->stringId_f());
            //        MACRO_ADDACTONQTSOLOG(logMessageTmp, logItem_c::type_ec::info);
            if (stoppingChecksExecution_pri)
            {
                stoppingChecksExecution_pri = false;
                MACRO_ADDACTONQTSOLOG("Execution finished, after stopping", parentActionPtrTmp, logItem_c::type_ec::info);
                if (somethingStoppedTmp)
                {
                    MACRO_ADDACTONQTSOLOG("Execution finished, one or more checks were stopped", parentActionPtrTmp, logItem_c::type_ec::info);
                    checksExecutionStopped_pri = true;
                    Q_EMIT checksExecutionStopped_signal();
                }
            }
            else
            {

            }
            text_c logMessageTmp("Checks result is {1}", QSTRINGBOOL(resultTmp));
            MACRO_ADDACTONQTSOLOG(logMessageTmp, parentActionPtrTmp, logItem_c::type_ec::info);
            Q_EMIT checksExecutionFinished_signal(resultTmp);
        }
        else
        {
            //no definitive results yet
        }
        break;
    }

    if (allFinishedTmp)
    {
        executingChecks_pri = false;
    }
}

std::vector<check_c*> checksDataHub_c::executeCheckDataRows_f(std::vector<int> rows_par)
{
	action_c* parentActionPtrTmp(static_cast<action_c*>(parent()));
	MACRO_ADDACTONQTSOLOG("Execute Checks from rows", parentActionPtrTmp, logItem_c::type_ec::debug);
	if (executingChecks_pri)
	{
		//do nothing if already executing
	}
	else
	{
		bool somethingIsExecuting(false);
		bool checksToRunValidTmp(true);
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
					MACRO_ADDACTONQTSOLOG("Check is executing", parentActionPtrTmp, logItem_c::type_ec::warning);
					somethingIsExecuting = true;
					break;
				}
				if (not checkPtrTmp->enabled_f())
				{
					MACRO_ADDACTONQTSOLOG("Check is not enabled", parentActionPtrTmp, logItem_c::type_ec::warning);
					continue;
				}
				if (not checkPtrTmp->isFieldsCheckValid_f())
				{
					MACRO_ADDACTONQTSOLOG("Check to execute is not valid", parentActionPtrTmp, logItem_c::type_ec::error);
					checksToRunValidTmp = false;
					continue;
				}
				checksToRun_pri.emplace_back(checkPtrTmp);
			}
		}
		if (not somethingIsExecuting and checksToRunValidTmp)
		{
			executeChecks_f();
		}
	}
	return checksToRun_pri;
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
        Q_EMIT stoppingChecksExecution_signal();
        for (check_c* check_ite_ptr : checksToRun_pri)
        {
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
    check_c* checkDataTmp(checksToRunSeq_pri.front());
    MACRO_ADDACTONQTSOLOG("Execute next check", checkDataTmp, logItem_c::type_ec::debug);
    checksToRunSeq_pri.pop_front();
    //if there are >0 checks to run, make it so when checkDataTmp finishes,
    //calls this function again to run the next one
    if (checksToRunSeq_pri.size() > 0)
    {
        checkDataExecutionResult_c* checkDataExecutionResultTmp_ptr(checkDataTmp->createGetCheckDataExecutionResult_ptr_f());
        QObject::connect(
                    checkDataExecutionResultTmp_ptr
                    , &checkDataExecutionResult_c::finished_signal
                    , this
                    , &checksDataHub_c::executeNextSeqCheck_f
                    , Qt::UniqueConnection);
    }
    checkDataTmp->tryExecute_f();
    //        threadedFunction_c* threadedFunction_ptr = new threadedFunction_c(std::bind(&checkData_c::tryExecute_f, checkDataTmp), true);
    //        QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &QObject::deleteLater);
    //        QObject::connect(checkDataExecutionResultTmp_ptr, &checkDataExecutionResult_c::finished_signal, threadedFunction_ptr, &threadedFunction_c::quit);

    //        threadedFunction_ptr->start();
}

checksDataHub_c::checksDataHub_c(action_c* parentAction_par)
    : QObject(parentAction_par)
{
    MACRO_ADDACTONQTSOLOG("checksDataHub constructed", logItem_c::type_ec::debug);
}

//ignore the "warning: base class ‘class QObject’ should be explicitly initialized in the copy constructor [-Wextra]"
//default QObject is fine (also their copy ctor is deleted)
checksDataHub_c::checksDataHub_c(const checksDataHub_c& from_par_con)
    : checkDataIdToRow_pri(from_par_con.checkDataIdToRow_pri)
    , rowToCheckDataId_pri(from_par_con.rowToCheckDataId_pri)
    , checkDataIdToCheckUMap_pri(from_par_con.checkDataIdToCheckUMap_pri)
{
    setParentAction_f(static_cast<action_c*>(from_par_con.parent()));
    MACRO_ADDACTONQTSOLOG("checksDataHub copied (const)", logItem_c::type_ec::debug);
}

checksDataHub_c::checksDataHub_c(checksDataHub_c& from_par)
    : checkDataIdToRow_pri(from_par.checkDataIdToRow_pri)
    , rowToCheckDataId_pri(from_par.rowToCheckDataId_pri)
    , checkDataIdToCheckUMap_pri(from_par.checkDataIdToCheckUMap_pri)
{
    setParentAction_f(static_cast<action_c*>(from_par.parent()));
    MACRO_ADDACTONQTSOLOG("checksDataHub copied (no const)", logItem_c::type_ec::debug);
}

checksDataHub_c& checksDataHub_c::operator=(const checksDataHub_c& from_par_con)
{
    checkDataIdToRow_pri = from_par_con.checkDataIdToRow_pri;
    rowToCheckDataId_pri = from_par_con.rowToCheckDataId_pri;
    checkDataIdToCheckUMap_pri = from_par_con.checkDataIdToCheckUMap_pri;
    setParentAction_f(static_cast<action_c*>(from_par_con.parent()));

    MACRO_ADDACTONQTSOLOG("checksDataHub assigned= (const)", logItem_c::type_ec::debug);
    return *this;
}

checksDataHub_c& checksDataHub_c::operator=(checksDataHub_c& from_par_con)
{
    checkDataIdToRow_pri = from_par_con.checkDataIdToRow_pri;
    rowToCheckDataId_pri = from_par_con.rowToCheckDataId_pri;
    checkDataIdToCheckUMap_pri = from_par_con.checkDataIdToCheckUMap_pri;
    setParentAction_f(static_cast<action_c*>(from_par_con.parent()));

    MACRO_ADDACTONQTSOLOG("checksDataHub assigned= (no const)", logItem_c::type_ec::debug);
    return *this;
}

checksDataHub_c& checksDataHub_c::operator=(checksDataHub_c&& from_par) noexcept
{
    checkDataIdToRow_pri = std::move(from_par.checkDataIdToRow_pri);
    rowToCheckDataId_pri = std::move(from_par.rowToCheckDataId_pri);
    checkDataIdToCheckUMap_pri = std::move(from_par.checkDataIdToCheckUMap_pri);
    //lastRunChecks_pri = std::move(from_par.lastRunChecks_pri);
    setParentAction_f(static_cast<action_c*>(from_par.parent()));

    from_par.setParentAction_f(nullptr);
    MACRO_ADDACTONQTSOLOG("checksDataHub assigned-moved", logItem_c::type_ec::debug);
    return *this;
}

checksDataHub_c::checksDataHub_c(checksDataHub_c&& from_par) noexcept
    : checkDataIdToRow_pri(std::move(from_par.checkDataIdToRow_pri))
    , rowToCheckDataId_pri(std::move(from_par.rowToCheckDataId_pri))
    , checkDataIdToCheckUMap_pri(std::move(from_par.checkDataIdToCheckUMap_pri))
    //, lastRunChecks_pri(std::move(from_par.lastRunChecks_pri))
{
    setParentAction_f(static_cast<action_c*>(from_par.parent()));

    from_par.setParentAction_f(nullptr);
    MACRO_ADDACTONQTSOLOG("checksDataHub constructed-moved", logItem_c::type_ec::debug);
}

void checksDataHub_c::setParentAction_f(action_c* parentAction_par)
{
    text_c textTmp("checksDataHub set parent action, parentAction is nullptr? {0}",  QSTRINGBOOL(parentAction_par not_eq nullptr));
    MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    //if it's null or (it's not and it's not running)
    if (parentAction_par == nullptr or not parentAction_par->isExecuting_f())
    {
        setParent(parentAction_par);
        for (std::pair<const int_fast64_t, check_c*>& pair_ite : checkDataIdToCheckUMap_pri)
        {
            pair_ite.second->setParent(parentAction_par);
        }
    }
}

//action_c* checksDataHub_c::parentAction_f() const
//{
//    return parentAction_pri;
//}

uint_fast64_t checksDataHub_c::updateActionStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    text_c logMessageTmp("ChecksDataHub update stringId dependencies, old: {0} new: {1}", oldStringId_par_con, newStringId_par_con);
    MACRO_ADDACTONQTSOLOG(logMessageTmp, logItem_c::type_ec::debug);
    uint_fast64_t updateCountTmp(0);
    for (std::pair<const int_fast64_t, check_c*>& pair_ite : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite.second->updateActionStringIdDependencies_f(newStringId_par_con, oldStringId_par_con))
        {
            updateCountTmp = updateCountTmp + 1;
        }
    }
    return updateCountTmp;
}

uint_fast64_t checksDataHub_c::actionStringIdDependencyCount_f(const QString& stringId_par_con) const
{
    uint_fast64_t resultTmp(0);
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        resultTmp = resultTmp + pair_ite_con.second->actionStringIdDependencyCount_f(stringId_par_con);
    }
    return resultTmp;
}

int_fast64_t checksDataHub_c::stringTriggerCreationConflict_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par) const
{
    uint_fast64_t resultTmp(0);
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite_con.second == objectToIgnore_par)
        {
            continue;
        }
        resultTmp = resultTmp + pair_ite_con.second->stringTriggerCreationConflict_f(stringTrigger_par_con);
    }
    return resultTmp;
}

uint_fast64_t checksDataHub_c::updateStringTriggerDependencies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
{
    text_c logMessageTmp("ChecksDataHub update string trigger parser dependencies, old: {0} new: {1}", oldStringTrigger_par_con, newStringTrigger_par_con);
    MACRO_ADDACTONQTSOLOG(logMessageTmp, logItem_c::type_ec::debug);
    uint_fast64_t updateCountTmp(0);
    for (std::pair<const int_fast64_t, check_c*>& pair_ite : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite.second->updateStringTriggerDependecies_f(newStringTrigger_par_con, oldStringTrigger_par_con))
        {
            updateCountTmp = updateCountTmp + 1;
        }
    }
    return updateCountTmp;
}

uint_fast64_t checksDataHub_c::stringTriggerDependencyCount_f(
        const QString& stringTrigger_par_con
        , const void* const objectToIgnore_par) const
{
    uint_fast64_t resultTmp(0);
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        if (pair_ite_con.second == objectToIgnore_par)
        {
            continue;
        }
        resultTmp = resultTmp + pair_ite_con.second->sringTriggerDependencyCount_f(stringTrigger_par_con);
    }
    return resultTmp;
}

QSet<QString> checksDataHub_c::stringTriggerCreationCollection_f() const
{
    QSet<QString> resultTmp;
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        QSet<QString> setTmp(pair_ite_con.second->stringTriggerCreationCollection_f());
        resultTmp.unite(setTmp);
    }
    return resultTmp;
}

QSet<QString> checksDataHub_c::stringTriggersInUseByChecks_f(const QSet<QString>& searchValues_par_con) const
{
    QSet<QString> resultTmp;
    for (const std::pair<const int_fast64_t, check_c*>& pair_ite_con : checkDataIdToCheckUMap_pri)
    {
        QSet<QString> setTmp(pair_ite_con.second->stringTriggersInUse_f(searchValues_par_con));
        resultTmp.unite(setTmp);
    }
    return resultTmp;
}

bool checksDataHub_c::areChecksValid_f(textCompilation_c* errors_par) const
{
    bool areValidTmp(true);
    for (unsigned int i = 0; i < rowToCheckDataId_pri.size(); ++i)
    {
         auto checkDataIdTmp(rowToCheckDataId_pri.at(i));
         check_c* const checkPtrTmp_con(check_ptr_f(checkDataIdTmp));
         areValidTmp = areValidTmp and checkPtrTmp_con->isFieldsCheckValid_f(errors_par);
    }
    return areValidTmp;
}

checksDataHub_c checksDataHub_c::clone_f() const
{
    checksDataHub_c copyTmp(nullptr);
    for (unsigned int rowIndex_ite = 0; rowIndex_ite < rowToCheckDataId_pri.size(); ++rowIndex_ite)
    {
        auto checkDataIdTmp(rowToCheckDataId_pri.at(rowIndex_ite));
        check_c* const checkPtrTmp_con(checkDataIdToCheckUMap_pri.at(checkDataIdTmp));
        copyTmp.insertCheck_f(checkPtrTmp_con->clone_f(), rowIndex_ite);
    }
    return copyTmp;
}

bool checksDataHub_c::checksExecutionStopped_f() const
{
    return checksExecutionStopped_pri;
}

void checksDataHub_c::executeChecks_f()
{
    text_c textTmp("Execute checks, checksToRun_pri.empty()? {0}",  QSTRINGBOOL(checksToRun_pri.empty()));
    MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    if (not checksToRun_pri.empty())
    {
        checksExecutionStopped_pri = false;
        executingChecks_pri = true;
        checksToRunSeq_pri.clear();
        Q_EMIT checksExecutionStarted_signal();
        for (check_c* check_ite_ptr : checksToRun_pri)
        {
            checkDataExecutionResult_c* checkDataExecutionResultPtrTmp(check_ite_ptr->createGetCheckDataExecutionResult_ptr_f());
            if (checkDataExecutionResultPtrTmp->lastState_f() == checkExecutionState_ec::initial)
            {
                //initial state requires no changes
            }
            else
            {
                checkDataExecutionResultPtrTmp->tryClear_f();
            }
        }
        for (check_c* check_ite_ptr : checksToRun_pri)
        {
            checkDataExecutionResult_c* checkDataExecutionResultPtrTmp(check_ite_ptr->createGetCheckDataExecutionResult_ptr_f());
            QObject::connect(
                        checkDataExecutionResultPtrTmp
                        , &checkDataExecutionResult_c::finished_signal
                        , this
                        , &checksDataHub_c::verifyCheckResults_f
                        , Qt::UniqueConnection);

            checkDataExecutionResultPtrTmp->trySetExecutionState_f(checkExecutionState_ec::preparing);
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

checksDataHub_c::~checksDataHub_c()
{
    //in theory¿ this is not necessary, since checks are "setParentED" to the action (QObject)
    //when the action gets dtored it should dtor its children (QObjects too)
    MACRO_ADDACTONQTSOLOG("CheckDataHub destroyed", logItem_c::type_ec::debug);
    for (std::pair<const int_fast64_t, check_c*>& pair_ite : checkDataIdToCheckUMap_pri)
    {
        pair_ite.second->deleteLater();
    }
}

bool checksDataHub_c::validRow_f(const row_t row_par_con) const
{
    return row_par_con <= size_f();
}

bool checksDataHub_c::insertCheck_f(
        check_c* checkPtr_par
        , const row_t row_par_con)
{
    bool resultTmp(false);
    action_c* parentActionPtrTmp(static_cast<action_c*>(parent()));
    while (true)
    {
        if (parentActionPtrTmp not_eq nullptr and parentActionPtrTmp->isExecuting_f())
        {
            break;
        }
        if (not validRow_f(row_par_con))
        {
            break;
        }

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
        //this way there is no need to delete all the checks on the destruction of the action_c/actonHub_c object/s
        checkPtr_par->setParent(parentActionPtrTmp);
        resultTmp = true;

        break;
    }
    text_c textTmp("Check inserted? {0}",  QSTRINGBOOL(resultTmp));
    MACRO_ADDACTONQTSOLOG(textTmp, parentActionPtrTmp, logItem_c::type_ec::info);
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

decltype(checksDataHub_c::rowToCheckDataId_pri.size()) checksDataHub_c::size_f() const
{
    return rowToCheckDataId_pri.size();
}

check_c* checksDataHub_c::check_ptr_f(const int_fast64_t checkDataId_par_con)
{
    check_c* checkPtrTmp(nullptr);
    auto findResult(checkDataIdToCheckUMap_pri.find(checkDataId_par_con));
    if (findResult not_eq checkDataIdToCheckUMap_pri.end())
    {
        checkPtrTmp = findResult->second;
    }
    return checkPtrTmp;
}

check_c* checksDataHub_c::check_ptr_f(const int_fast64_t checkDataId_par_con) const
{
    check_c* checkPtrTmp(nullptr);
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
        const row_t row_par_con
        , const int_fast64_t checkDataId_par_con)
{
    bool resultTmp(false);
    action_c* parentActionPtrTmp(static_cast<action_c*>(parent()));
    while (true)
    {
        if (parentActionPtrTmp not_eq nullptr and parentActionPtrTmp->isExecuting_f())
        {
            break;
        }
        //invalid index
        if (row_par_con >= size_f())
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
        row_t removedRowIndexTmp(row_par_con);
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

    text_c textTmp("Check in row {0} removed: {1}", row_par_con, QSTRINGBOOL(resultTmp));
    MACRO_ADDACTONQTSOLOG(textTmp, parentActionPtrTmp, logItem_c::type_ec::info);
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

