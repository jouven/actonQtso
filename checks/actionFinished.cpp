#include "actionFinished.hpp"

#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include <QJsonArray>

const QMap<QString, actionFinishedCheck_c::actionExecutionResultFields_ec> actionFinishedCheck_c::strToActionExecutionResultFieldsMap_sta_con(
{
//Keys are lower case because this way when reading from json the strings can be lower-cased
//and compared, allowing to ignore case
    {	"endstate", actionFinishedCheck_c::actionExecutionResultFields_ec::endState}
    , {	"errorstr", actionFinishedCheck_c::actionExecutionResultFields_ec::errorStr}
    , {	"outputstr", actionFinishedCheck_c::actionExecutionResultFields_ec::outputStr}
    , {	"returncode", actionFinishedCheck_c::actionExecutionResultFields_ec::returnCode}
    , {	"enddatetime", actionFinishedCheck_c::actionExecutionResultFields_ec::endDateTime}
    , {	"startdatetime", actionFinishedCheck_c::actionExecutionResultFields_ec::startDateTime}
    , {	"externalerrorstr", actionFinishedCheck_c::actionExecutionResultFields_ec::externalErrorStr}
    , {	"externaloutputstr", actionFinishedCheck_c::actionExecutionResultFields_ec::externalOutputStr}
});

const std::unordered_map<actionFinishedCheck_c::actionExecutionResultFields_ec, QString> actionFinishedCheck_c::actionExecutionResultFieldsToStrUMap_sta_con(
{
    {	actionFinishedCheck_c::actionExecutionResultFields_ec::endState, "endState" }
    , {	actionFinishedCheck_c::actionExecutionResultFields_ec::errorStr, "errorStr" }
    , {	actionFinishedCheck_c::actionExecutionResultFields_ec::outputStr, "outputStr" }
    , {	actionFinishedCheck_c::actionExecutionResultFields_ec::returnCode, "returnCode" }
    , {	actionFinishedCheck_c::actionExecutionResultFields_ec::endDateTime, "endDateTime" }
    , {	actionFinishedCheck_c::actionExecutionResultFields_ec::startDateTime, "startDateTime" }
    , {	actionFinishedCheck_c::actionExecutionResultFields_ec::externalErrorStr, "externalErrorStr" }
    , {	actionFinishedCheck_c::actionExecutionResultFields_ec::externalOutputStr, "externalOutputStr" }
});

QString actionFinishedCheck_c::actionStringId_f() const
{
    return actionStringId_pri;
}

QString actionFinishedCheck_c::actionStringIdParsed_f() const
{
    COPYPARSERETURNVAR(actionStringId_pri);
}

void actionFinishedCheck_c::setActionStringId_f(const QString& actionStringId_par_con)
{
    actionStringId_pri = actionStringId_par_con;
}

bool actionFinishedCheck_c::failCheckOnNotSuccessfulActionFinish_f() const
{
    return failCheckOnNotSuccessfulActionFinish_pri;
}

void actionFinishedCheck_c::setFailCheckOnNotSuccessfulActionFinish_f(const bool failCheckOnNotSuccessfulActionFinish_par_con)
{
    failCheckOnNotSuccessfulActionFinish_pri = failCheckOnNotSuccessfulActionFinish_par_con;
}

bool actionFinishedCheck_c::mapActionResultToStringParserConfig_f(
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

        if (actonDataHub_ptr_ext->hasStringTriggerAnyDependency_f(stringTrigger_par_con))
        {
            //other checks are already using this string trigger
            break;
        }

        resultTmp = true;
        actionExecutionResultFieldToStringTrigger_pri.emplace(actionExecutionResultField_par_con, stringTrigger_par_con);
        break;
    }
    return resultTmp;
}

void actionFinishedCheck_c::removeMapActionResultToStringParserConfig_f(const actionFinishedCheck_c::actionExecutionResultFields_ec actionExecutionResultField_par_con)
{
    actionExecutionResultFieldToStringTrigger_pri.erase(actionExecutionResultField_par_con);
}

std::unordered_map<actionFinishedCheck_c::actionExecutionResultFields_ec, QString> actionFinishedCheck_c::actionExecutionResultFieldToStringTrigger_f() const
{
    return actionExecutionResultFieldToStringTrigger_pri;
}

void actionFinishedCheck_c::setActionExecutionResultFieldToStringTrigger_f(const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con)
{
    actionExecutionResultFieldToStringTrigger_pri = actionExecutionResultFieldToStringTrigger_par_con;
}

actionFinishedCheck_c::actionFinishedCheck_c(
        const QString& actionStringId_par_con
        , const bool failCheckOnNotSuccessfulActionFinish_par_con
        , const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con)
    : actionStringId_pri(actionStringId_par_con)
    , failCheckOnNotSuccessfulActionFinish_pri(failCheckOnNotSuccessfulActionFinish_par_con)
    , actionExecutionResultFieldToStringTrigger_pri(actionExecutionResultFieldToStringTrigger_par_con)
{}

void actionFinishedCheck_c::write_f(QJsonObject& json_par) const
{
    json_par["actionStringId"] = actionStringId_pri;
    json_par["failCheckOnNotSuccessfulActionFinish"] = failCheckOnNotSuccessfulActionFinish_pri;
    if (not actionExecutionResultFieldToStringTrigger_pri.empty())
    {
        QJsonArray actionExecutionResultFieldToStringTriggerPairArray;
        for (const std::pair<const actionFinishedCheck_c::actionExecutionResultFields_ec, QString>& ite_par_con : actionExecutionResultFieldToStringTrigger_pri)
        {
            QJsonObject pairTmp;
            pairTmp["actionResultField"] = actionExecutionResultFieldsToStrUMap_sta_con.at(ite_par_con.first);
            pairTmp["stringTrigger"] = ite_par_con.second;
            actionExecutionResultFieldToStringTriggerPairArray.append(pairTmp);
        }
        json_par["actionExecutionResultFieldToStringTrigger"] = actionExecutionResultFieldToStringTriggerPairArray;
    }
}

void actionFinishedCheck_c::read_f(const QJsonObject& json_par_con)
{
    actionStringId_pri = json_par_con["actionStringId"].toString();
    failCheckOnNotSuccessfulActionFinish_pri = json_par_con["failCheckOnNotSuccessfulActionFinish"].toBool();
    if (not json_par_con["actionExecutionResultFieldToStringTrigger"].isUndefined())
    {
        QJsonArray pairsTmp(json_par_con["actionExecutionResultFieldToStringTrigger"].toArray());
        if (not pairsTmp.isEmpty())
        {
            actionExecutionResultFieldToStringTrigger_pri.reserve(pairsTmp.size());
            for (const QJsonValueRef& jsonArrayItem_ite_con : pairsTmp)
            {
                QJsonObject jsonObjectTmp(jsonArrayItem_ite_con.toObject());
                actionExecutionResultFieldToStringTrigger_pri.emplace(
                            strToActionExecutionResultFieldsMap_sta_con.value(jsonObjectTmp["actionResultField"].toString().toLower())
                        , jsonObjectTmp["stringTrigger"].toString()
                );
            }
        }
    }
}
