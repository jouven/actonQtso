#include "checkData.hpp"

#include "actonDataHub.hpp"

#include "checkDataExecutionResult.hpp"

#include "checkMappings/checkMapping.hpp"
#include "checkExecution/actionFinishedExecution.hpp"
#include "checkExecution/sameFileExecution.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "essentialQtso/macros.hpp"
//#include "criptoQtso/hashQt.hpp"
//#include "signalso/signal.hpp"
#include "comuso/practicalTemplates.hpp"

#include <QJsonObject>
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

int_fast64_t check_c::id_f() const
{
    return id_pri;
}

const QMap<QString, checkData_c::resultLogic_ec> checkData_c::strToResultLogicMap_sta_con(
{
//Keys are lower case because this way when reading from json we can "lower" w/e is
//there and compare, allowing to ignore case
    { "trueonerror", checkData_c::resultLogic_ec::trueOnError}
    , {	"trueonsuccess", checkData_c::resultLogic_ec::trueOnSuccess}
    , {	"trueonfailure", checkData_c::resultLogic_ec::trueOnFailure}
    , {	"truealwaysexceptonerror", checkData_c::resultLogic_ec::trueAlwaysExceptOnError}
});

const std::unordered_map<checkData_c::resultLogic_ec, QString> checkData_c::resultLogicToStrUMap_sta_con(
{
    {	checkData_c::resultLogic_ec::trueOnError, "trueOnError" }
    , {	checkData_c::resultLogic_ec::trueOnSuccess, "trueOnSuccess" }
    , {	checkData_c::resultLogic_ec::trueOnFailure, "trueOnFailure" }
    , {	checkData_c::resultLogic_ec::trueAlwaysExceptOnError, "trueAlwaysExceptOnError" }

});

QString check_c::description_f() const
{
    return description_pro;
}

//QJsonObject& checkData_c::checkDataJSON_ref_f()
//{
//    return checkDataJSON_pri;
//}

bool check_c::isExecuting_f() const
{
    return checkDataExecutionResult_ptr_pri not_eq nullptr
            and checkDataExecution_ptr_pri not_eq nullptr
            and equalOnce_ft(checkDataExecutionResult_ptr_pri->lastState_f()
                            , checkExecutionState_ec::preparing
                            , checkExecutionState_ec::executing)
            and not checkDataExecutionResult_ptr_pri->finished_f();
}

bool check_c::isEditable_f() const
{
    return not isExecuting_f() or checkDataExecutionResult_ptr_pri == nullptr or checkDataExecutionResult_ptr_pri->lastState_f() == checkExecutionState_ec::initial;
}

//use only on sets
bool check_c::tryClearResultsOnEdit_f()
{
    bool resultTmp(true);
    if (not isEditable_f())
    {
        resultTmp = checkDataExecutionResult_ptr_pri->tryClear_f();
    }
    return resultTmp;
}

bool check_c::derivedUpdateStringIdDependencies_f(const QString&, const QString&)
{
    return false;
}

bool check_c::derivedHasStringIdAnyDependency_f(const QString&) const
{
    return false;
}

bool check_c::derivedUpdateStringTriggerDependecies_f(const QString&, const QString&)
{
    return false;
}

bool check_c::derivedHasStringTriggerAnyDependency_f(const QString& ) const
{
    return false;
}

std::vector<QString> check_c::derivedStringTriggersInUse_f() const
{
    return std::vector<QString>();
}

void check_c::setDescription_f(const QString& description_par_con)
{
    description_pro = description_par_con;
}

