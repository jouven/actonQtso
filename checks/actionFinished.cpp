#include "actionFinished.hpp"

#include "../checkExecution/actionFinishedExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"
#include "../reused/stringAlgo.hpp"

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

//QString actionFinishedData_c::actionStringIdParsed_f() const
//{
//    COPYPARSERETURNVAR(actionStringId_pro);
//}

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
        if (actonDataHub_ptr_ext->stringTriggerCreationConflict_f(stringTrigger_par_con).first not_eq -1)
        {
            //in use in stringParserMap or other actions/checks are already using this string trigger
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

//std::unordered_map<actionFinishedData_c::actionExecutionResultFields_ec, QString> actionFinishedData_c::actionExecutionResultFieldToStringTriggerParsed_f() const
//{
//    std::unordered_map<actionFinishedData_c::actionExecutionResultFields_ec, QString> copyTmp(actionExecutionResultFieldToStringTrigger_pro);
//    if (actonDataHub_ptr_ext not_eq nullptr and actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f() not_eq nullptr)
//    {
//        for (auto& pair_ite : actionExecutionResultFieldToStringTrigger_pro)
//        {
//            QString strTmp(pair_ite.second);
//            actonDataHub_ptr_ext->executionOptions_f().stringParserMap_f()->executeForString_f(std::addressof(strTmp));
//            copyTmp.at(pair_ite.first) = strTmp;
//        }
//    }
//    return actionExecutionResultFieldToStringTrigger_pro;
//}

void actionFinishedData_c::setActionExecutionResultFieldToStringTrigger_f(const std::unordered_map<actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringTrigger_par_con)
{
    actionExecutionResultFieldToStringTrigger_pro = actionExecutionResultFieldToStringTrigger_par_con;
}

uint_fast64_t actionFinishedData_c::finishedCount_f() const
{
    return finishedCount_pro;
}

void actionFinishedData_c::setFinishedCount_f(const uint_fast64_t finishedCount_par_con)
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

bool actionFinishedData_c::isFieldsDataValid_f(
        textCompilation_c* errorsPtr_par
        , const void* const objectToIgnore_par) const
{
    bool resultTmp(false);
    while (true)
    {
        if (actionStringId_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Action stringId is empty")
            break;
        }

        bool emptyMappingFoundTmp(false);
        QSet<QString> uniqueStringsTmp;
        for (const std::pair<const actionFinishedData_c::actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringPair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
        {
            if (actionExecutionResultFieldToStringPair_ite_con.second.isEmpty())
            {
                emptyMappingFoundTmp = true;
                text_c errorTextTmp("Action execution result field to empty string trigger mapping found: {0}"
                                    , actionExecutionResultFieldsToStrUMap_sta_con.at(actionExecutionResultFieldToStringPair_ite_con.first));
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                continue;
            }
            if (uniqueStringsTmp.contains(actionExecutionResultFieldToStringPair_ite_con.second))
            {
                text_c errorTextTmp("Action execution result field to string trigger mapping not unique: {0}"
                                    , actionExecutionResultFieldToStringPair_ite_con.second);
                APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
            }
            else
            {
                uniqueStringsTmp.insert(actionExecutionResultFieldToStringPair_ite_con.second);
            }

            auto firstConflictTmp(actonDataHub_ptr_ext->stringTriggerCreationConflict_f(actionExecutionResultFieldToStringPair_ite_con.second, objectToIgnore_par));
            if (firstConflictTmp.first not_eq -1)
            {
                if (firstConflictTmp.first > 0)
                {
                    text_c errorTextTmp("Action execution result field to string trigger already in use \"{0}\" in actionId \"{1}\" / checkId \"{2}\""
                                        , actionExecutionResultFieldToStringPair_ite_con.second, firstConflictTmp.first, firstConflictTmp.second);
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                }
                else
                {
                    text_c errorTextTmp("Action execution result field to string trigger already in use \"{0}\" in stringParserMap"
                                        , actionExecutionResultFieldToStringPair_ite_con.second);
                    APPENDTEXTPTR(errorsPtr_par, errorTextTmp)
                }
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
        , const uint_fast64_t finishedCount_par_con
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
        bool strTonumberConversionResultTmp(false);
        auto resultTmp(json_par_con["finishedCount"].toString().toULongLong(std::addressof(strTonumberConversionResultTmp)));
        if (strTonumberConversionResultTmp)
        {
            finishedCount_pro = resultTmp;
        }
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

uint_fast64_t actionFinishedCheck_c::derivedStringTriggerCreationConflictCount_f(const QString& stringTrigger_par_con) const
{
    uint_fast64_t resultTmp(0);

    for (const std::pair<const actionFinishedData_c::actionExecutionResultFields_ec, QString>& actionExecutionResultFieldToStringPair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
    {
        if (actionExecutionResultFieldToStringPair_ite_con.second.isEmpty()
            and actionExecutionResultFieldToStringPair_ite_con.second == stringTrigger_par_con)
        {
            //mapped stringTriggers are unique, once found it can skip the rest
            resultTmp = 1;// resultTmp + vectorQStringCountSubString_f(actionExecutionResultFieldToStringPair_ite_con.second, {stringTrigger_par_con});
            break;
        }
    }

    return resultTmp;
}

uint_fast64_t actionFinishedCheck_c::derivedUpdateActionStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    uint_fast64_t resultTmp(0);
    if (actionStringId_pro == oldStringId_par_con)
    {
        actionStringId_pro = newStringId_par_con;
        resultTmp = 1;
    }
    return resultTmp;
}

uint_fast64_t actionFinishedCheck_c::derivedActionStringIdDependencyCount_f(const QString& stringId_par_con) const
{
    uint_fast64_t resultTmp(0);
    if (actionStringId_pro == stringId_par_con)
    {
        resultTmp = 1;
    }
    return resultTmp;
}

//uint64_t actionFinishedCheck_c::derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
//{
//    uint_fast64_t resultTmp(0);

//    for (auto& actionExecutionResultFieldToStringPair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
//    {
//        if (actionExecutionResultFieldToStringPair_ite_con.second == oldStringTrigger_par_con)
//        {
//            actionExecutionResultFieldToStringPair_ite_con.second = newStringTrigger_par_con;
//            resultTmp = resultTmp + 1;
//        }
//    }

//    //resultTmp = resultTmp + replaceSubString_f(actionStringId_pro, oldStringTrigger_par_con, newStringTrigger_par_con);

//    return resultTmp;
//}

//uint64_t actionFinishedCheck_c::derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const
//{
//    uint_fast64_t resultTmp(0);
//    for (auto& actionExecutionResultFieldToStringPair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
//    {
//        if (actionExecutionResultFieldToStringPair_ite_con.second == stringTrigger_par_con)
//        {
//            resultTmp = resultTmp + 1;
//        }
//    }

//    //resultTmp = resultTmp + vectorQStringCountSubString_f(actionStringId_pro, {stringTrigger_par_con});

//    return resultTmp;
//}

//QSet<QString> actionFinishedCheck_c::derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const
//{
//    QSet<QString> resultTmp;
//    for (const QString& searchValue_ite_con : searchValues_par_con)
//    {
//        for (const auto& actionExecutionResultFieldToStringPair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
//        {
//            if (actionExecutionResultFieldToStringPair_ite_con.second == searchValue_ite_con)
//            {
//                resultTmp.insert(actionExecutionResultFieldToStringPair_ite_con.second);
//            }
//        }
////        if (vectorQStringCountSubString_f(actionStringId_pro, {searchValue_ite_con}, true) > 0)
////        {
////            resultTmp.insert(actionStringId_pro);
////        }
//    }

//    return resultTmp;
//}

QSet<QString> actionFinishedCheck_c::derivedStringTriggerCreationCollection_f() const
{
    QSet<QString> keyStringsTmp;
    for (const auto& pair_ite_con : actionExecutionResultFieldToStringTrigger_pro)
    {
        if (pair_ite_con.second.isEmpty())
        {
        }
        else
        {
            keyStringsTmp.insert(pair_ite_con.second);
        }
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
        //when an actionFinished type is updated
        //if a stringtrigger key changes search the actions/checks
        //if they use this key and update it
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
        , const actionFinishedData_c& actionFinishedData_par_con)
    : check_c(checkData_par_con)
    , actionFinishedData_c(actionFinishedData_par_con)
{
}
