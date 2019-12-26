#include "actionMapping.hpp"

#include "../actionData.hpp"

#include "../actions/runProcess.hpp"
#include "../actions/createDirectory.hpp"
#include "../actions/copyFile.hpp"
#include "../actions/deleteFileDir.hpp"
#include "../actions/metaEndExecutionCycle.hpp"

const std::unordered_map<actionType_ec, std::function<action_c*()>> actionTypeToActionCreationFunctionMap_ext_con(
{
    {	actionType_ec::runProcess, []() -> action_c* { return new runProcessAction_c;}}
    , {	actionType_ec::createDirectory, []() -> action_c* { return new createDirectoryAction_c;}}
    , {	actionType_ec::copyFile, []() -> action_c* { return new copyFileAction_c;}}
    , {	actionType_ec::deleteFileDir, []() -> action_c* { return new deleteFileDirAction_c;}}
    , {	actionType_ec::metaEndExecutionCycle, []() -> action_c* { return new metaEndExecutionCycleAction_c;}}

});