void check_c::prepareToRun_f()
{
    //IMPORTANT no stop checking here, like an action does, because checks don't run something else like an action with checks,
    //so it's not necessary, since the function uses the main-thread, which blocks the GUI, it's not possible to execute and immediately stop
    text_c textTmp("Check id: {0} preparing to run", this->id_pri);
    MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    //checkDataExecutionResult_ptr_pri->trySetExecutionState_f(checkExecutionState_ec::preparing);

    //IMPORTANT some check types barely use resources or their execution
    //depends on something "external" happening,
    //they will just wait for something to happen
    //i.e. finishedAction waits for an action to finish (connects
    //a signal to slot and that's it)
    //or query the OS for something trivial like: free space, a path exists, an enviroment variable...
    //the point here is that if these types of checks use threads, which are "finite",
    //they will block other action/threads which their execution depends on themselves and
    //use resources (CPU, memory, storage...)
    //the execution of these check types won't block the main thread

    //ADDENDUM this has the "secondary effect" of running the next action, row wise, before this check's action,
    //if there are no other checks that require a thread, example: action that has actionFinished check and the next action has no checks
    //even if the actionFinished executes instantly the next action will execute first
    //IT'S IMPORTANT TO GATE-ORDER ACTION EXECUTION USING actionFinished checks
    //ROW ORDER doesn't guarantee execution order
    if (equalOnce_ft(type_f()
                     , checkType_ec::actionFinished
                     , checkType_ec::timer
                     , checkType_ec::pathExists))
    {
        execute_f();
    }
    else
    {
        threadedFunction_c* threadedFunction_ptr = new threadedFunction_c(std::bind(&check_c::execute_f, this), true);
        QObject::connect(threadedFunction_ptr, &threadedFunction_c::finished, threadedFunction_ptr, &QObject::deleteLater);
        QObject::connect(checkDataExecutionResult_ptr_pri, &checkDataExecutionResult_c::finished_signal, threadedFunction_ptr, &threadedFunction_c::quit);
        threadedFunction_ptr->start();
    }
}

void check_c::setCheckDataExecutionNull_f()
{
    checkDataExecution_ptr_pri = nullptr;
}

void check_c::execute_f()
{
    while (true)
    {
        //this only affects sequential checks and thread-queued checks, checks that "started" but are waiting for a thread,
        //then something stopped the execution, then when a thread is available they will enter here
        if (checkDataExecutionResult_ptr_pri->lastState_f() == checkExecutionState_ec::stoppingByUser)
        {
            checkDataExecutionResult_ptr_pri->trySetFinished_f(false);
            break;
        }

        {
            text_c textTmp("Check Id: {0} execute", this->id_pri);
            MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::info);
        }

        checkDataExecution_ptr_pri = createExecutionObj_f(checkDataExecutionResult_ptr_pri);

        {
            text_c textTmp("Check Id: {0} execution object ctored", this->id_pri);
            MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::info);
        }

        QObject::connect(checkDataExecution_ptr_pri, &baseCheckExecution_c::destroyed, this, &check_c::setCheckDataExecutionNull_f );

        checkDataExecutionResult_ptr_pri->trySetExecutionState_f(checkExecutionState_ec::executing);
        checkDataExecution_ptr_pri->execute_f();
        break;
    }
}

void check_c::tryExecute_f()
{
    {
        text_c textTmp("Check Id: {0} try execute", this->id_pri);
        MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    }
    while (true)
    {
        //not enabled
        if (not enabled_pro)
        {
            text_c textTmp("Check Id: {0} can't run a not enabled Check", this->id_pri);
            MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::info);
            break;
        }
        else
        {
            //go on
        }
        //something is executing
        if (isExecuting_f())
        {
            text_c textTmp("Check Id: {0} already executing", this->id_pri);
            MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::info);
            break;
        }
        else
        {
            //go on
        }

        if (checkDataExecutionResult_ptr_pri == nullptr)
        {
            //initialize result obj if necessary
             checkDataExecutionResult_ptr_pri = new checkDataExecutionResult_c(this);
        }
        else
        {
            //when repeating execution

            //when running checks from checkDataHub_c this has been done previously
            //if checks are re-executed, if the first one finishes before the second is cleared (or any after before the next one),
            //checkdatahub_c::verifycheckresults might wrongly get that some checks have already finished (when they haven't)
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

void check_c::stopExecution_f()
{
    {
        text_c textTmp("Check Id: {0} try stop execution", this->id_pri);
        MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    }
    while (true)
    {
        if (not enabled_pro)
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
            text_c textTmp("Check Id: {0} stopped while preparing", this->id_pri);
            MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::info);
            break;
        }

        if (isExecuting_f())
        {
            text_c textTmp("Check Id: {0} stopping execution", this->id_pri);
            MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::info);
            checkDataExecutionResult_ptr_pri->trySetExecutionState_f(checkExecutionState_ec::stoppingByUser);
            checkDataExecution_ptr_pri->stop_f();
        }

        break;
    }
}

