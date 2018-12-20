#include "actionData.hpp"

#include "actonDataHub.hpp"

#include "actionDataExecutionResult.hpp"
#include "actionMappings/actionStrMapping.hpp"
#include "actionExecution/runProcessExecution.hpp"
#include "actionExecution/createDirectoryExecution.hpp"

#include "checkDataExecutionResult.hpp"
#include "checksBaseSerialization.hpp"

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


int_fast64_t actionData_c::id_f() const
{
    return id_pri;
}

actionType_ec actionData_c::type_f() const
{
    return type_pri;
}

QString actionData_c::description_f() const
{
    return description_pri;
}

QJsonObject actionData_c::actionDataJSON_f() const
{
    return actionDataJSON_pri;
}

//QJsonObject& actionData_c::actionDataJSON_ref_f()
//{
//    return actionDataJSON_pri;
//}

bool actionData_c::isExecuting_f() const
{
    //if the result object exists
    //and result state is one of the "executing" ones
    //and the result object hasn't reached *any* finish (error, success, timeout...)
    return actionDataExecutionResult_ptr_pri not_eq nullptr
            and actionDataExecution_ptr_pri not_eq nullptr
            and equalOnce_ft(actionDataExecutionResult_ptr_pri->lastState_f()
                             , actionExecutionState_ec::preparing
                             , actionExecutionState_ec::executingChecks
                             , actionExecutionState_ec::executing)
            and not actionDataExecutionResult_ptr_pri->finished_f();
}

bool actionData_c::isEditable_f() const
{
    return not isExecuting_f() or actionDataExecutionResult_ptr_pri == nullptr or actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::initial;
}

//use only on sets
bool actionData_c::tryClearResultsOnEdit_f()
{
    bool resultTmp(true);
    if (not isEditable_f())
    {
        resultTmp = actionDataExecutionResult_ptr_pri->tryClear_f();
    }
    return resultTmp;
}

void actionData_c::setType_f(const actionType_ec type_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            type_pri = type_par_con;
        }
    }
}

void actionData_c::setDescription_f(const QString& description_par_con)
{
    description_pri = description_par_con;
}

void actionData_c::setActionDataJSON_f(const QJsonObject& actionDataJSON_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            actionDataJSON_pri = actionDataJSON_par_con;
        }
    }
}

void actionData_c::tryStopExecution_f(const bool killAfterTimeout_par_con)
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" try stop execution", logItem_c::type_ec::debug);
    while (true)
    {
        if (not enabled_pri)
        {
            break;
        }

        if (actionDataExecutionResult_ptr_pri not_eq nullptr
            and actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::preparing)
        {
            actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::stoppingByUser);
            MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" stopped while preparing", logItem_c::type_ec::info);
            break;
        }

        if (checkDataHub_pri.executingChecks_f())
        {
            //this should be enough since it will "return" to preparing
            checkDataHub_pri.stopExecutingChecks_f();
            MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" stopping while executing cheks", logItem_c::type_ec::info);
            break;
        }

        if (isExecuting_f())
        {
            if (isKillingExecutionAfterTimeout_pri)
            {
                MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" force killing execution" , logItem_c::type_ec::info);
                kill_f();
                break;
            }

            //this is for actions that stopping can cause an error,
            //which will trigger another stop (if the option to stop on error is set)
            //this happens with the runProcess depending on the process, some don't like SIGTERM
            if (actionDataExecutionResult_ptr_pri->lastState_f() not_eq actionExecutionState_ec::stoppingByUser)
            {
                actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::stoppingByUser);
//                if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::executingChecks)
//                {
//                    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" stopping check/s execution", logItem_c::type_ec::info);
//                    stopExecutingChecks_f();
//                }
//                else
//                {
                    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" stopping execution", logItem_c::type_ec::info);
                    actionDataExecution_ptr_pri->stop_f();
