#include "checkExecutionStateStrMapping.hpp"

//const QHash<QString, checkExecutionState_ec> strToCheckExecutionStateUMap_glo_sta_con(
//{
//    {	"Initial", checkExecutionState_ec::initial}
//    , {	"Executing", checkExecutionState_ec::executing}
//    , {	"Error", checkExecutionState_ec::error}
//    , {	"Stopped by the user", checkExecutionState_ec::stoppedByUser}
//    , {	"Finished", checkExecutionState_ec::finished}
//    , {	"Time out", checkExecutionState_ec::timeOut}
//});

const std::unordered_map<checkExecutionState_ec, QString> checkExecutionStateToStrUMap_glo_sta_con(
{
    {	checkExecutionState_ec::initial, "Initial" }
    , {	checkExecutionState_ec::preparing, "Preparing" }
    , {	checkExecutionState_ec::executing, "Executing" }
    , {	checkExecutionState_ec::error, "Error" }
    , {	checkExecutionState_ec::stoppingByUser, "Stopping by the user" }
    , {	checkExecutionState_ec::stoppedByUser, "Stopped by the user" }
    , {	checkExecutionState_ec::timeOut, "Time out" }
    , {	checkExecutionState_ec::finishedTrue, "Finished true" }
    , {	checkExecutionState_ec::finishedFalse, "Finished false" }
});