checkDataExecutionResult_c* check_c::createGetCheckDataExecutionResult_ptr_f()
{
    while (true)
    {
        if (not enabled_pro)
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

checkDataExecutionResult_c* check_c::checkDataExecutionResult_ptr_f() const
{
    return checkDataExecutionResult_ptr_pri;
}

bool check_c::threaded_f() const
{
    return threaded_pro;
}

void check_c::setThreaded_f(const bool threaded_par_con)
{
    if (
       isExecuting_f()
        )
    {
        text_c textTmp("Check Id: {0} is executing = {1}", this->id_pri, QSTRINGBOOL(isExecuting_f()));
        MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            threaded_pro = threaded_par_con;
        }
    }
}

bool check_c::enabled_f() const
{
    return enabled_pro;
}

void check_c::setEnabled_f(const bool enabled_par_con)
{
    if (
       isExecuting_f()
        )
    {
        text_c textTmp("Check Id: {0} is executing = {1}", this->id_pri, QSTRINGBOOL(isExecuting_f()));
        MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    }
    else
    {
        if (tryClearResultsOnEdit_f())
        {
            enabled_pro = enabled_par_con;
            //on disable delete the execution objects
            if (not enabled_pro)
            {
                deleteExecutionObjects_f();
            }
        }
    }
}

//action_c* check_c::parentAction_f() const
//{
//    return parentAction_pri;
//}

bool check_c::updateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    bool updatedTmp(false);
    if (
        isExecuting_f()
        )
    {
        text_c textTmp("Check Id: {0} is executing = {1}", this->id_pri, QSTRINGBOOL(isExecuting_f()));
        MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    }
    else
    {
        updatedTmp = derivedUpdateStringIdDependencies_f(newStringId_par_con, oldStringId_par_con);
    }
    return updatedTmp;
}

bool check_c::hasStringIdAnyDependency_f(const QString& stringId_par_con) const
{
    return derivedHasStringIdAnyDependency_f(stringId_par_con);
}

bool check_c::updateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
{
    bool resultTmp(false);
    if (
        isExecuting_f()
        )
    {
        text_c textTmp("Check Id: {0} is executing = {1}", this->id_pri, QSTRINGBOOL(isExecuting_f()));
        MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    }
    else
    {
        resultTmp = derivedUpdateStringTriggerDependecies_f(newStringTrigger_par_con, oldStringTrigger_par_con);
    }
    return resultTmp;
}

bool check_c::hasStringTriggerAnyDependency_f(const QString& stringTrigger_par_con) const
{
    return derivedHasStringTriggerAnyDependency_f(stringTrigger_par_con);
}

std::vector<QString> check_c::stringTriggersInUse_f() const
{
    return derivedStringTriggersInUse_f();
}

check_c* check_c::readCreateDerived_f(const checkType_ec checkType_par_con)
{
    check_c* resultTmp(nullptr);
    auto findResultTmp(checkTypeToCheckCreationFunctionMap_ext_con.find(checkType_par_con));
    if (findResultTmp not_eq checkTypeToCheckCreationFunctionMap_ext_con.cend())
    {
        resultTmp = findResultTmp->second();
    }
    else
    {
        //nullptr is the default value
    }
    return resultTmp;
}

check_c* check_c::clone_f() const
{
    return derivedClone_f();
}

bool check_c::isFieldsCheckValid_f(textCompilation_c* errorsPtr_par) const
{
    textCompilation_c errorsTmp;
    textCompilation_c* errorsTmpPtr(errorsPtr_par == nullptr ? nullptr : std::addressof(errorsTmp));
    bool isValidResultTmp(
                checkData_c::isFieldsCheckDataValid_f(errorsTmpPtr)
                and derivedIsValidCheck_f(errorsTmpPtr));
    if (errorsPtr_par not_eq nullptr and not errorsTmp.empty_f())
    {
        text_c errorTmp("Error/s found in check Id {0}", this->id_pri);
        errorsPtr_par->append_f(errorTmp);
        errorsPtr_par->append_f(errorsTmp);
    }
    else
    {
        if (not isValidResultTmp)
        {
            text_c logTextTmp("Check Id: \"{0}\" is not valid but also has no errors", this->id_pri);
            MACRO_ADDACTONQTSOLOG(logTextTmp, logItem_c::type_ec::warning);
        }
        else
        {
            //if it's valid nothing to do
        }
    }
    return isValidResultTmp;
}

void check_c::updateCheckData_f(const checkData_c& checkData_par_con)
{
    this->checkData_c::operator=(checkData_par_con);
}

void check_c::deleteCheckDataExecutionObject_f()
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

void check_c::deleteCheckDataExecutionResultObject_f()
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

void check_c::deleteExecutionObjects_f()
{
    deleteCheckDataExecutionObject_f();
    deleteCheckDataExecutionResultObject_f();
}

void check_c::deleteUsedPtrs_f()
{
    deleteExecutionObjects_f();
}

