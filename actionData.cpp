#include "actionData.hpp"

#include "actonDataHub.hpp"
#include "checkData.hpp"

#include "actionDataExecutionResult.hpp"
#include "actionMappings/actionMapping.hpp"
#include "actionExecution/baseActionExecution.hpp"
#include "actionMappings/actionStrMapping.hpp"

#include "checkDataExecutionResult.hpp"
#include "checksBaseSerialization.hpp"
#include "actonBaseSerialization.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "essentialQtso/macros.hpp"
#include "comuso/practicalTemplates.hpp"

//#include "criptoQtso/hashQt.hpp"
//#include "signalso/signal.hpp"

#include <QJsonArray>
#include <QTimer>

#ifdef DEBUGJOUVEN
#include "actionMappings/actionExecutionStateStrMapping.hpp"
#include <QDebug>
#endif

int_fast64_t nextActionDataId_f()
{
    static int_fast64_t rootId(0);
    rootId = rootId + 1;
//    if (rootId == 2)
//    {
//        throw;
//    }
    return rootId;
}

//////////////////
//actionData_c
//////////////////


int_fast64_t action_c::id_f() const
{
    return id_pri;
}

//actionType_ec actionData_c::type_f() const
//{
//    return type_pri;
//}

QString action_c::description_f() const
{
    return description_pro;
}


//QJsonObject actionData_c::actionDataJSON_f() const
//{
//    return actionDataJSON_pri;
//}

//QJsonObject& actionData_c::actionDataJSON_ref_f()
//{
//    return actionDataJSON_pri;
//}

bool action_c::isExecuting_f() const
{
    //if the result object exists
    //and last state is one of the "executing" ones
    //and the result object hasn't reached *any* finish (error, success, timeout...)
    return actionDataExecutionResult_ptr_pri not_eq nullptr
            and actionDataExecution_ptr_pri not_eq nullptr
            and equalOnce_ft(actionDataExecutionResult_ptr_pri->lastState_f()
                             , actionExecutionState_ec::preparing
                             , actionExecutionState_ec::executingChecks
                             , actionExecutionState_ec::executing)
            and not actionDataExecutionResult_ptr_pri->finished_f()
            //this is different than the above because it takes care of single check execution (without executing the action)
            and not checkDataHub_pri.executingChecks_f();
}

bool action_c::checkResultLogicAnd_f() const
{
    return checkResultLogicAnd_pro;
}

void action_c::setCheckResultLogicAnd_f(const bool checkResultLogicAnd_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Action is executing", this, logItem_c::type_ec::warning);
    }
    else
    {
        checkResultLogicAnd_pro = checkResultLogicAnd_par_con;
    }
}

//bool action_c::isEditable_f() const
//{
//    return not isExecuting_f()
//            or (actionDataExecutionResult_ptr_pri == nullptr)
//            or actionDataExecutionResult_ptr_pri->finished_f()
//            or (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::initial);
//}

//use only on set value functions
bool action_c::tryClearResultsOnEdit_f()
{
    bool resultTmp(true);
    if (not isExecuting_f() and actionDataExecutionResult_ptr_pri not_eq nullptr)
    {
        resultTmp = actionDataExecutionResult_ptr_pri->tryClear_f();
    }
    return resultTmp;
}

uint_fast64_t action_c::derivedUpdateActionStringIdDependencies_f(const QString&, const QString&)
{
    return 0;
}

uint_fast64_t action_c::derivedActionStringIdDependencyCount_f(const QString&) const
{
    return 0;
}

uint_fast64_t action_c::derivedStringTriggerCreationConflictCount_f(const QString&) const
{
    return 0;
}

uint_fast64_t action_c::derivedUpdateStringTriggerDependecies_f(const QString&, const QString&)
{
    return 0;
}

uint_fast64_t action_c::derivedStringTriggerDependencyCount_f(const QString&) const
{
    return 0;
}

QSet<QString> action_c::derivedStringTriggerCreationCollection_f() const
{
    return QSet<QString>();
}

QSet<QString> action_c::derivedStringTriggersInUse_f(const QSet<QString>&) const
{
    return QSet<QString>();
}

