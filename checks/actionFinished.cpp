#include "actionFinished.hpp"

#include "../checkExecution/actionFinishedExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

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

int_fast64_t actionFinishedData_c::finishedCount_f() const
{
    return finishedCount_pro;
}

void actionFinishedData_c::setFinishedCount_f(const int_fast64_t finishedCount_par_con)
{
    finishedCount_pro = finishedCount_par_con;
}

bool actionFinishedData_c::successOnActionSuccess_f() const
{
    return successOnActionSuccess_pro;
}

void actionFinishedData_c::setSuccessOnActionSuccess_f(const bool successOnActionSuccess_par_con)
{
    successOnActionSuccess_pro = successOnActionSuccess_par_con;
}

bool actionFinishedData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        if (actionStringIdParsed_f().isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Action stringId is empty")
            break;
        }
        //TODO-FUTURE(see last comment line) there needs to be a hard validation and a soft validation
        //to deal with dynamic stuff like stringparser results used in a field
        //before executing a hard validation should be done to check if the chain of parsing
        //basically all fields that are stringParsed must be gathered and checked for against all possible actionExecutionResultFieldToStringPair_ite_con
        //and what's inside the stringparser obj
        //do it as a separate option because it can become pretty heavy?, parsing all the strings and for each one search all the possible string triggers

        if (finishedCount_pro < 0)
        {
            text_c errorTextTmp("Wrong finished count value: {0}, value must be between 0 and INT64MAX", finishedCount_pro);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
            break;
        }

        bool emptyMappingFoundTmp(false);
        for (const std::pair<const actionFinishedData_c::actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringPair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
        {
            if (actionExecutionResultFieldToStringPair_ite_con.second.isEmpty())
            {
                emptyMappingFoundTmp = true;
                text_c errorTextTmp("Action execution result field to empty string trigger mapping found: {0}"
                                    , actionExecutionResultFieldsToStrUMap_sta_con.at(actionExecutionResultFieldToStringPair_ite_con.first));
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            }
        }
        if (emptyMappingFoundTmp)
        {
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

actionFinishedData_c::actionFinishedData_c(
        const QString& actionStringId_par_con
        , const int_fast64_t finishedCount_par_con
        , const bool successOnActionSuccess_par_con
        , const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con)
    : actionStringId_pro(actionStringId_par_con)
    , finishedCount_pro(finishedCount_par_con)
    , successOnActionSuccess_pro(successOnActionSuccess_par_con)
    , actionExecutionResultFieldToStringTrigger_pro(actionExecutionResultFieldToStringTrigger_par_con)
{}

void actionFinishedCheck_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["actionStringId"] = actionStringId_pro;
    json_par["finishedCount"] = QString::number(finishedCount_pro);
    json_par["successOnActionSuccess"] = successOnActionSuccess_pro;
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
    if (json_par_con["actionStringId"].isString())
    {
        actionStringId_pro = json_par_con["actionStringId"].toString();
    }
    if (json_par_con["finishedCount"].isString())
    {
        finishedCount_pro = json_par_con["finishedCount"].toString().toLongLong();
    }
    if (json_par_con["successOnActionSuccess"].isBool())
    {
        successOnActionSuccess_pro = json_par_con["successOnActionSuccess"].toBool();
    }

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

bool actionFinishedCheck_c::derivedIsValidCheck_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
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
    return checkTypeToStrUMap_ext_con.at(type_f());
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

void actionFinishedCheck_c::updateActionFinishedData_f(
        const actionFinishedData_c& actionFinishedData_par_con
        , actonDataHub_c* actonDataHubPtr_par
)
{
    if (actonDataHubPtr_par not_eq nullptr)
    {
        QObject::connect(this
                         , &actionFinishedCheck_c::stringTriggerChanged_signal
                         , actonDataHubPtr_par
                         , &actonDataHub_c::updateStringTriggerDependencies_f
                         , Qt::UniqueConnection);
    }
    std::unordered_map<actionExecutionResultFields_ec, QString> oldTriggerValuesTmp(actionExecutionResultFieldToStringTrigger_pro);
    this->actionFinishedData_c::operator=(actionFinishedData_par_con);
    for (const auto& actionResultField_trigger_pair_ite_con : oldTriggerValuesTmp)
    {
        const QString newValueTmp(actionExecutionResultFieldToStringTrigger_pro.at(actionResultField_trigger_pair_ite_con.first));
        const QString oldValueTmp(actionResultField_trigger_pair_ite_con.second);
        if (newValueTmp not_eq oldValueTmp)
        {
            Q_EMIT stringTriggerChanged_signal(newValueTmp, oldValueTmp);
        }
    }
}


actionFinishedCheck_c::actionFinishedCheck_c(
        const checkData_c& checkData_par_con
        , const actionFinishedData_c& actionFinished_par_con)
    : check_c(checkData_par_con)
    , actionFinishedData_c(actionFinished_par_con)
{
}
