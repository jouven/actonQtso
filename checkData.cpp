#include "checkData.hpp"

#include "actonDataHub.hpp"

#include "checkDataExecutionResult.hpp"
#include "checksDataHub.hpp"

#include "checkMappings/checkStrMapping.hpp"
#include "checkExecution/actionFinishedExecution.hpp"
#include "checkExecution/sameFileExecution.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "essentialQtso/macros.hpp"
//#include "criptoQtso/hashQt.hpp"
//#include "signalso/signal.hpp"
#include "comuso/practicalTemplates.hpp"

#include <QJsonArray>

#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

int_fast64_t nextCheckDataId_f()
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
//checkData_c
//////////////////


int_fast64_t checkData_c::id_f() const
{
    return id_pri;
}

checkType_ec checkData_c::type_f() const
{
    return type_pri;
}

QString checkData_c::description_f() const
{
    return description_pri;
}

QJsonObject checkData_c::checkDataJSON_f() const
{
    return checkDataJSON_pri;
}

//QJsonObject& checkData_c::checkDataJSON_ref_f()
//{
//    return checkDataJSON_pri;
//}

bool checkData_c::isExecuting_f() const
{
    return checkDataExecutionResult_ptr_pri not_eq nullptr
            and checkDataExecution_ptr_pri not_eq nullptr
            and equalOnce_ft(checkDataExecutionResult_ptr_pri->lastState_f()
                            , checkExecutionState_ec::preparing
                            , checkExecutionState_ec::executing)
            and not checkDataExecutionResult_ptr_pri->finished_f();
}

bool checkData_c::isEditable_f() const
{
    return not isExecuting_f() or checkDataExecutionResult_ptr_pri == nullptr or checkDataExecutionResult_ptr_pri->lastState_f() == checkExecutionState_ec::initial;
}

//use only on sets
bool checkData_c::tryClearResultsOnEdit_f()
{
    bool resultTmp(true);
    if (not isEditable_f())
    {
        resultTmp = checkDataExecutionResult_ptr_pri->tryClear_f();
    }
    return resultTmp;
}

void checkData_c::setType_f(const checkType_ec type_par_con)
{
    if (
        (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        or isExecuting_f()
        )
    {
        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            type_pri = type_par_con;
        }
    }
}

void checkData_c::setDescription_f(const QString& description_par_con)
{
    description_pri = description_par_con;
}

void checkData_c::setCheckDataJSON_f(const QJsonObject& checkDataJSON_par_con)
{
    if (
        (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        or isExecuting_f()
        )
    {
        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            checkDataJSON_pri = checkDataJSON_par_con;
        }
    }
}

void checkData_c::prepareToRun_f()
{
    //IMPORTANT no stop checking here, like an action does, because checks don't run something else like an action with checks,
    //so it's not necessary, since the function uses the main-thread, which blocks the GUI, it's not possible to execute and immediately stop
    MACRO_ADDACTONQTSOLOG("Check, id: " + QString::number(this->id_pri) + " preparing to run", logItem_c::type_ec::debug);
    checkDataExecutionResult_ptr_pri->trySetExecutionState_f(checkExecutionState_ec::preparing);

    threadedFunction_c* threadedFunction_ptr = new threadedFunction_c(std::bind(&checkData_c::execute_f, this), true);
    QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &QObject::deleteLater);
    QObject::connect(checkDataExecutionResult_ptr_pri, &checkDataExecutionResult_c::finished_signal, threadedFunction_ptr, &threadedFunction_c::quit);
    threadedFunction_ptr->start();
}

void checkData_c::execute_f()
{
    while (true)
    {
        if (checkDataExecutionResult_ptr_pri->lastState_f() == checkExecutionState_ec::stoppingByUser)
        {
            checkDataExecutionResult_ptr_pri->trySetFinished_f();
            break;
        }

        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " execute", logItem_c::type_ec::info);
        switch (type_pri)
        {
            case checkType_ec::actionFinished:
            {
                actionFinishedCheck_c actionFinishedCheckTmp;
                actionFinishedCheckTmp.read_f(checkDataJSON_f());
                actionFinishedCheckExecution_c* actionFinishedCheckExecutionTmp = new actionFinishedCheckExecution_c(checkDataExecutionResult_ptr_pri, actionFinishedCheckTmp);

                checkDataExecution_ptr_pri = actionFinishedCheckExecutionTmp;
            }
                break;
            case checkType_ec::sameFile:
            {
                sameFileCheck_c sameFileCheckTmp;
                sameFileCheckTmp.read_f(checkDataJSON_f());
                sameFileCheckExecution_c* sameFileCheckExecutionTmp = new sameFileCheckExecution_c(checkDataExecutionResult_ptr_pri, sameFileCheckTmp);

                checkDataExecution_ptr_pri = sameFileCheckExecutionTmp;
            }
                break;
            default:
            {
                //theoretically it shouldn't enter here ever
            }
        }

        QObject::connect(checkDataExecution_ptr_pri, &baseCheckExecution_c::destroyed, checkDataExecutionResult_ptr_pri, [this]() { checkDataExecution_ptr_pri = nullptr; });

        checkDataExecution_ptr_pri->execute_f();

        break;
    }
}