//                }
            }

            if (killAfterTimeout_par_con)
            {
                MACRO_ADDACTONQTSOLOG(
                            "Action, stringId: \"" + this->stringId_pri + "\" killing execution in " + QString::number(actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f()) + "ms"
                            , logItem_c::type_ec::info
                );
                isKillingExecutionAfterTimeout_pri = true;
                QTimer::singleShot(actonDataHub_ptr_ext->executionOptions_f().killTimeoutMilliseconds_f(), std::bind(&actionData_c::kill_f, this));
            }
        }
        break;
    }
}

void actionData_c::kill_f()
{
    while (true)
    {
        if (not enabled_pri)
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
            and (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::stoppingByUser)
            )
        {
            MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" killing execution", logItem_c::type_ec::info);
            actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::killingByUser);
            actionDataExecution_ptr_pri->kill_f();
        }
        break;
    }
    isKillingExecutionAfterTimeout_pri = false;
}

void actionData_c::examineCheckResults_f(std::vector<checkData_c*> checksRun_par)
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\"examining checks results", logItem_c::type_ec::debug);
    if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::stoppingByUser
        or actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::killingByUser)
    {
        actionDataExecutionResult_ptr_pri->trySetFinished_f();
    }
    else
    {
        int_fast32_t resultSetCount(0);
        //to help the "and" logic
        bool resultTmp(true);
        bool resultSetTmp(false);
        for (checkData_c* check_ite_ptr : checksRun_par)
        {
            checkDataExecutionResult_c* checkDataExecutionResult_ptr_Ref(check_ite_ptr->checkDataExecutionResult_ptr_f());

            if (checkDataExecutionResult_ptr_Ref->finished_f())
            {
                resultSetCount = resultSetCount + 1;
                if (checkResultLogicAnd_pri)
                {
                    resultTmp = resultTmp and checkDataExecutionResult_ptr_Ref->result_f();
                    if (resultSetCount == checkDataHub_pri.size_f())
                    {
                        resultSetTmp = true;
                    }
                }
                else
                {
                    resultTmp = checkDataExecutionResult_ptr_Ref->result_f();
                    resultSetTmp = true;
                    if (resultTmp)
                    {
                        break;
                    }
                }
            }
            else
            {
                if (checkResultLogicAnd_pri)
                {
                    break;
                }
            }

        }
        if (resultSetTmp and resultTmp)
        {
            MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" preparing to run Action", logItem_c::type_ec::info);
            prepareToRunAction_f();
        }
    }
}

void actionData_c::prepareToRunAction_f()
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" preparing to run", logItem_c::type_ec::debug);
    if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::stoppingByUser
        or actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::killingByUser)
    {
        actionDataExecutionResult_ptr_pri->trySetFinished_f();
    }
    else
    {
        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" preparing to run", logItem_c::type_ec::info);
        actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::preparing);
        threadedFunction_c* threadedFunction_ptr = new threadedFunction_c(std::bind(&actionData_c::execute_f, this), true);
        QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &threadedFunction_c::deleteLater);
        QObject::connect(actionDataExecutionResult_ptr_pri, &actionDataExecutionResult_c::finished_signal, threadedFunction_ptr, &threadedFunction_c::quit);
        threadedFunction_ptr->start();
    }
}

bool actionData_c::killingExecutionAfterTimeout_f() const
{
    return isKillingExecutionAfterTimeout_pri;
}

void actionData_c::stopExecutingChecks_f()
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" stop executing checks", logItem_c::type_ec::debug);
    while (true)
    {
        if (not enabled_pri)
        {
            break;
        }
        for (int_fast32_t index_ite = 0, l = checkDataHub_pri.size_f(); index_ite < l; ++index_ite)
        {
            int_fast32_t checkDataIdTmp(checkDataHub_pri.rowToCheckDataId_f(index_ite));
            checkData_c* checkDataTmp_ptr(checkDataHub_pri.checkData_ptr_f(checkDataIdTmp));
            checkDataTmp_ptr->stopExecution_f();
        }
        break;
    }
}