//void actionData_c::setType_f(const actionType_ec type_par_con)
//{
//    if (isExecuting_f())
//    {
//        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
//    }
//    else
//    {
//        if (tryClearResultsOnEdit_f())
//        {
//            type_pri = type_par_con;
//        }
//    }
//}

void action_c::setDescription_f(const QString& description_par_con)
{
    description_pro = description_par_con;
}

//void actionData_c::setActionDataJSON_f(const QJsonObject& actionDataJSON_par_con)
//{
//    if (isExecuting_f())
//    {
//        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
//    }
//    else
//    {
//        if (tryClearResultsOnEdit_f())
//        {
//            actionDataJSON_pri = actionDataJSON_par_con;
//        }
//    }
//}

void action_c::tryStopExecution_f(const bool killAfterTimeout_par_con)
{
    MACRO_ADDACTONQTSOLOG("Try stop execution", this, logItem_c::type_ec::debug);
    while (true)
    {
        if (not enabled_pro)
        {
            break;
        }

        if (actionDataExecutionResult_ptr_pri not_eq nullptr
            and actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::preparing)
        {
            actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::stopping);
            MACRO_ADDACTONQTSOLOG("Stopped while preparing", this, logItem_c::type_ec::info);
            break;
        }

        if (checkDataHub_pri.executingChecks_f())
        {
            actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::stopping);
            //this should be enough since it will "return" to preparing
            checkDataHub_pri.stopExecutingChecks_f();
            MACRO_ADDACTONQTSOLOG("Stopping while executing checks", this, logItem_c::type_ec::info);
            break;
        }

        if (isExecuting_f())
        {
            if (isKillingExecutionAfterTimeout_pri)
            {
                MACRO_ADDACTONQTSOLOG("Force killing execution", this, logItem_c::type_ec::info);
                kill_f();
                break;
            }

            //this is for actions that stopping can cause an error,
            //which will trigger another stop (if the option to stop on error is set)
            //this happens with the runProcess depending on the process, some don't like SIGTERM
            if (actionDataExecutionResult_ptr_pri->lastState_f() not_eq actionExecutionState_ec::stopping)
            {
                actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::stopping);
//                if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::executingChecks)
//                {
//                    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" stopping check/s execution", logItem_c::type_ec::info);
//                    stopExecutingChecks_f();
//                }
//                else
//                {
                MACRO_ADDACTONQTSOLOG("Stopping execution", this, logItem_c::type_ec::info);
                actionDataExecution_ptr_pri->stop_f();
//                }
            }

            if (killAfterTimeout_par_con)
            {
                text_c logTextTmp("Killing execution in {0} ms", actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f());
                MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::info);
                isKillingExecutionAfterTimeout_pri = true;
                QTimer::singleShot(actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f(), this, &action_c::kill_f);
            }
        }
        break;
    }
}

void action_c::kill_f()
{
    while (true)
    {
        if (not enabled_pro)
        {
            break;
        }
        if (actionDataExecutionResult_ptr_pri == nullptr)
        {
            break;
        }
        if (actionDataExecution_ptr_pri == nullptr)
        {
            break;
        }

        if (not actionDataExecutionResult_ptr_pri->finished_f()
            and (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::stopping)
            )
        {
            MACRO_ADDACTONQTSOLOG("Killing execution", this, logItem_c::type_ec::info);
            actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::killing);
            actionDataExecution_ptr_pri->kill_f();
        }
        break;
    }
    isKillingExecutionAfterTimeout_pri = false;
}

void action_c::examineCheckResults_f(const bool result_par_con)
{
    MACRO_ADDACTONQTSOLOG("Examining checks results", this, logItem_c::type_ec::debug);
    if (result_par_con)
    {
        lastCheckLogicResult_pri = result_par_con;
        lastCheckLogicResultSet_pri = true;
        //checks result logic success
        prepareToExecuteAction_f();
    }
    else
    {
        //checks result logic failed
        //the action_c class can't emit Q_EMIT anyFinish_signal();
        //but the slot function can be called
        actionDataExecutionResult_ptr_pri->trySetFinished_f();
    }
}

