#include "actionExecutionStateStrMapping.hpp"

const QHash<QString, actionExecutionState_ec> strToActionExecutionStateUMap_glo_sta_con(
{
    {	"Initial", actionExecutionState_ec::initial}
    , {	"Running", actionExecutionState_ec::running}
    , {	"Error", actionExecutionState_ec::error}
    , {	"Stopped by the user", actionExecutionState_ec::stoppedByUser}
    , {	"Success", actionExecutionState_ec::success}
    , {	"Time out", actionExecutionState_ec::timeOut}
});

const std::unordered_map<actionExecutionState_ec, QString> actionExecutionStateToStrUMap_glo_sta_con(
{
    {	actionExecutionState_ec::initial, "Initial" }
    , {	actionExecutionState_ec::running, "Running" }
    , {	actionExecutionState_ec::error, "Error" }
    , {	actionExecutionState_ec::stoppedByUser, "Stopped by the user" }
    , {	actionExecutionState_ec::timeOut, "Time out" }
    , {	actionExecutionState_ec::success, "Success" }
});
