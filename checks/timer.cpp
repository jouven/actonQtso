#include "timer.hpp"

#include "../checkExecution/timerExecution.hpp"
#include "../checkMappings/checkStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

#include <QJsonObject>

const QMap<QString, timerData_c::type_ec> timerData_c::strTotimerTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json the strings can be lower-cased
//and compared, allowing to ignore case
    {	"timer", timerData_c::type_ec::timer}
    , {	"datetime", timerData_c::type_ec::dateTime}
});

const std::unordered_map<timerData_c::type_ec, QString> timerData_c::timerTypeToStrUMap_sta_con(
{
    {	timerData_c::type_ec::timer, "timer" }
    , {	timerData_c::type_ec::dateTime, "dateTime" }
});

timerData_c::type_ec timerData_c::alarmType_f() const
{
    return type_pro;
}

void timerData_c::setAlarmType_f(const type_ec& alarmType_par_con)
{
    type_pro = alarmType_par_con;
}

int_fast64_t timerData_c::value_f() const
{
    return value_pro;
}

void timerData_c::setValue_f(const int_fast64_t& value_par_con)
{
    value_pro = value_par_con;
}

bool timerData_c::isFieldsDataValid_f(textCompilation_c* errorsPtr_par) const
{
    bool resultTmp(false);
    while (true)
    {
        if (value_pro < 0)
        {
            text_c errorTextTmp("Wrong value: {0}, value must be between 0 and INT64MAX", value_pro);
            APPENDTEXTPTR(errorsPtr_par, errorTextTmp);
            break;
        }

        resultTmp = true;
        break;
    }
    return resultTmp;
}

bool timerData_c::errorOnPastDateTimeValue_f() const
{
    return errorOnPastDateTimeValue_pro;
}

void timerData_c::setErrorOnPastDateTimeValue_f(const bool errorOnPastDateTimeValue_par_con)
{
    errorOnPastDateTimeValue_pro = errorOnPastDateTimeValue_par_con;
}

timerData_c::timerData_c(
        const type_ec type_par_con
        , const int_fast64_t value_par_con
        , const bool errorOnPastDatetimeValue_par_con)
    : type_pro(type_par_con)
    , value_pro(value_par_con)
    , errorOnPastDateTimeValue_pro(errorOnPastDatetimeValue_par_con)
{}

void timerCheck_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["type"] = timerTypeToStrUMap_sta_con.at(type_pro);
    json_par["value"] = QString::number(value_pro);
    json_par["errorOnPastDatetimeValue"] = errorOnPastDateTimeValue_pro;
}

void timerCheck_c::derivedRead_f(const QJsonObject& json_par_con)
{
    if (json_par_con["type"].isString())
    {
        type_pro = strTotimerTypeMap_sta_con.value(json_par_con["type"].toString().toLower());
    }
    if (json_par_con["value"].isString())
    {
        bool strTonumberConversionResultTmp(false);
        auto resultTmp(json_par_con["value"].toString().toLongLong(std::addressof(strTonumberConversionResultTmp)));
        if (strTonumberConversionResultTmp)
        {
            value_pro = resultTmp;
        }
    }
    if (json_par_con["errorOnPastDatetimeValue"].isBool())
    {
        errorOnPastDateTimeValue_pro = json_par_con["errorOnPastDatetimeValue"].toBool();
    }
}

bool timerCheck_c::derivedIsValidCheck_f(textCompilation_c* errors_par) const
{
    return isFieldsDataValid_f(errors_par);
}

check_c* timerCheck_c::derivedClone_f() const
{
    //slice and dice
    timerData_c actionFinishedTmp(*this);
    checkData_c checkDataTmp(*this);
    timerCheck_c* resultTmp(new timerCheck_c(checkDataTmp, actionFinishedTmp));
    return resultTmp;
}

baseCheckExecution_c* timerCheck_c::createExecutionObj_f(checkDataExecutionResult_c* checkDataExecutionResult_ptr_par)
{
    return new timerCheckExecution_c(checkDataExecutionResult_ptr_par, this);
}

checkType_ec timerCheck_c::type_f() const
{
    return checkType_ec::timer;
}

timerCheck_c::timerCheck_c(
        const checkData_c& checkData_par_con
        , const timerData_c& timerData_par_con)
    : check_c(checkData_par_con)
    , timerData_c(timerData_par_con)
{
}

void timerCheck_c::updateTimerData_f(const timerData_c& timerData_par_con)
{
    this->timerData_c::operator=(timerData_par_con);
}