void actionData_c::execute_f()
{
    while (true)
    {
        if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::stoppingByUser
            or actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::killingByUser)
        {
            actionDataExecutionResult_ptr_pri->trySetFinished_f();
            break;
        }

        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" execute", logItem_c::type_ec::debug);
        switch (type_pri)
        {
            case actionType_ec::runProcess:
            {
                runProcessAction_c runProcessActionTmp;
                runProcessActionTmp.read_f(actionDataJSON_f());
                runProcessActionExecution_c* runProcessActionExecutionTmp = new runProcessActionExecution_c(actionDataExecutionResult_ptr_pri, runProcessActionTmp);

                actionDataExecution_ptr_pri = runProcessActionExecutionTmp;
            }
                break;
            case actionType_ec::createDirectory:
            {
                createDirectoryAction_c createDirectoryActionTmp;
                createDirectoryActionTmp.read_f(actionDataJSON_f());

                createDirectoryActionExecution_c* createDirectoryActionExecutionTmp = new createDirectoryActionExecution_c(actionDataExecutionResult_ptr_pri, createDirectoryActionTmp);
                actionDataExecution_ptr_pri = createDirectoryActionExecutionTmp;
            }
                break;
            default:
            {
                //theoretically it shouldn't enter here ever
            }
        }

        QObject::connect(actionDataExecution_ptr_pri, &baseActionExecution_c::destroyed, [this]() { actionDataExecution_ptr_pri = nullptr; });

        actionDataExecution_ptr_pri->execute_f();
        break;
    }
}

void actionData_c::tryExecute_f()
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" try execute", logItem_c::type_ec::debug);
    while (true)
    {
        if (not enabled_pri)
        {
            MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" can't run a not enabled Action", logItem_c::type_ec::info);
            break;
        }
        //if it's executing already
        if (isExecuting_f())
        {
            MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" already executing", logItem_c::type_ec::info);
            break;
        }
        else
        {
            //go on
        }

        //if it's a finished results object, delete
        if (actionDataExecutionResult_ptr_pri == nullptr)
        {
            //initialize result obj if necessary
            actionDataExecutionResult_ptr_pri = new actionDataExecutionResult_c(this);
        }
        else
        {
            if (actionDataExecutionResult_ptr_pri->lastState_f() == actionExecutionState_ec::initial)
            {
                //initial state requires no changes
            }
            else
            {
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

        if (checkDataHub_pri.size_f() == 0 or not checksEnabled_pri)
        {
            //no checks path
            prepareToRunAction_f();
        }
        else
        {
            QObject::connect(
                        checkDataHub_pri.proxyQObj_f()
                        , &checksDataHubProxyQObj_c::checksExecutionStarted_signal
                        , actionDataExecutionResult_ptr_pri
            , [this]()
            {
                QObject::disconnect(checkDataHub_pri.proxyQObj_f(), &checksDataHubProxyQObj_c::checksExecutionStarted_signal, actionDataExecutionResult_ptr_pri, nullptr);
                actionDataExecutionResult_ptr_pri->trySetExecutionState_f(actionExecutionState_ec::executingChecks);
            }
            );
            QObject::connect(
                        checkDataHub_pri.proxyQObj_f()
                        , &checksDataHubProxyQObj_c::checksExecutionFinished_signal
                        , actionDataExecutionResult_ptr_pri
            , [this](std::vector<checkData_c*> checksRun_par)
            {
                QObject::disconnect(checkDataHub_pri.proxyQObj_f(), &checksDataHubProxyQObj_c::checksExecutionFinished_signal, actionDataExecutionResult_ptr_pri, nullptr);
                examineCheckResults_f(checksRun_par);
            }
            );

            checkDataHub_pri.executeCheckDataRows_f();

        }
        break;
    }
}