void action_c::prepareToExecuteAction_f()
{
    MACRO_ADDACTONQTSOLOG("Preparing to execute", this, logItem_c::type_ec::info);
    actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::preparing);
    if (equalOnce_ft(type_f()
                     , actionType_ec::metaEndExecutionCycle
                     , actionType_ec::closeActon
                     , actionType_ec::createDirectory
                     , actionType_ec::modifyEnvironment
                     , actionType_ec::changeWorkingDirectory
                     //folderChangeReaction manages its threads and lives in the main thread because it can call self created actions (which spawn threads)
                     //and spawning a thread from "not the main thread" is BIG NO in Qt
                     , actionType_ec::folderChangeReaction))
    {
        execute_f();
    }
    else
    {
        threadedFunction_c* threadedFunction_ptr = new threadedFunction_c(std::bind(&action_c::execute_f, this), true);
        QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &threadedFunction_c::deleteLater);
        QObject::connect(actionDataExecutionResult_ptr_pri, &actionDataExecutionResult_c::finished_signal, threadedFunction_ptr, &threadedFunction_c::quit);
        threadedFunction_ptr->start();
    }
}

bool action_c::killingExecutionAfterTimeout_f() const
{
    return isKillingExecutionAfterTimeout_pri;
}

void action_c::stopExecutingChecks_f()
{
    MACRO_ADDACTONQTSOLOG("Stop executing checks", this, logItem_c::type_ec::debug);
    while (true)
    {
        if (not enabled_pro)
        {
            break;
        }
        for (int_fast32_t index_ite = 0, l = checkDataHub_pri.size_f(); index_ite < l; ++index_ite)
        {
            int_fast32_t checkDataIdTmp(checkDataHub_pri.rowToCheckDataId_f(index_ite));
            check_c* checkTmp_ptr(checkDataHub_pri.check_ptr_f(checkDataIdTmp));
            checkTmp_ptr->stopExecution_f();
        }
        break;
    }
}

void action_c::setActionDataExecutionNull_f()
{
    actionDataExecution_ptr_pri = nullptr;
}

void action_c::execute_f()
{
    while (true)
    {
        //this only affects thread-queued actions, actions that "started" but are waiting for a thread,
        //then something stopped the execution, then when a thread is available they will enter here
        if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::stopping
            or actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::killing)
        {
            actionDataExecutionResult_ptr_pri->trySetFinished_f();
            MACRO_ADDACTONQTSOLOG("Finished (stopped or \"killed\") before execute", this, logItem_c::type_ec::debug);
            break;
        }

        MACRO_ADDACTONQTSOLOG("Execute", this, logItem_c::type_ec::debug);

        actionDataExecution_ptr_pri = createExecutionObj_f(actionDataExecutionResult_ptr_pri);

        MACRO_ADDACTONQTSOLOG("Execution object ctored", this, logItem_c::type_ec::debug);

        QObject::connect(actionDataExecution_ptr_pri, &baseActionExecution_c::destroyed, this, &action_c::setActionDataExecutionNull_f);

        actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::executing);
        actionDataExecution_ptr_pri->execute_f();
        break;
    }
}

void action_c::setExecutionStateExecutingChecks_f()
{
    if (actionDataExecutionResult_ptr_pri not_eq nullptr)
    {
        actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::executingChecks);
    }
}