//check_c::check_c(
//        const checkType_ec type_par_con
//        , const QString& uniqueIdString_par_con
//        , const QString& description_par_con
//        , const bool threaded_par_con
//        , const QJsonObject& checkDataJson_par_con
//        )
//    : id_pri(nextCheckDataId_f())
//    , type_pri(type_par_con)
//    , uniqueIdString_pri(uniqueIdString_par_con)
//    , description_pri(description_par_con)
//    , threaded_pri(threaded_par_con)
//    , checkDataJSON_pri(checkDataJson_par_con)
//{
//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " constructed", logItem_c::type_ec::debug);
//}

check_c::check_c(
        const checkData_c& checkData_par_con
)
    : checkData_c(checkData_par_con)
    , id_pri(nextCheckDataId_f())
{
    text_c textTmp("Check Id: {0} constructed", this->id_pri);
    MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
}

check_c::check_c()
    : check_c(checkData_c())
{
    text_c textTmp("Check Id: {0} constructed (empty constructor)", this->id_pri);
    MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
}


//check_c::check_c(const check_c& from_par_con)
//    : id_pri(from_par_con.id_pri)
//    , type_pri(from_par_con.type_pri)
//    , description_pri(from_par_con.description_pri)
//    , threaded_pri(from_par_con.threaded_pri)
//    , enabled_pri(from_par_con.enabled_pri)
//    , checkDataJSON_pri(from_par_con.checkDataJSON_pri)
//    , parentAction_pri(from_par_con.parentAction_pri)
//{
//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " copied (const)", logItem_c::type_ec::debug);
//}

//check_c::check_c(check_c& from_par)
//    : id_pri(from_par.id_pri)
//    , type_pri(from_par.type_pri)
//    , description_pri(from_par.description_pri)
//    , threaded_pri(from_par.threaded_pri)
//    , enabled_pri(from_par.enabled_pri)
//    , checkDataJSON_pri(from_par.checkDataJSON_pri)
//    , parentAction_pri(from_par.parentAction_pri)
//{
//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " copied (no const)", logItem_c::type_ec::debug);
//}

//check_c::check_c(check_c&& from_par) noexcept
//    : id_pri(from_par.id_pri)
//    , type_pri(from_par.type_pri)
//    , description_pri(std::move(from_par.description_pri))
//    , threaded_pri(from_par.threaded_pri)
//    , enabled_pri(from_par.enabled_pri)
//    , checkDataJSON_pri(std::move(from_par.checkDataJSON_pri))
//    , checkDataExecution_ptr_pri(from_par.checkDataExecution_ptr_pri)
//    , checkDataExecutionResult_ptr_pri(from_par.checkDataExecutionResult_ptr_pri)
//    , parentAction_pri(from_par.parentAction_pri)
//{
//    from_par.checkDataExecution_ptr_pri = nullptr;
//    from_par.checkDataExecutionResult_ptr_pri = nullptr;

//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " constructed-moved", logItem_c::type_ec::debug);
//}

//check_c& check_c::operator=(const check_c& from_par_con)
//{
//    type_pri = from_par_con.type_pri;
//    description_pri = from_par_con.description_pri;
//    threaded_pri = from_par_con.threaded_pri;
//    enabled_pri = from_par_con.enabled_pri;
//    checkDataJSON_pri = from_par_con.checkDataJSON_pri;
//    parentAction_pri = from_par_con.parentAction_pri;

//    deleteUsedPtrs_f();
//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " assigned= (const)", logItem_c::type_ec::debug);
//    return *this;
//}

//check_c& check_c::operator=(check_c& from_par_con)
//{
//    type_pri = from_par_con.type_pri;
//    description_pri = from_par_con.description_pri;
//    threaded_pri = from_par_con.threaded_pri;
//    enabled_pri = from_par_con.enabled_pri;
//    checkDataJSON_pri = from_par_con.checkDataJSON_pri;
//    parentAction_pri = from_par_con.parentAction_pri;

//    deleteUsedPtrs_f();
//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " assigned= (const)", logItem_c::type_ec::debug);
//    return *this;
//}

//check_c&check_c::operator=(check_c&& from_par) noexcept
//{
//    //id_pri = std::move(from_par.id_pri);
//    type_pri = from_par.type_pri;
//    description_pri = std::move(from_par.description_pri);
//    threaded_pri = from_par.threaded_pri;
//    enabled_pri = from_par.enabled_pri;
//    checkDataJSON_pri = std::move(from_par.checkDataJSON_pri);
//    checkDataExecution_ptr_pri = from_par.checkDataExecution_ptr_pri;
//    checkDataExecutionResult_ptr_pri = from_par.checkDataExecutionResult_ptr_pri;
//    parentAction_pri = from_par.parentAction_pri;