void checkData_c::tryExecute_f()
{
    //TODO verify if the check ids all come from the same nextId_f()
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " try execute", logItem_c::type_ec::debug);
    while (true)
    {
        //not enabled
        if (not enabled_pri)
        {
            MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " can't run a not enabled Check", logItem_c::type_ec::info);
            break;
        }
        else
        {
            //go on
        }
        //something is executing
        if (isExecuting_f())
        {
           MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " already executing", logItem_c::type_ec::info);
           break;
        }
        else
        {
            //go on
        }

        //if it's a finished results object, delete
        if (checkDataExecutionResult_ptr_pri == nullptr)
        {
            //initialize result obj if necessary
             checkDataExecutionResult_ptr_pri = new checkDataExecutionResult_c(this);
        }
        else
        {
            if (checkDataExecutionResult_ptr_pri->lastState_f() == checkExecutionState_ec::initial)
            {
                //initial state requires no changes
            }
            else
            {
                checkDataExecutionResult_ptr_pri->tryClear_f();
            }
        }

        if (checkDataExecution_ptr_pri == nullptr)
        {

        }
        else
        {
            deleteCheckDataExecutionObject_f();
        }

        prepareToRun_f();
        break;
    }
}

void checkData_c::stopExecution_f()
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " try stop execution", logItem_c::type_ec::debug);
    while (true)
    {
        if (not enabled_pri)
        {
            break;
        }

        if (checkDataExecutionResult_ptr_pri not_eq nullptr
            and checkDataExecutionResult_ptr_pri->lastState_f() == checkExecutionState_ec::stoppingByUser)
        {
            break;
        }

        if (checkDataExecutionResult_ptr_pri not_eq nullptr
            and checkDataExecutionResult_ptr_pri->lastState_f() == checkExecutionState_ec::preparing)
        {
            checkDataExecutionResult_ptr_pri->trySetExecutionState_f(checkExecutionState_ec::stoppingByUser);
            MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " stopped while preparing", logItem_c::type_ec::info);
            break;
        }

        if (isExecuting_f())
        {
            MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " stopping execution", logItem_c::type_ec::info);
            checkDataExecutionResult_ptr_pri->trySetExecutionState_f(checkExecutionState_ec::stoppingByUser);
            checkDataExecution_ptr_pri->stop_f();
        }

        break;
    }
}

checkDataExecutionResult_c* checkData_c::createGetCheckDataExecutionResult_ptr_f()
{
    while (true)
    {
        if (not enabled_pri)
        {
            break;
        }
//        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
//        if (deleteObject_par_con and not isExecuting_f())
//        {
//            deleteCheckDataExecutionResultObject_f();
//        }
        if (checkDataExecutionResult_ptr_pri == nullptr)
        {
            checkDataExecutionResult_ptr_pri = new checkDataExecutionResult_c(this);
        }
        break;
    }
    return checkDataExecutionResult_ptr_pri;
}

checkDataExecutionResult_c* checkData_c::checkDataExecutionResult_ptr_f() const
{
    return checkDataExecutionResult_ptr_pri;
}

bool checkData_c::threaded_f() const
{
    return threaded_pri;
}

void checkData_c::setThreaded_f(const bool threaded_par_con)
{
    if (
        (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        or isExecuting_f()
        )
    {
        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            threaded_pri = threaded_par_con;
        }
    }
}

bool checkData_c::enabled_f() const
{
    return enabled_pri;
}

void checkData_c::setEnabled_f(const bool enabled_par_con)
{
    if (
        (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        or isExecuting_f()
        )
    {
        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            enabled_pri = enabled_par_con;
            //on disable delete the execution objects
            if (not enabled_pri)
            {
                deleteExecutionObjects_f();
            }
        }
    }
}

