#include "checkExecutionStateStrMapping.hpp"

#include <QMap>

const QMap<QString, checkExecutionState_ec>& stringToCheckExecutionStateMapRef_f()
{
    static const QMap<QString, checkExecutionState_ec> stringToCheckExecutionStateMap_sta_con(
    {
    //Keys are lower case because this way when reading from json the strings can be lower-cased
    //and compared, allowing to ignore case
    {	"initial", checkExecutionState_ec::initial}
    , {	"preparing", checkExecutionState_ec::preparing}
    , {	"executing", checkExecutionState_ec::executing}
    , {	"error", checkExecutionState_ec::error}
    , {	"stopping", checkExecutionState_ec::stopping}
    , {	"stopped", checkExecutionState_ec::stopped}
    , {	"timeout", checkExecutionState_ec::timeOut}
    , {	"success", checkExecutionState_ec::success}
    });
    return stringToCheckExecutionStateMap_sta_con;
}

const std::unordered_map<checkExecutionState_ec, QString>& checkExecutionStateToStringUMap_ref()
{
    static const std::unordered_map<checkExecutionState_ec, QString> checkExecutionStateToStringUMap_sta_con(
    {
        {	checkExecutionState_ec::empty, "empty" }
        , {	checkExecutionState_ec::initial, "Initial" }
        , {	checkExecutionState_ec::preparing, "Preparing" }
        , {	checkExecutionState_ec::executing, "Executing" }
        , {	checkExecutionState_ec::error, "Error" }
        , {	checkExecutionState_ec::stopping, "Stopping by the user" }
        , {	checkExecutionState_ec::stopped, "Stopped by the user" }
        , {	checkExecutionState_ec::timeOut, "Time out" }
        , {	checkExecutionState_ec::success, "Success" }
    });
    return checkExecutionStateToStringUMap_sta_con;
}

QString checkExecutionStateToString_f(const checkExecutionState_ec checkExecutionState_par_con)
{
    return checkExecutionStateToStringUMap_ref().at(checkExecutionState_par_con);
}

checkExecutionState_ec stringToCheckExecutionState_f(const QString& string_par_con)
{
    return stringToCheckExecutionStateMapRef_f().value(string_par_con, checkExecutionState_ec::empty);
}

QList<QString> checkExecutionStateStringValues_f()
{
    return stringToCheckExecutionStateMapRef_f().keys();
}
