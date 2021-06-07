#include "actionExecutionStateStrMapping.hpp"

#include <QMap>

const QMap<QString, actionExecutionState_ec>& stringToActionExecutionStateMapRef_f()
{
    static const QMap<QString, actionExecutionState_ec> stringToActionExecutionStateMap_sta_con(
    {
    //Keys are lower case because this way when reading from json the strings can be lower-cased
    //and compared, allowing to ignore case
    {	"initial", actionExecutionState_ec::initial}
    , {	"preparing", actionExecutionState_ec::preparing}
    , {	"executingchecks", actionExecutionState_ec::executingChecks}
    , {	"executing", actionExecutionState_ec::executing}
    , {	"error", actionExecutionState_ec::error}
    , {	"stopping", actionExecutionState_ec::stopping}
    , {	"stopped", actionExecutionState_ec::stopped}
    , {	"killing", actionExecutionState_ec::killing}
    , {	"killed", actionExecutionState_ec::killed}
    , {	"timeout", actionExecutionState_ec::timeOut}
    , {	"success", actionExecutionState_ec::success}
    , {	"checksfailed", actionExecutionState_ec::checksFailed}
    });
    return stringToActionExecutionStateMap_sta_con;
}

const std::unordered_map<actionExecutionState_ec, QString>& actionExecutionStateToStringUMap_ref()
{
    static const std::unordered_map<actionExecutionState_ec, QString> actionExecutionStateToStringUMap_sta_con(
    {
        {	actionExecutionState_ec::empty, "empty" }
        , {	actionExecutionState_ec::initial, "Initial" }
        , {	actionExecutionState_ec::preparing, "Preparing" }
        , {	actionExecutionState_ec::executingChecks, "Executing checks" }
        , {	actionExecutionState_ec::executing, "Executing" }
        , {	actionExecutionState_ec::error, "Error" }
        , {	actionExecutionState_ec::stopping, "Stopping" }
        , {	actionExecutionState_ec::stopped, "Stopped" }
        , {	actionExecutionState_ec::killing, "Killing" }
        , {	actionExecutionState_ec::killed, "Killed" }
        , {	actionExecutionState_ec::timeOut, "Time out" }
        , {	actionExecutionState_ec::success, "Success" }
        , {	actionExecutionState_ec::checksFailed, "Checks failed" }
    });
    return actionExecutionStateToStringUMap_sta_con;
}

QString actionExecutionStateToString_f(const actionExecutionState_ec actionExecutionState_par_con)
{
    return actionExecutionStateToStringUMap_ref().at(actionExecutionState_par_con);
}

actionExecutionState_ec stringToActionExecutionState_f(const QString& string_par_con)
{
    return stringToActionExecutionStateMapRef_f().value(string_par_con, actionExecutionState_ec::empty);
}

QList<QString> actionExecutionStateStringValues_f()
{
    return stringToActionExecutionStateMapRef_f().keys();
}