actionData_c* checkData_c::parentAction_f() const
{
    return parentAction_pri;
}

bool checkData_c::updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    bool updatedTmp(false);
    if (
        (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        or isExecuting_f()
        )
    {
        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
    }
    else
    {
        if (type_pri == checkType_ec::actionFinished)
        {
            actionFinishedCheck_c actionFinishedTmp;
            actionFinishedTmp.read_f(checkDataJSON_pri);
            if (actionFinishedTmp.actionStringId_f() == oldStringId_par_con)
            {
                actionFinishedTmp.setActionStringId_f(newStringId_par_con);
                actionFinishedTmp.write_f(checkDataJSON_pri);
                updatedTmp = true;
            }
        }
    }
    return updatedTmp;
}

bool checkData_c::hasStringIdAnyDependency_f(const QString& stringId_par_con) const
{
    bool resultTmp(false);
    if (type_pri == checkType_ec::actionFinished)
    {
        actionFinishedCheck_c actionFinishedTmp;
        actionFinishedTmp.read_f(checkDataJSON_pri);
        if (actionFinishedTmp.actionStringId_f() == stringId_par_con)
        {
            resultTmp = true;
        }
    }
    return resultTmp;
}

void checkData_c::deleteCheckDataExecutionObject_f()
{
    if (checkDataExecution_ptr_pri == nullptr)
    {

    }
    else
    {
        checkDataExecution_ptr_pri->deleteLater();
        checkDataExecution_ptr_pri = nullptr;
    }
}

void checkData_c::deleteCheckDataExecutionResultObject_f()
{
    if (checkDataExecutionResult_ptr_pri == nullptr)
    {

    }
    else
    {
        checkDataExecutionResult_ptr_pri->deleteLater();
        checkDataExecutionResult_ptr_pri = nullptr;
    }
}

void checkData_c::deleteExecutionObjects_f()
{
    deleteCheckDataExecutionObject_f();
    deleteCheckDataExecutionResultObject_f();
}

void checkData_c::deleteUsedPtrs_f()
{
    deleteExecutionObjects_f();
}

checkData_c::checkData_c(
        const checkType_ec type_par_con
        , const QString& uniqueIdString_par_con
        , const QString& description_par_con
        , const bool threaded_par_con
        , const QJsonObject& checkDataJson_par_con
        )
    : id_pri(nextCheckDataId_f())
    , type_pri(type_par_con)
    , uniqueIdString_pri(uniqueIdString_par_con)
    , description_pri(description_par_con)
    , threaded_pri(threaded_par_con)
    , checkDataJSON_pri(checkDataJson_par_con)
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " constructed", logItem_c::type_ec::debug);
}

checkData_c::checkData_c()
    : id_pri(nextCheckDataId_f())
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " constructed", logItem_c::type_ec::debug);
}

checkData_c::checkData_c(const checkData_c& from_par_con)
    : id_pri(from_par_con.id_pri)
    , type_pri(from_par_con.type_pri)
    , description_pri(from_par_con.description_pri)
    , threaded_pri(from_par_con.threaded_pri)
    , enabled_pri(from_par_con.enabled_pri)
    , checkDataJSON_pri(from_par_con.checkDataJSON_pri)
    , parentAction_pri(from_par_con.parentAction_pri)
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " copied (const)", logItem_c::type_ec::debug);
}

checkData_c::checkData_c(checkData_c& from_par)
    : id_pri(from_par.id_pri)
    , type_pri(from_par.type_pri)
    , description_pri(from_par.description_pri)
    , threaded_pri(from_par.threaded_pri)
    , enabled_pri(from_par.enabled_pri)
    , checkDataJSON_pri(from_par.checkDataJSON_pri)
    , parentAction_pri(from_par.parentAction_pri)
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " copied (no const)", logItem_c::type_ec::debug);
}

checkData_c::checkData_c(checkData_c&& from_par) noexcept
    : id_pri(from_par.id_pri)
    , type_pri(from_par.type_pri)
    , description_pri(std::move(from_par.description_pri))
    , threaded_pri(from_par.threaded_pri)
    , enabled_pri(from_par.enabled_pri)
    , checkDataJSON_pri(std::move(from_par.checkDataJSON_pri))
    , checkDataExecution_ptr_pri(from_par.checkDataExecution_ptr_pri)
    , checkDataExecutionResult_ptr_pri(from_par.checkDataExecutionResult_ptr_pri)
    , parentAction_pri(from_par.parentAction_pri)
{
    from_par.checkDataExecution_ptr_pri = nullptr;
    from_par.checkDataExecutionResult_ptr_pri = nullptr;

    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " constructed-moved", logItem_c::type_ec::debug);
}