void action_c::tryExecute_f()
{
    MACRO_ADDACTONQTSOLOG("Try execute", this, logItem_c::type_ec::debug);
    while (true)
    {
        if (not enabled_pro)
        {
            MACRO_ADDACTONQTSOLOG("Can't execute a disabled Action", this, logItem_c::type_ec::info);
            break;
        }
        //if it's executing already
        if (isExecuting_f())
        {
            MACRO_ADDACTONQTSOLOG("Already executing", this, logItem_c::type_ec::info);
            break;
        }
        else
        {
            //go on
        }

        if (actionDataExecutionResult_ptr_pri == nullptr)
        {
            //initialize result obj if necessary
            actionDataExecutionResult_ptr_pri = new actionDataExecutionResult_c(this);
        }
        else
        {
            //when repeating execution

            //when running actions from actonDataHub_c this has been done previously
            //if actions are re-executed, if the first one finishes before the second is cleared (or any after before the next one),
            //actonDataHub-c::verifyExecutionFinished_f (or actionFinished checks) might think that some actions have
            //already finished (when they just haven been initialized again)
            if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::initial)
            {
                //initial state requires no changes
            }
            else
            {
                //if it's a finished results object, clear it
                actionDataExecutionResult_ptr_pri->tryClear_f();
            }
        }

        //execution objects auto delete themselves but check and do it if necessary
        if (actionDataExecution_ptr_pri == nullptr)
        {

        }
        else
        {
            deleteActionDataExecutionObject_f();
        }

        if (checkDataHub_pri.size_f() == 0 or not checksEnabled_pro)
        {
            //no checks path
            prepareToExecuteAction_f();
        }
        else
        {
            QObject::connect(
                        std::addressof(checkDataHub_pri)
                        , &checksDataHub_c::checksExecutionStarted_signal
                        , this
                        , &action_c::setExecutionStateExecutingChecks_f
                        , Qt::UniqueConnection);

            QObject::connect(
                        std::addressof(checkDataHub_pri)
                        , &checksDataHub_c::checksExecutionFinished_signal
                        , this
                        , &action_c::examineCheckResults_f
                        , Qt::UniqueConnection);

            lastCheckLogicResultSet_pri = false;
            checkDataHub_pri.executeCheckDataRows_f();
        }
        break;
    }
}


actionDataExecutionResult_c* action_c::createGetActionDataExecutionResult_ptr_f()
{
//#ifdef DEBUGJOUVEN
//        qDebug() << "createGetActionDataExecutionResult_ptr_f" << endl;
//#endif
    while (true)
    {
//#ifdef DEBUGJOUVEN
//        qDebug() << "createGetActionDataExecutionResult_ptr_f enabled_pri " << enabled_pri << endl;
//#endif
        if (not enabled_pro)
        {
            break;
        }

//        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
//        if (deleteObject_par_con and not isExecuting_f())
//        {
//            deleteActionDataExecutionResultObject_f();
//        }
        if (actionDataExecutionResult_ptr_pri == nullptr)
        {
            //#ifdef DEBUGJOUVEN
            //        qDebug() << "new actionDataExecutionResult_c(*this); " << endl;
            //#endif
            actionDataExecutionResult_ptr_pri = new actionDataExecutionResult_c(this);
        }
        break;
    }
    return actionDataExecutionResult_ptr_pri;
}

actionDataExecutionResult_c* action_c::actionDataExecutionResult_ptr_f() const
{
    return actionDataExecutionResult_ptr_pri;
}

//the following two functions are "simple" now but if an action type starts using other actionIds they will become more complex
uint_fast64_t action_c::updateActionStringIdDependencies_f(
        const QString& newActionStringId_par_con
        , const QString& oldActionStringId_par_con)
{
    return derivedUpdateActionStringIdDependencies_f(newActionStringId_par_con, oldActionStringId_par_con)
            + checkDataHub_pri.updateActionStringIdDependencies_f(newActionStringId_par_con, oldActionStringId_par_con);
}

uint_fast64_t action_c::actionStringIdDependencyCount_f(const QString& actionStringId_par_con) const
{
    return derivedActionStringIdDependencyCount_f(actionStringId_par_con)
            + checkDataHub_pri.actionStringIdDependencyCount_f(actionStringId_par_con);
}

uint_fast64_t action_c::updateStringTriggerDependencies_f(
        const QString& newStringTrigger_par_con
        , const QString& oldStringTrigger_par_con)
{
    return derivedUpdateStringTriggerDependecies_f(newStringTrigger_par_con, oldStringTrigger_par_con)
            + checkDataHub_pri.updateStringTriggerDependencies_f(newStringTrigger_par_con, oldStringTrigger_par_con);
}

