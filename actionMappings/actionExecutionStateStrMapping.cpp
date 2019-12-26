#include "actionExecutionStateStrMapping.hpp"

//const QHash<QString, actionExecutionState_ec> strToActionExecutionStateUMap_ext_con(
//{
//    {	"Initial", actionExecutionState_ec::initial}
//    , {	"Executing", actionExecutionState_ec::executing}
//    , {	"Error", actionExecutionState_ec::error}
//    , {	"Stopped by the user", actionExecutionState_ec::stoppedByUser}
//    , {	"Success", actionExecutionState_ec::success}
//    , {	"Time out", actionExecutionState_ec::timeOut}
//});

const std::unordered_map<actionExecutionState_ec, QString> actionExecutionStateToStrUMap_ext_con(
{
    {	actionExecutionState_ec::initial, "Initial" }
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