checkData_c& checkData_c::operator=(const checkData_c& from_par_con)
{
    type_pri = from_par_con.type_pri;
    description_pri = from_par_con.description_pri;
    threaded_pri = from_par_con.threaded_pri;
    enabled_pri = from_par_con.enabled_pri;
    checkDataJSON_pri = from_par_con.checkDataJSON_pri;
    parentAction_pri = from_par_con.parentAction_pri;

    deleteUsedPtrs_f();
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " assigned= (const)", logItem_c::type_ec::debug);
    return *this;
}

checkData_c& checkData_c::operator=(checkData_c& from_par_con)
{
    type_pri = from_par_con.type_pri;
    description_pri = from_par_con.description_pri;
    threaded_pri = from_par_con.threaded_pri;
    enabled_pri = from_par_con.enabled_pri;
    checkDataJSON_pri = from_par_con.checkDataJSON_pri;
    parentAction_pri = from_par_con.parentAction_pri;

    deleteUsedPtrs_f();
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " assigned= (const)", logItem_c::type_ec::debug);
    return *this;
}

checkData_c&checkData_c::operator=(checkData_c&& from_par) noexcept
{
    //id_pri = std::move(from_par.id_pri);
    type_pri = from_par.type_pri;
    description_pri = std::move(from_par.description_pri);
    threaded_pri = from_par.threaded_pri;
    enabled_pri = from_par.enabled_pri;
    checkDataJSON_pri = std::move(from_par.checkDataJSON_pri);
    checkDataExecution_ptr_pri = from_par.checkDataExecution_ptr_pri;
    checkDataExecutionResult_ptr_pri = from_par.checkDataExecutionResult_ptr_pri;
    parentAction_pri = from_par.parentAction_pri;

    from_par.checkDataExecution_ptr_pri = nullptr;
    from_par.checkDataExecutionResult_ptr_pri = nullptr;
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " assigned-moved", logItem_c::type_ec::debug);
    return *this;
}

void checkData_c::setParentAction_f(actionData_c* parentAction_par)
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " parentAction is nullptr? " +  QSTRINGBOOL(parentAction_par not_eq nullptr), logItem_c::type_ec::debug);
    //if it's null or (it's not and it's not executing)
    if (parentAction_pri == nullptr or not parentAction_pri->isExecuting_f())
    {
        parentAction_pri = parentAction_par;
    }
}

checkData_c::~checkData_c()
{
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " destroyed", logItem_c::type_ec::debug);
    deleteUsedPtrs_f();
}

void checkData_c::write_f(QJsonObject& json) const
{
    json["type"] = checkTypeToStrUMap_glo_sta_con.at(type_pri);
    json["description"] = description_pri;
    json["threaded"] = threaded_pri;
    json["enabled"] = enabled_pri;
    json["checkDataJSON"] = checkDataJSON_pri;
    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " serialized", logItem_c::type_ec::debug);
}

void checkData_c::read_f(const QJsonObject& json)
{
    //no need since the time it's used it to load an empty checkData_c object which won't be execution or have any execution objects
//    if (
//        (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
//        or isExecuting_f()
//        )
//    {
//        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " is executing: " + QSTRINGBOOL(isExecuting_f()), logItem_c::type_ec::debug);
//    }
//    else
//    {
        type_pri = strToCheckTypeMap_glo_sta_con.value(json["type"].toString().toLower());
        description_pri = json["description"].toString();
        threaded_pri = json["threaded"].toBool();
        enabled_pri = json["enabled"].toBool();
        checkDataJSON_pri = json["checkDataJSON"].toObject();
        MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " serialized", logItem_c::type_ec::debug);
//    }
}

QString checkData_c::uniqueIdString_f() const
{
    return uniqueIdString_pri;
}

void checkData_c::setUniqueIdString_f(const QString& uniqueIdString_par_con)
{
    if (
        (parentAction_pri not_eq nullptr and parentAction_pri->isExecuting_f())
        or isExecuting_f()
        )
    {

    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            uniqueIdString_pri = uniqueIdString_par_con;
        }
    }
}