std::pair<int_fast64_t, int_fast64_t> action_c::stringTriggerCreationConflict_f(const QString& stringTrigger_par_con, const void* const objectToIgnore_par) const
{
    int_fast64_t actionIdTmp(0);
    if (derivedStringTriggerCreationConflictCount_f(stringTrigger_par_con) > 0)
    {
        actionIdTmp = id_pri;
    }
    return std::pair<int_fast64_t, int_fast64_t>(
                actionIdTmp
                , checkDataHub_pri.stringTriggerCreationConflict_f(stringTrigger_par_con, objectToIgnore_par));
}

uint_fast64_t action_c::stringTriggerDependencyCount_f(
        const QString& stringTrigger_par_con
        , const void* const objectToIgnore_par)
{
    return derivedStringTriggerDependencyCount_f(stringTrigger_par_con)
            + checkDataHub_pri.stringTriggerDependencyCount_f(stringTrigger_par_con, objectToIgnore_par);
}

QSet<QString> action_c::stringTriggerCreationCollection_f() const
{
    QSet<QString> resulTmp(derivedStringTriggerCreationCollection_f());
    resulTmp.unite(checkDataHub_pri.stringTriggerCreationCollection_f());
    return resulTmp;
}

QSet<QString> action_c::stringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const
{
    QSet<QString> resulTmp(derivedStringTriggersInUse_f(searchValues_par_con));
    resulTmp.unite(checkDataHub_pri.stringTriggersInUseByChecks_f(searchValues_par_con));
    return resulTmp;
}

action_c* action_c::readCreateDerived_f(const actionType_ec actionType_par_con)
{
    action_c* resultTmp(nullptr);
    auto findResultTmp(actionTypeToActionCreationFunctionMap_ext_con.find(actionType_par_con));
    if (findResultTmp not_eq actionTypeToActionCreationFunctionMap_ext_con.cend())
    {
        resultTmp = findResultTmp->second();
    }
    else
    {
        //nullptr is the default value
    }
    return resultTmp;
}

action_c* action_c::clone_f() const
{
    action_c* cloneTmp(derivedClone_f());
    cloneTmp->checkDataHub_pri = checkDataHub_pri.clone_f();
    cloneTmp->checkDataHub_pri.setParentAction_f(cloneTmp);
    return cloneTmp;
}

bool action_c::isFieldsActionValid_f(textCompilation_c* errorsPtr_par) const
{
    //required to reorder the errors, see the if contents
    textCompilation_c errorsTmp;
    textCompilation_c* errorsTmpPtr(errorsPtr_par == nullptr ? nullptr : std::addressof(errorsTmp));
    bool isValidResultTmp(
                actionData_c::isFieldsActionDataValid_f(errorsTmpPtr)
                and derivedIsValidAction_f(errorsTmpPtr)
                and checkDataHub_pri.areChecksValid_f(errorsTmpPtr));
    if (errorsPtr_par not_eq nullptr and not errorsTmp.empty_f())
    {
        errorsPtr_par->append_f(errorsTmp);
    }
    else
    {
        if (not isValidResultTmp)
        {
            MACRO_ADDACTONQTSOLOG("Not valid but also has no errors", this, logItem_c::type_ec::warning);
        }
        else
        {
            //if it's valid nothing to do
        }
    }
    return isValidResultTmp;
}

void action_c::updateActionData_f(
        const actionData_c& actionData_par_con
        , const bool updateObjectsThatUsedOldValue_par_con
        , textCompilation_c* errorsPtr_par)
{
    const QString oldStringIdTmp(this->stringId_pro);
    while (actionData_par_con.isFieldsActionDataValid_f(errorsPtr_par))
    {
        if (isExecuting_f())
        {
            text_c logTextTmp("Can't update action data while executing");
            APPENDTEXTPTR(errorsPtr_par, logTextTmp)
            MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::warning);
            break;
        }

        if (actonDataHub_ptr_ext not_eq nullptr)
        {
            if (actionData_par_con.stringId_f() not_eq this->stringId_f()
                and not actonDataHub_ptr_ext->validStringId_f(actionData_par_con.stringId_f()))
            {
                text_c logTextTmp("Can't update action data, stringId already in use");
                APPENDTEXTPTR(errorsPtr_par, logTextTmp)
                MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::warning);
                break;
            }

            tryClearResultsOnEdit_f();

            QString oldStringIdTmp(stringId_pro);
            this->actionData_c::operator=(actionData_par_con);
            if (updateObjectsThatUsedOldValue_par_con)
            {
                actonDataHub_ptr_ext->updateStringIdDependencies_f(actionData_par_con.stringId_f(), oldStringIdTmp);
            }
        }
        else
        {
            tryClearResultsOnEdit_f();
            this->actionData_c::operator=(actionData_par_con);
        }
        break;
    }

    if (oldStringIdTmp not_eq actionData_par_con.stringId_f())
    {
        Q_EMIT actionStringIdChanged_signal(this->stringId_pro, oldStringIdTmp);
    }
}