actionDataExecutionResult_c* actionData_c::createGetActionDataExecutionResult_ptr_f()
{
//#ifdef DEBUGJOUVEN
//        qDebug() << "createGetActionDataExecutionResult_ptr_f" << endl;
//#endif
    while (true)
    {
//#ifdef DEBUGJOUVEN
//        qDebug() << "createGetActionDataExecutionResult_ptr_f enabled_pri " << enabled_pri << endl;
//#endif
        if (not enabled_pri)
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

actionDataExecutionResult_c* actionData_c::actionDataExecutionResult_ptr_f() const
{
    return actionDataExecutionResult_ptr_pri;
}

//the following two functions are "simple" now but if an action type starts using other actionIds they will become more complex
int_fast32_t actionData_c::updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    return checkDataHub_pri.updateStringIdDependencies_f(newStringId_par_con, oldStringId_par_con);
}

bool actionData_c::hasStringIdAnyDependency_f(const QString& stringId_par_con) const
{
    return checkDataHub_pri.hasStringIdAnyDependency_f(stringId_par_con);
}

QString actionData_c::stringId_f() const
{
    return stringId_pri;
}

int_fast32_t actionData_c::setStringId_f(const QString& newStringId_par_con, const bool updateFieldsThatUsedOldValue_par_con)
{
    int_fast32_t updateCountTmp(0);
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            QString oldStringIdTmp(stringId_pri);
            stringId_pri = newStringId_par_con;
            if (updateFieldsThatUsedOldValue_par_con)
            {
                updateCountTmp = updateCountTmp + actonDataHub_ptr_ext->updateStringIdDependencies_f(newStringId_par_con, oldStringIdTmp);
            }
        }
    }
    return updateCountTmp;
}

bool actionData_c::checksEnabled_f() const
{
    return checksEnabled_pri;
}

void actionData_c::setChecksEnabled_f(const bool checksEnabled_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            checksEnabled_pri = checksEnabled_par_con;
        }
    }
}


checksDataHub_c* actionData_c::checkDataHub_ptr_f()
{
    return std::addressof(checkDataHub_pri);
}

const checksDataHub_c& actionData_c::checkDataHub_f() const
{
    return checkDataHub_pri;
}

bool actionData_c::enabled_f() const
{
    return enabled_pri;
}

void actionData_c::setEnabled_f(const bool enabled_par_con)
{
    if (isExecuting_f())
    {
        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            enabled_pri = enabled_par_con;
            //on disable delete the execution objects
            if (not enabled_pri)
            {
                deleteActionDataExecutionObject_f();
                deleteActionDataExecutionResultObject_f();
            }
        }
    }
}

void actionData_c::deleteActionDataExecutionObject_f()
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

void actionData_c::deleteActionDataExecutionResultObject_f()
{
    if (actionDataExecutionResult_ptr_pri == nullptr)
    {

    }
    else
    {
        MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" actionDataExecutionResult_ptr_pri: " + QSTRINGBOOL((actionDataExecutionResult_ptr_pri == nullptr)), logItem_c::type_ec::debug);
        actionDataExecutionResult_ptr_pri->deleteLater();
        actionDataExecutionResult_ptr_pri = nullptr;
    }
}

void actionData_c::deleteUsedPtrs_f()
{
    deleteActionDataExecutionObject_f();
    deleteActionDataExecutionResultObject_f();
}

actionData_c::actionData_c(
        const actionType_ec type_par_con
        , const QString& stringId_par_con
        , const QString& description_par_con
        , const bool checkResultLogicAnd_par_con
        , const bool runAllChecksAnyway_par_con
        , const bool checksEnabled_par_con
        , const bool enabled_par_con
        , const QJsonObject& actionDataJson_par_con
        )
    : id_pri(nextActionDataId_f())
    , type_pri(type_par_con)
    , stringId_pri(stringId_par_con)
    , description_pri(description_par_con)
    , checkResultLogicAnd_pri(checkResultLogicAnd_par_con)
    , runAllChecksAnyway_pri(runAllChecksAnyway_par_con)
    , checksEnabled_pri(checksEnabled_par_con)
    , enabled_pri(enabled_par_con)
    , actionDataJSON_pri(actionDataJson_par_con)
    , checkDataHub_pri(this)
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" constructed, enabled: " + QSTRINGBOOL(enabled_pri), logItem_c::type_ec::debug);
}

