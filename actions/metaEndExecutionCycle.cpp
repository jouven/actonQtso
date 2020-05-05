#include "metaEndExecutionCycle.hpp"

#include "../actionExecution/metaEndExecutionCycleExecution.hpp"
#include "../actionMappings/actionStrMapping.hpp"
#include "../actonDataHub.hpp"

#include "stringParserMapQtso/stringParserMap.hpp"

#include <QJsonObject>

metaEndExecutionCycleData_c::endType_ec metaEndExecutionCycleData_c::endType_f() const
{
    return endType_pro;
}

void metaEndExecutionCycleData_c::setEndType_f(const endType_ec& type_par_con)
{
    endType_pro = type_par_con;
}

//int_fast64_t metaEndExecutionCycleData_c::killTimeout_f() const
//{
//    return killTimeout_pro;
//}

//void metaEndExecutionCycleData_c::setKillTimeout_f(const int_fast64_t& killTimeout_par_con)
//{
//    killTimeout_pro = killTimeout_par_con;
//}

bool metaEndExecutionCycleData_c::killAfterTimeout_f() const
{
    return killAfterTimeout_pro;
}

void metaEndExecutionCycleData_c::setKillAfterTimeout_f(const bool killAfterTimeout_par_con)
{
    killAfterTimeout_pro = killAfterTimeout_par_con;
}

metaEndExecutionCycleData_c::metaEndExecutionCycleData_c(
        const endType_ec type_par_con
        , const bool killAfterTimeout_par_con)
    : endType_pro(type_par_con)
    , killAfterTimeout_pro(killAfterTimeout_par_con)
{}

const QMap<QString, metaEndExecutionCycleData_c::endType_ec> metaEndExecutionCycleData_c::strToEndExecutionTypeMap_sta_con(
{
//Keys are lower case because this way when reading from json the strings can be lower-cased
//and compared, allowing to ignore case
    {	"stop", metaEndExecutionCycleData_c::endType_ec::stop}
    , {	"waittofinish", metaEndExecutionCycleData_c::endType_ec::waitToFinish}
});

const std::unordered_map<metaEndExecutionCycleData_c::endType_ec, QString> metaEndExecutionCycleData_c::endExecutionTypeToStrUMap_sta_con(
{
    {	metaEndExecutionCycleData_c::endType_ec::stop, "stop" }
    , {	metaEndExecutionCycleData_c::endType_ec::waitToFinish, "waitToFinish" }
});

void metaEndExecutionCycleAction_c::derivedWrite_f(QJsonObject& json_par) const
{
    json_par["endType"] = endExecutionTypeToStrUMap_sta_con.at(endType_pro);
    json_par["killAfterTimeout"] = killAfterTimeout_pro;
//    json_par["killTimeout"] = QString::number(killTimeout_pro);
}

void metaEndExecutionCycleAction_c::derivedRead_f(const QJsonObject& json_par_con)
{
    if (json_par_con["endType"].isString())
    {
        endType_pro = strToEndExecutionTypeMap_sta_con.value(json_par_con["endType"].toString());
    }
    if (json_par_con["killAfterTimeout"].isBool())
    {
        killAfterTimeout_pro = json_par_con["killAfterTimeout"].toBool();
    }
//    if (json_par_con["killTimeout"].isString())
//    {
//        bool goodNumberConversionTmp(false);
//        int_fast64_t tmpInt(json_par_con["killTimeout"].toString().toLongLong(std::addressof(goodNumberConversionTmp)));
//        if (goodNumberConversionTmp and tmpInt >= 0)
//        {
//            killTimeout_pro = tmpInt;
//        }
    //    }
}

bool metaEndExecutionCycleAction_c::derivedIsValidAction_f(textCompilation_c* ) const
{
    return true; //isFieldsDataValid_f(errors_par);
}

action_c* metaEndExecutionCycleAction_c::derivedClone_f() const
{
    //slice and dice
    metaEndExecutionCycleData_c createDirectoryDataTmp(*this);
    actionData_c actionDataTmp(*this);
    return new metaEndExecutionCycleAction_c(actionDataTmp, createDirectoryDataTmp);
}

baseActionExecution_c* metaEndExecutionCycleAction_c::createExecutionObj_f(actionDataExecutionResult_c* actionDataExecutionResult_ptr_par)
{
    return new metaEndExecutionCycleActionExecution_c(actionDataExecutionResult_ptr_par, this);
}

actionType_ec metaEndExecutionCycleAction_c::type_f() const
{
    return actionType_ec::metaEndExecutionCycle;
}

//QString metaEndExecutionCycleAction_c::typeStr_f() const
//{
//    return actionTypeToStrUMap_ext_con.at(type_f());
//}

metaEndExecutionCycleAction_c::metaEndExecutionCycleAction_c(
        const actionData_c& actionData_par_con
        , const metaEndExecutionCycleData_c& metaEndExecutionData_par_con)
    : action_c(actionData_par_con)
    , metaEndExecutionCycleData_c(metaEndExecutionData_par_con)
{
}

void metaEndExecutionCycleAction_c::updateMetaEndExecutionCycleData_f(const metaEndExecutionCycleData_c& metaEndExecutionCycleData_par_con)
{
    this->metaEndExecutionCycleData_c::operator=(metaEndExecutionCycleData_par_con);
}