//    from_par.checkDataExecution_ptr_pri = nullptr;
//    from_par.checkDataExecutionResult_ptr_pri = nullptr;
//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " assigned-moved", logItem_c::type_ec::debug);
//    return *this;
//}

//void check_c::setParentAction_f(action_c* parentAction_par)
//{
//    MACRO_ADDACTONQTSOLOG("Check Id: " + QString::number(this->id_pri) + " parentAction is nullptr? " +  QSTRINGBOOL(parentAction_par not_eq nullptr), logItem_c::type_ec::debug);
//    //if it's null or (it's not and it's not executing)
//    if (parentAction_pri == nullptr or not parentAction_pri->isExecuting_f())
//    {
//        parentAction_pri = parentAction_par;
//    }
//}

check_c::~check_c()
{
    text_c textTmp("Check Id: {0} destroyed", this->id_pri);
    MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
    deleteUsedPtrs_f();
}

void check_c::write_f(QJsonObject& json_ref_par) const
{
    json_ref_par["type"] = typeStr_f();
    //json_ref_par["uniqueIdString"] = uniqueIdString_pro;
    json_ref_par["description"] = description_pro;
    json_ref_par["threaded"] = threaded_pro;
    json_ref_par["enabled"] = enabled_pro;
    QJsonObject derivedObjTmp;
    derivedWrite_f(derivedObjTmp);
    json_ref_par["checkTypeData"] = derivedObjTmp;

    text_c textTmp("Check Id: {0} serialized", this->id_pri);
    MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
}

void check_c::read_f(const QJsonObject& json_par_con)
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
    //uniqueIdString_pro = json_par_con["uniqueIdString"].toString();
        description_pro = json_par_con["description"].toString();
        if (json_par_con["threaded"].isBool())
        {
            threaded_pro = json_par_con["threaded"].toBool();
        }
        if (json_par_con["enabled"].isBool())
        {
            enabled_pro = json_par_con["enabled"].toBool();
        }
        derivedRead_f(json_par_con["checkTypeData"].toObject());

        text_c textTmp("Check Id: {0} deserialized", this->id_pri);
        MACRO_ADDACTONQTSOLOG(textTmp, logItem_c::type_ec::debug);
//    }
}

//QString check_c::uniqueIdString_f() const
//{
//    return uniqueIdString_pro;
//}

//void check_c::setUniqueIdString_f(const QString& uniqueIdString_par_con)
//{
//    if (
//        isExecuting_f()
//        )
//    {

//    }
//    else
//    {
//        if (tryClearResultsOnEdit_f())
//        {
//            uniqueIdString_pro = uniqueIdString_par_con;
//        }
//    }
//}

checkData_c::resultLogic_ec check_c::resultLogic_f() const
{
    return resultLogic_pro;
}

void check_c::setResultLogic_f(const checkData_c::resultLogic_ec& resultLogic_par_con)
{
    resultLogic_pro = resultLogic_par_con;
}

//checkData_c::checkData_c(
//        const checkData_c* const source_par_con
//        ) :
//    id_pro(nextCheckDataId_f())
//  , uniqueIdString_pro(source_par_con->uniqueIdString_pro)
//  , description_pro(source_par_con->description_pro)
//  , threaded_pro(source_par_con->threaded_pro)
//  , enabled_pro(source_par_con->threaded_pro)
//  , resultLogic_pro(source_par_con->resultLogic_pro)
//{
//    MACRO_ADDACTONQTSOLOG("checkData_c, \"constructed\" (\"derived constructor\")", logItem_c::type_ec::debug);
//}

checkData_c::checkData_c(
        //const QString& uniqueIdString_par_con
        const QString& description_par_con
        , const bool threaded_par_con
        , const bool enabled_par_con
        , const resultLogic_ec resultLogic_par_con)
    : //id_pro(nextCheckDataId_f()),
      //uniqueIdString_pro(uniqueIdString_par_con)
    description_pro(description_par_con)
    , threaded_pro(threaded_par_con)
    , enabled_pro(enabled_par_con)
    , resultLogic_pro(resultLogic_par_con)
{

}

bool checkData_c::isFieldsCheckDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
//        if (stringId_pro.isEmpty())
//        {
//            APPENDTEXTPTR(errorsPtr_par, "StringId is empty")
//            break;
//        }
        if (description_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Description is empty")
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}
