#include "actionFinished.hpp"

#include "../checkExecution/actionFinishedExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include <QJsonObject>
#include <QJsonArray>

const QMap<QString, actionFinishedData_c::actionExecutionResultFields_ec> actionFinishedData_c::strToActionExecutionResultFieldsMap_sta_con(
{
//Keys are lower case because this way when reading from json the strings can be lower-cased
//and compared, allowing to ignore case
    {	"endstate", actionFinishedData_c::actionExecutionResultFields_ec::endState}
    , {	"errorstr", actionFinishedData_c::actionExecutionResultFields_ec::errorStr}
    , {	"outputstr", actionFinishedData_c::actionExecutionResultFields_ec::outputStr}
    , {	"returncode", actionFinishedData_c::actionExecutionResultFields_ec::returnCode}
    , {	"enddatetime", actionFinishedData_c::actionExecutionResultFields_ec::endDateTime}
    , {	"startdatetime", actionFinishedData_c::actionExecutionResultFields_ec::startDateTime}
    , {	"externalerrorstr", actionFinishedData_c::actionExecutionResultFields_ec::externalErrorStr}
    , {	"externaloutputstr", actionFinishedData_c::actionExecutionResultFields_ec::externalOutputStr}
});

const std::unordered_map<actionFinishedData_c::actionExecutionResultFields_ec, QString> actionFinishedData_c::actionExecutionResultFieldsToStrUMap_sta_con(
{
    {	actionFinishedData_c::actionExecutionResultFields_ec::endState, "endState" }
    , {	actionFinishedData_c::actionExecutionResultFields_ec::errorStr, "errorStr" }
    , {	actionFinishedData_c::actionExecutionResultFields_ec::outputStr, "outputStr" }
    , {	actionFinishedData_c::actionExecutionResultFields_ec::returnCode, "returnCode" }
    , {	actionFinishedData_c::actionExecutionResultFields_ec::endDateTime, "endDateTime" }
    , {	actionFinishedData_c::actionExecutionResultFields_ec::startDateTime, "startDateTime" }
    , {	actionFinishedData_c::actionExecutionResultFields_ec::externalErrorStr, "externalErrorStr" }
    , {	actionFinishedData_c::actionExecutionResultFields_ec::externalOutputStr, "externalOutputStr" }
});

QString actionFinishedData_c::actionStringId_f() const
{
    return actionStringId_pro;
}

QString actionFinishedData_c::actionStringIdParsed_f() const
{
    COPYPARSERETURNVAR(actionStringId_pro);
}

void actionFinishedData_c::setActionStringId_f(const QString& actionStringId_par_con)
{
    actionStringId_pro = actionStringId_par_con;
}

bool actionFinishedData_c::failCheckOnNotSuccessfulActionFinish_f() const
{
    return failCheckOnNotSuccessfulActionFinish_pro;
}

void actionFinishedData_c::setFailCheckOnNotSuccessfulActionFinish_f(const bool failCheckOnNotSuccessfulActionFinish_par_con)
{
    failCheckOnNotSuccessfulActionFinish_pro = failCheckOnNotSuccessfulActionFinish_par_con;
}

bool actionFinishedData_c::mapActionResultToStringParserConfig_f(
        const actionFinishedCheck_c::actionExecutionResultFields_ec actionExecutionResultField_par_con
        , const QString& stringTrigger_par_con
)
{
    bool resultTmp(false);
    while (true)
    {
        if (actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->stringTriggerIndex_f(stringTrigger_par_con) not_eq -1)
        {
            //something with this string trigger already exists on the parser configs, do nothing
            break;
        }

        if (actonDataHub_ptr_ext->hasStringTriggerAnyDependency_f(stringTrigger_par_con, nullptr))
        {
            //other checks are already using this string trigger
            break;
        }

        resultTmp = true;
        actionExecutionResultFieldToStringTrigger_pro.emplace(actionExecutionResultField_par_con, stringTrigger_par_con);
        break;
    }
    return resultTmp;
}

void actionFinishedData_c::removeMapActionResultToStringParserConfig_f(const actionFinishedCheck_c::actionExecutionResultFields_ec actionExecutionResultField_par_con)
{
    actionExecutionResultFieldToStringTrigger_pro.erase(actionExecutionResultField_par_con);
}

std::unordered_map<actionFinishedCheck_c::actionExecutionResultFields_ec, QString> actionFinishedData_c::actionExecutionResultFieldToStringTrigger_f() const
{
    return actionExecutionResultFieldToStringTrigger_pro;
}

void actionFinishedData_c::setActionExecutionResultFieldToStringTrigger_f(const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con)
{
    actionExecutionResultFieldToStringTrigger_pro = actionExecutionResultFieldToStringTrigger_par_con;
}

actionFinishedData_c::actionFinishedData_c(
        const QString& actionStringId_par_con
        , const bool failCheckOnNotSuccessfulActionFinish_par_con
        , const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con)
    : actionStringId_pro(actionStringId_par_con)
    , failCheckOnNotSuccessfulActionFinish_pro(failCheckOnNotSuccessfulActionFinish_par_con)
    , actionExecutionResultFieldToStringTrigger_pro(actionExecutionResultFieldToStringTrigger_par_con)
{}