QString actionData_c::stringId_f() const
{
    return stringId_pro;
}

uint_fast64_t action_c::setStringId_f(
        const QString& newStringId_par_con
        , const bool updateObjectsThatUsedOldValue_par_con
        , textCompilation_c* errorsPtr_par)
{
    uint_fast64_t updateCountTmp(0);
    QString oldActionStringIdTmp(this->stringId_pro);
    while (true)
    {
        if (newStringId_par_con.isEmpty())
        {
            text_c logTextTmp("New action stringId is empty");
            APPENDTEXTPTR(errorsPtr_par, logTextTmp)
            MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::warning);
            break;
        }
        if (this->stringId_pro == newStringId_par_con)
        {
            text_c logTextTmp("New action stringId: \"{0}\" to update is the same", newStringId_par_con);
            APPENDTEXTPTR(errorsPtr_par, logTextTmp)
            MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::warning);
            break;
        }
        if (isExecuting_f())
        {
            text_c logTextTmp("Can't update action stringId while executing");
            APPENDTEXTPTR(errorsPtr_par, logTextTmp)
            MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::warning);
            break;
        }

        if (actonDataHub_ptr_ext not_eq nullptr)
        {
            if (not actonDataHub_ptr_ext->validStringId_f(newStringId_par_con))
            {
                text_c logTextTmp("Can't update action stringId: \"{0}\" is already in use", newStringId_par_con);
                APPENDTEXTPTR(errorsPtr_par, logTextTmp)
                MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::warning);
                break;
            }

            tryClearResultsOnEdit_f();

            QString oldStringIdTmp(stringId_pro);
            stringId_pro = newStringId_par_con;
            updateCountTmp = 1;
            if (updateObjectsThatUsedOldValue_par_con)
            {
                updateCountTmp = updateCountTmp + actonDataHub_ptr_ext->updateStringIdDependencies_f(newStringId_par_con, oldStringIdTmp);
            }
        }
        else
        {
            tryClearResultsOnEdit_f();
            stringId_pro = newStringId_par_con;
            updateCountTmp = 1;
        }
        break;
    }
    if (updateCountTmp > 0)
    {
        Q_EMIT actionStringIdChanged_signal(newStringId_par_con, oldActionStringIdTmp);
    }
    return updateCountTmp;
}

bool action_c::checksEnabled_f() const
{
    return checksEnabled_pro;
}

void action_c::setChecksEnabled_f(const bool checksEnabled_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Can't enable/disable checks while executing", this, logItem_c::type_ec::warning);
    }
    else
    {
        tryClearResultsOnEdit_f();
        checksEnabled_pro = checksEnabled_par_con;
    }
}


checksDataHub_c* action_c::checkDataHub_ptr_f()
{
    return std::addressof(checkDataHub_pri);
}

const checksDataHub_c& action_c::checkDataHub_f() const
{
    return checkDataHub_pri;
}

bool action_c::isStoppingExecution_f() const
{
    return actionDataExecutionResult_ptr_pri not_eq nullptr
            and actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::stopping;
}

bool action_c::enabled_f() const
{
    return enabled_pro;
}

