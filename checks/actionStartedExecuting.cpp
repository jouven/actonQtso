#include "actionStartedExecuting.hpp"

#include "../checkExecution/actionStartedExecutingExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"
#include "../reused/stringAlgo.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"
#include "essentialQtso/macros.hpp"
#include "textQtso/text.hpp"

#include <QJsonObject>

QString actionStartedExecutingData_c::actionStringId_f() const
{
    return actionStringId_pro;
}

//QString actionStartedExecutingData_c::actionStringIdParsed_f() const
//{
//    COPYPARSERETURNVAR(actionStringId_pro);
//}

void actionStartedExecutingData_c::setActionStringId_f(const QString& actionStringId_par_con)
{
    actionStringId_pro = actionStringId_par_con;
}

bool actionStartedExecutingData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        if (actionStringId_pro.isEmpty())
        {
            APPENDTEXTPTR(errorsPtr_par, "Action stringId is empty")
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

int_fast64_t actionStartedExecutingData_c::waitFinishMs_f() const
{
    return waitFinishMs_pro;
}

void actionStartedExecutingData_c::setWaitFinishMs_f(const int_fast64_t& waitFinishMs_par_con)
{
    waitFinishMs_pro = waitFinishMs_par_con;
}

actionStartedExecutingData_c::actionStartedExecutingData_c(
        const QString& actionStringId_par_con
        , const uint_fast64_t delayMs_par_con)
    : actionStringId_pro(actionStringId_par_con)
    , waitFinishMs_pro(delayMs_par_con)
{}

void actionStartedExecutingCheck_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["actionStringId"] = actionStringId_pro;
    json_par["waitFinishMs"] = QString::number(waitFinishMs_pro);
}

void actionStartedExecutingCheck_c::derivedRead_f(const QJsonObject& json_par_con)
{
    actionStringId_pro = json_par_con["actionStringId"].toString();
    if (not json_par_con["waitFinishMs"].isString())
    {
        bool strTonumberConversionResultTmp(false);
        auto resultTmp(json_par_con["waitFinishMs"].toString().toLongLong(std::addressof(strTonumberConversionResultTmp)));
        if (strTonumberConversionResultTmp)
        {
            waitFinishMs_pro = resultTmp;
        }
    }
}

bool actionStartedExecutingCheck_c::derivedIsValidCheck_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

check_c* actionStartedExecutingCheck_c::derivedClone_f() const
{
    //slice and dice
    actionStartedExecutingData_c actionStartedExecutingDataTmp(*this);
    checkData_c checkDataTmp(*this);
    return new actionStartedExecutingCheck_c(checkDataTmp, actionStartedExecutingDataTmp);
}

baseCheckExecution_c* actionStartedExecutingCheck_c::createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par)
{
    return new actionStartedExecutingCheckExecution_c(checkDataExecutionResult_ptr_par, this);
}

checkType_ec actionStartedExecutingCheck_c::type_f() const
{
    return checkType_ec::actionStartedExecuting;
}

uint64_t actionStartedExecutingCheck_c::derivedUpdateActionStringIdDependencies_f(const QString& newStringId_par_con, const QString& oldStringId_par_con)
{
    uint_fast64_t resultTmp(0);
    if (actionStringId_pro == oldStringId_par_con)
    {
        actionStringId_pro = newStringId_par_con;
        resultTmp = 1;
    }
    return resultTmp;
}

uint64_t actionStartedExecutingCheck_c::derivedActionStringIdDependencyCount_f(const QString& stringId_par_con) const
{
    uint_fast64_t resultTmp(0);
    if (actionStringId_pro == stringId_par_con)
    {
        resultTmp = 1;
    }
    return resultTmp;
}

//uint64_t actionStartedExecutingCheck_c::derivedUpdateStringTriggerDependecies_f(const QString& newStringTrigger_par_con, const QString& oldStringTrigger_par_con)
//{
//    uint_fast64_t resultTmp(0);
//    resultTmp = resultTmp + replaceSubString_f(actionStringId_pro, oldStringTrigger_par_con, newStringTrigger_par_con);
//    return resultTmp;
//}

//uint64_t actionStartedExecutingCheck_c::derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const
//{
//    uint_fast64_t resultTmp(0);
//    resultTmp = resultTmp + vectorQStringCountSubString_f(actionStringId_pro, {stringTrigger_par_con});
//    return resultTmp;
//}

//QSet<QString> actionStartedExecutingCheck_c::derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const
//{
//    QSet<QString> resultTmp;
//    for (const QString& searchValue_ite_con : searchValues_par_con)
//    {
//        if (vectorQStringCountSubString_f(searchValue_ite_con, {actionStringId_pro}, true) > 0)
//        {
//            resultTmp.insert(searchValue_ite_con);
//        }
//    }
//    return resultTmp;
//}

actionStartedExecutingCheck_c::actionStartedExecutingCheck_c(const checkData_c& checkData_par_con
        , const actionStartedExecutingData_c& actionStartedExecuting_par_con)
    : check_c(checkData_par_con)
    , actionStartedExecutingData_c(actionStartedExecuting_par_con)
{}

void actionStartedExecutingCheck_c::updateAcionStartedExecutingData_f(const actionStartedExecutingData_c& actionStartedExecutingData_par_con)
{
    this->actionStartedExecutingData_c::operator=(actionStartedExecutingData_par_con);
}

