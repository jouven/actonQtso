#include "actionMapping.hpp"

#include "../actionData.hpp"

#include "../actions/runProcess.hpp"
#include "../actions/createDirectory.hpp"
#include "../actions/copyFile.hpp"
#include "../actions/deleteFileDir.hpp"
#include "../actions/metaEndExecutionCycle.hpp"
#include "../actions/folderChangeReaction.hpp"

action_c* createNewActionOfType_f(const actionType_ec type_par_con)
{
    action_c* resultTmp(nullptr);
    while (true)
    {
        if (type_par_con == actionType_ec::runProcess)
        {
            resultTmp = new runProcessAction_c();
            break;
        }
        if (type_par_con == actionType_ec::runProcess)
        {
            resultTmp = new runProcessAction_c();
            break;
        }
        if (type_par_con == actionType_ec::createDirectory)
        {
            resultTmp = new createDirectoryAction_c();
            break;
        }
        if (type_par_con == actionType_ec::copyFile)
        {
            resultTmp = new copyFileAction_c();
            break;
        }
        if (type_par_con == actionType_ec::deleteFileDir)
        {
            resultTmp = new deleteFileDirAction_c();
            break;
        }
        if (type_par_con == actionType_ec::metaEndExecutionCycle)
        {
            resultTmp = new metaEndExecutionCycleAction_c();
            break;
        }
        if (type_par_con == actionType_ec::folderChangeReaction)
        {
            resultTmp = new folderChangeReactionAction_c();
            break;
        }
        break;
    }
    return resultTmp;
}