void action_c::setEnabled_f(const bool enabled_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Can't enable/disable this action while executing", this, logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            enabled_pro = enabled_par_con;
            //on disable delete the execution objects
            if (not enabled_pro)
            {
                deleteActionDataExecutionObject_f();
                deleteActionDataExecutionResultObject_f();
            }
        }
    }
}

bool action_c::lastCheckLogicResult_f() const
{
    return lastCheckLogicResult_pri;
}

bool action_c::lastCheckLogicResultSet_f() const
{
    return lastCheckLogicResultSet_pri;
}

void action_c::deleteActionDataExecutionObject_f()
{
    if (actionDataExecution_ptr_pri == nullptr)
    {

    }
    else
    {
        actionDataExecution_ptr_pri->deleteLater();
        actionDataExecution_ptr_pri = nullptr;
    }
}

void action_c::deleteActionDataExecutionResultObject_f()
{
    if (actionDataExecutionResult_ptr_pri == nullptr)
    {

    }
    else
    {
        text_c logTextTmp("actionDataExecutionResult_ptr_pri null? {0}", QSTRINGBOOL((actionDataExecutionResult_ptr_pri == nullptr)));
        MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::debug);
        actionDataExecutionResult_ptr_pri->deleteLater();
        actionDataExecutionResult_ptr_pri = nullptr;
    }
}

action_c::action_c(
        const actionData_c& actionData_par_con
) : actionData_c(actionData_par_con)
    , id_pri(nextActionDataId_f())
    , checkDataHub_pri(this)
{
    text_c logTextTmp("Action constructed, enabled = {0}", QSTRINGBOOL(enabled_pro));
    MACRO_ADDACTONQTSOLOG(logTextTmp, this, logItem_c::type_ec::debug);
}

void action_c::deleteExecutionObjects_f()
{
    deleteActionDataExecutionObject_f();
    deleteActionDataExecutionResultObject_f();
}

void action_c::deleteUsedPtrs_f()
{
    deleteExecutionObjects_f();
}

action_c::~action_c()
{
    MACRO_ADDACTONQTSOLOG("Action destroyed", this, logItem_c::type_ec::debug);
    deleteUsedPtrs_f();
}

QString action_c::typeStr_f() const
{
    return actionTypeToStrUMap_ext_con.at(type_f());
}

action_c::action_c()
    : action_c(actionData_c())
{
    MACRO_ADDACTONQTSOLOG("Action constructed (empty constructor)", this, logItem_c::type_ec::debug);
}


void action_c::write_f(QJsonObject& json_ref_par) const
{
    json_ref_par["type"] = typeStr_f();
    json_ref_par["stringId"] = stringId_pro;
    json_ref_par["description"] = description_pro;
    json_ref_par["checkResultLogicAnd"] = checkResultLogicAnd_pro;
    QJsonObject derivedObjTmp;
    derivedWrite_f(derivedObjTmp);
    json_ref_par["actionTypeData"] = derivedObjTmp;
    json_ref_par["checksEnabled"] = checksEnabled_pro;
    json_ref_par["enabled"] = enabled_pro;
    json_ref_par["stopAllExecutionOnError"] = stopAllExecutionOnError_pro;
    json_ref_par["repeatExecution"] = repeatExecution_pro;

    copyFromChecksDataHubAndSerialize_f(checkDataHub_pri, json_ref_par);
    MACRO_ADDACTONQTSOLOG("Action serialized", this, logItem_c::type_ec::debug);
}

void action_c::read_f(
        const QJsonObject& json_par_con
        , const bool loadOnlyValid_par_con
        , textCompilation_c* errors_par)
{
    //no need to check if it's running since the read_f it's, right now, only used after creating an actionData_c object
    stringId_pro = json_par_con["stringId"].toString();
    description_pro = json_par_con["description"].toString();
    if (json_par_con["checkResultLogicAnd"].isBool())
    {
        checkResultLogicAnd_pro = json_par_con["checkResultLogicAnd"].toBool();
    }
    if (not json_par_con["checks"].isUndefined())
    {
        deserializeAndCopyToChecksDataHub_f(
                    json_par_con
                    , checkDataHub_pri
                    , loadOnlyValid_par_con
                    , errors_par);
    }

    if (json_par_con["checksEnabled"].isBool())
    {
        checksEnabled_pro = json_par_con["checksEnabled"].toBool();
    }
    if (json_par_con["enabled"].isBool())
    {
        enabled_pro = json_par_con["enabled"].toBool();
    }
    if (json_par_con["stopAllExecutionOnError"].isBool())
    {
        stopAllExecutionOnError_pro = json_par_con["stopAllExecutionOnError"].toBool();
    }
    if (json_par_con["repeatExecution"].isBool())
    {
        repeatExecution_pro = json_par_con["repeatExecution"].toBool();
    }

    derivedRead_f(json_par_con["actionTypeData"].toObject());
    MACRO_ADDACTONQTSOLOG("Action deserialized", this, logItem_c::type_ec::debug);
}