actionData_c::actionData_c(const actionData_c& from_par_con)
    : id_pri(from_par_con.id_pri)
    , type_pri(from_par_con.type_pri)
    , stringId_pri(from_par_con.stringId_pri)
    , description_pri(from_par_con.description_pri)
    , checkResultLogicAnd_pri(from_par_con.checkResultLogicAnd_pri)
    , runAllChecksAnyway_pri(from_par_con.runAllChecksAnyway_pri)
    , checksEnabled_pri(from_par_con.checksEnabled_pri)
    , enabled_pri(from_par_con.enabled_pri)
    , actionDataJSON_pri(from_par_con.actionDataJSON_pri)
    , checkDataHub_pri(from_par_con.checkDataHub_pri)

{
    checkDataHub_pri.setParentAction_f(this);
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" copied (const)", logItem_c::type_ec::debug);
}

actionData_c::actionData_c(actionData_c& from_par)
    : id_pri(from_par.id_pri)
    , type_pri(from_par.type_pri)
    , stringId_pri(from_par.stringId_pri)
    , description_pri(from_par.description_pri)
    , checkResultLogicAnd_pri(from_par.checkResultLogicAnd_pri)
    , runAllChecksAnyway_pri(from_par.runAllChecksAnyway_pri)
    , checksEnabled_pri(from_par.checksEnabled_pri)
    , enabled_pri(from_par.enabled_pri)
    , actionDataJSON_pri(from_par.actionDataJSON_pri)
    , checkDataHub_pri(from_par.checkDataHub_pri)

{
    checkDataHub_pri.setParentAction_f(this);
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" copied (no const)", logItem_c::type_ec::debug);
}

actionData_c& actionData_c::operator=(const actionData_c& from_par_con)
{
    type_pri = from_par_con.type_pri;
    stringId_pri = from_par_con.stringId_pri;
    description_pri = from_par_con.description_pri;
    checkResultLogicAnd_pri = from_par_con.checkResultLogicAnd_pri;
    runAllChecksAnyway_pri = from_par_con.runAllChecksAnyway_pri;
    checksEnabled_pri = from_par_con.checksEnabled_pri;
    enabled_pri = from_par_con.enabled_pri;
    actionDataJSON_pri = from_par_con.actionDataJSON_pri;
    checkDataHub_pri = from_par_con.checkDataHub_pri;
    checkDataHub_pri.setParentAction_f(this);

    deleteUsedPtrs_f();
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" assigned= (const)", logItem_c::type_ec::debug);
    return *this;
}

actionData_c& actionData_c::operator=(actionData_c& from_par_con)
{
    type_pri = from_par_con.type_pri;
    stringId_pri = from_par_con.stringId_pri;
    description_pri = from_par_con.description_pri;
    checkResultLogicAnd_pri = from_par_con.checkResultLogicAnd_pri;
    runAllChecksAnyway_pri = from_par_con.runAllChecksAnyway_pri;
    checksEnabled_pri = from_par_con.checksEnabled_pri;
    enabled_pri = from_par_con.enabled_pri;
    actionDataJSON_pri = from_par_con.actionDataJSON_pri;
    checkDataHub_pri = from_par_con.checkDataHub_pri;
    checkDataHub_pri.setParentAction_f(this);

    deleteUsedPtrs_f();
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" assigned= (no const)", logItem_c::type_ec::debug);
    return *this;
}

actionData_c& actionData_c::operator=(actionData_c&& from_par) noexcept
{
    //id_pri = std::move(from_par.id_pri);
    type_pri = from_par.type_pri;
    stringId_pri = std::move(from_par.stringId_pri);
    description_pri = std::move(from_par.description_pri);
    checkResultLogicAnd_pri = from_par.checkResultLogicAnd_pri;
    runAllChecksAnyway_pri = from_par.runAllChecksAnyway_pri;
    checksEnabled_pri = from_par.checksEnabled_pri;
    enabled_pri = from_par.enabled_pri;
    actionDataJSON_pri = std::move(from_par.actionDataJSON_pri);
    checkDataHub_pri = std::move(from_par.checkDataHub_pri);
    actionDataExecution_ptr_pri = from_par.actionDataExecution_ptr_pri;
    actionDataExecutionResult_ptr_pri =from_par.actionDataExecutionResult_ptr_pri;

    from_par.actionDataExecution_ptr_pri = nullptr;
    from_par.actionDataExecutionResult_ptr_pri = nullptr;

    checkDataHub_pri.setParentAction_f(this);
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" assigned-moved", logItem_c::type_ec::debug);
    return *this;
}