void actionFinishedCheck_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["actionStringId"] = actionStringId_pro;
    json_par["failCheckOnNotSuccessfulActionFinish"] = failCheckOnNotSuccessfulActionFinish_pro;
    if (not actionExecutionResultFieldToStringTrigger_pro.empty())
    {
        QJsonArray actionExecutionResultFieldToStringTriggerPairArray;
        for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& ite_par_con : actionExecutionResultFieldToStringTrigger_pro)
        {
            QJsonObject pairTmp;
            pairTmp["actionResultField"] = actionExecutionResultFieldsToStrUMap_sta_con.at(ite_par_con.first);
            pairTmp["stringTrigger"] = ite_par_con.second;
            actionExecutionResultFieldToStringTriggerPairArray.append(pairTmp);
        }
        json_par["actionExecutionResultFieldToStringTrigger"] = actionExecutionResultFieldToStringTriggerPairArray;
    }
}

void actionFinishedCheck_c::derivedRead_f(const QJsonObject& json_par_con)
{
    actionStringId_pro = json_par_con["actionStringId"].toString();
    failCheckOnNotSuccessfulActionFinish_pro = json_par_con["failCheckOnNotSuccessfulActionFinish"].toBool();
    if (not json_par_con["actionExecutionResultFieldToStringTrigger"].isUndefined())
    {
        QJsonArray pairsTmp(json_par_con["actionExecutionResultFieldToStringTrigger"].toArray());
        if (not pairsTmp.isEmpty())
        {
            actionExecutionResultFieldToStringTrigger_pro.reserve(pairsTmp.size());
            for (const QJsonValueRef& jsonArrayItem_ite_con : pairsTmp)
            {
                QJsonObject jsonObjectTmp(jsonArrayItem_ite_con.toObject());
                actionExecutionResultFieldToStringTrigger_pro.emplace(
                            strToActionExecutionResultFieldsMap_sta_con.value(jsonObjectTmp["actionResultField"].toString().toLower())
                        , jsonObjectTmp["stringTrigger"].toString()
                );
            }
        }
    }
}

check_c* actionFinishedCheck_c::derivedClone_f() const
{
    //slice and dice
    actionFinishedData_c actionFinishedTmp(*this);
    checkData_c checkDataTmp(*this);
    actionFinishedCheck_c* resultTmp(new actionFinishedCheck_c(checkDataTmp, actionFinishedTmp));
    //clear this field because otherwise it will conflict with the original object
    resultTmp->actionExecutionResultFieldToStringTrigger_pro.clear();
    return resultTmp;
}

baseCheckExecution_c* actionFinishedCheck_c::createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par)
{
    return new actionFinishedCheckExecution_c(checkDataExecutionResult_ptr_par, this);
}

checkType_ec actionFinishedCheck_c::type_f() const
{
    return checkType_ec::actionFinished;
}

QString actionFinishedCheck_c::typeStr_f() const
{
    return checkTypeToStrUMap_glo_sta_con.at(checkType_ec::actionFinished);
}

bool actionFinishedCheck_c::derivedUpdateStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    bool updatedTmp(false);
    if (actionStringId_pro == oldStringId_par_con)
    {
        actionStringId_pro = newStringId_par_con;
        updatedTmp = true;
    }
    return updatedTmp;
}

bool actionFinishedCheck_c::derivedHasStringIdAnyDependency_f(const QString& stringId_par_con) const
{
    bool resultTmp(false);
    if (actionStringId_pro == stringId_par_con)
    {
        resultTmp = true;
    }
    return resultTmp;
}

bool actionFinishedCheck_c::derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
{
    //int_fast32_t updateCountTmp(0);
    bool somethingChangedTmp(false);
    for (std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& pair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
    {
        if (pair_ite_con.second == oldStringTrigger_par_con)
        {
            pair_ite_con.second = newStringTrigger_par_con;
            somethingChangedTmp = true;
            //updateCountTmp = updateCountTmp + 1;
        }
    }
    return somethingChangedTmp;
}

bool actionFinishedCheck_c::derivedHasStringTriggerAnyDependency_f(const QString& stringTrigger_par_con) const
{
    bool resultTmp(false);
    for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& pair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
    {
        if (pair_ite_con.second == stringTrigger_par_con)
        {
            resultTmp = true;
            break;
        }
    }
    return resultTmp;
}

std::vector<QString> actionFinishedCheck_c::derivedStringTriggersInUse_f() const
{
    std::vector<QString> keyStringsTmp;
    for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& pair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
    {
        keyStringsTmp.emplace_back(pair_ite_con.second);
    }
    return keyStringsTmp;
}


actionFinishedCheck_c::actionFinishedCheck_c(
        const checkData_c& checkData_par_con
        , const actionFinishedData_c& actionFinished_par_con)
    : check_c(checkData_par_con)
    , actionFinishedData_c(actionFinished_par_con)
{
}