//actionData_c::actionData_c(
//        const actionData_c& source_par_con
//)
//    : stringId_pro(source_par_con.stringId_pro)
//    , description_pro(source_par_con.description_pro)
//    , checkResultLogicAnd_pro(source_par_con.checkResultLogicAnd_pro)
//    , enabled_pro(source_par_con.enabled_pro)
//    , checksEnabled_pro(source_par_con.checksEnabled_pro)
//    , stopAllExecutionOnError_pro(source_par_con.stopAllExecutionOnError_pro)
//{
//    MACRO_ADDACTONQTSOLOG("actionData_c, constructed (\"derived\" constructor)", logItem_c::type_ec::debug);
//}

bool action_c::stopAllExecutionOnError_f() const
{
    return stopAllExecutionOnError_pro;
}

void action_c::setStopAllExecutionOnError_f(const bool stopAllExecutionOnError_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Can't set stopAllExecutionOnError while executing", this, logItem_c::type_ec::debug);
    }
    else
    {
        stopAllExecutionOnError_pro = stopAllExecutionOnError_par_con;
    }
}

bool action_c::repeatExecution_f() const
{
    return repeatExecution_pro;
}

void action_c::setRepeatExecution_f(const bool repeatExecution_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Can't set repeatExecution while executing", this, logItem_c::type_ec::debug);
    }
    else
    {
        repeatExecution_pro = repeatExecution_par_con;
    }
}

//actionData_c::actionData_c()
//    : checkDataHub_pro(nullptr)
//{}

actionData_c::actionData_c(
        const QString& stringId_par_con
        , const QString& description_par_con
        , const bool checkResultLogicAnd_par_con
        , const bool enabled_par_con
        , const bool checksEnabled_par_con
        , const bool stopAllExecutionOnError_par_con
        , const bool repeatExecution_par_con)
    : stringId_pro(stringId_par_con)
    , description_pro(description_par_con)
    , checkResultLogicAnd_pro(checkResultLogicAnd_par_con)
    , enabled_pro(enabled_par_con)
    , checksEnabled_pro(checksEnabled_par_con)
    , stopAllExecutionOnError_pro(stopAllExecutionOnError_par_con)
    , repeatExecution_pro(repeatExecution_par_con)
    //, checkDataHub_pro(nullptr)
{
    MACRO_ADDACTONQTSOLOG("ActionData_c constructed (regular constructor)", logItem_c::type_ec::debug);
}

bool actionData_c::isFieldsActionDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        if (stringId_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "stringId is empty")
            break;
        }
        if (description_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Description is empty")
            break;
        }

        //the size could be limited... but it's not like system path, at most they can "kill"
        //the memory also qt plaintext, I think, has length limit by default
//        {
//            text_c errorTextTmp;
//            if (isValidStringSize_f(stringId_pro, 255, std::addressof(errorTextTmp), "Action stringId is too long: {0} (maximum length is {1})"))
//            {
//                //it's valid
//            }
//            else
//            {
//                APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
//                break;
//            }
//        }
//        {
//            text_c errorTextTmp;
//            if (isValidStringSize_f(description_pro, 255, std::addressof(errorTextTmp), "Action stringId is too long: {0} (maximum length is {1})"))
//            {
//                //it's valid
//            }
//            else
//            {
//                APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
//                break;
//            }
//        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}