actionData_c::actionData_c(actionData_c&& from_par) noexcept
    : id_pri(from_par.id_pri)
    , type_pri(from_par.type_pri)
    , stringId_pri(std::move(from_par.stringId_pri))
    , description_pri(std::move(from_par.description_pri))
    , checkResultLogicAnd_pri(from_par.checkResultLogicAnd_pri)
    , runAllChecksAnyway_pri(from_par.runAllChecksAnyway_pri)
    , checksEnabled_pri(from_par.checksEnabled_pri)
    , enabled_pri(from_par.enabled_pri)
    , actionDataJSON_pri(std::move(from_par.actionDataJSON_pri))
    , checkDataHub_pri(std::move(from_par.checkDataHub_pri))
    , actionDataExecution_ptr_pri(from_par.actionDataExecution_ptr_pri)
    , actionDataExecutionResult_ptr_pri(from_par.actionDataExecutionResult_ptr_pri)
{
    from_par.actionDataExecution_ptr_pri = nullptr;
    from_par.actionDataExecutionResult_ptr_pri = nullptr;

    checkDataHub_pri.setParentAction_f(this);
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" constructed-moved", logItem_c::type_ec::debug);
}


actionData_c::actionData_c()
    : id_pri(nextActionDataId_f())
    , checkDataHub_pri(this)
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" constructed (empty constructor)", logItem_c::type_ec::debug);
}


actionData_c::~actionData_c()
{
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" destroyed", logItem_c::type_ec::debug);
    deleteUsedPtrs_f();
}

void actionData_c::write_f(QJsonObject& json_ref_par) const
{
    json_ref_par["type"] = actionTypeToStrUMap_glo_sta_con.at(type_pri);
    json_ref_par["stringId"] = stringId_pri;
    json_ref_par["description"] = description_pri;
    json_ref_par["actionDataJSON"] = actionDataJSON_pri;
    json_ref_par["checkResultLogicAnd"] = checkResultLogicAnd_pri;
    json_ref_par["runAllChecksAnyway"] = runAllChecksAnyway_pri;
    json_ref_par["checksEnabled"] = checksEnabled_pri;
    json_ref_par["enabled"] = enabled_pri;

    copyFromChecksDataHubAndSerialize_f(checkDataHub_pri, json_ref_par);
    //this might need to be changed to id
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" serialized", logItem_c::type_ec::debug);
}

void actionData_c::read_f(const QJsonObject& json_par_con)
{
    //no need to check if it's running since the read_f it's, right now, only used after creating an actionData_c object
    type_pri = strToActionTypeMap_glo_sta_con.value(json_par_con["type"].toString().toLower());
    stringId_pri = json_par_con["stringId"].toString();
    description_pri = json_par_con["description"].toString();
    actionDataJSON_pri = json_par_con["actionDataJSON"].toObject();

    if (not json_par_con["checks"].isUndefined())
    {
        deserializeAndCopyToChecksDataHub_f(json_par_con, checkDataHub_pri);
    }

    checkResultLogicAnd_pri = json_par_con["checkResultLogicAnd"].toBool();
    runAllChecksAnyway_pri = json_par_con["runAllChecksAnyway"].toBool();
    checksEnabled_pri = json_par_con["checksEnabled"].toBool();
    enabled_pri = json_par_con["enabled"].toBool();
    MACRO_ADDACTONQTSOLOG("Action, stringId: \"" + this->stringId_pri + "\" deserialized", logItem_c::type_ec::debug);
}

