#include "createDirectoryExecution.hpp"

#include "../actionDataExecutionResult.hpp"

#include <QDir>

createDirectoryActionExecution_c::createDirectoryActionExecution_c(
        actionDataExecutionResult_c* actionExecutionResultObj_par_con
        , const createDirectoryAction_c& createDirectoryAction_par_con
)
    : baseActionExecution_c(actionExecutionResultObj_par_con)
    , createDirectoryAction_c(createDirectoryAction_par_con)
{}

void createDirectoryActionExecution_c::derivedExecute_f()
{
    QDir pathToCreateTmp(directoryPath_f());
    if (pathToCreateTmp.exists())
    {
        if (errorIfExists_f())
        {
            //addError already change the state
            //Q_EMIT executionStateChange_signal(actionExecutionState_ec::error);
            Q_EMIT addError_signal("Already exists");
        }
        else
        {
            Q_EMIT addOutput_signal("Already exists");
            //Q_EMIT executionStateChange_signal(actionExecutionState_ec::success);
        }
    }
    else
    {
        Q_EMIT executionStateChange_signal(actionExecutionState_ec::executing);
        while (true)
        {
            bool resultTmp(false);
            QString directoryNameTmp(pathToCreateTmp.dirName());
            bool parentExistsTmp(pathToCreateTmp.cdUp());
            if (not parentExistsTmp and not createParents_f())
            {
                Q_EMIT addError_signal("Can't create directory because parent directory doesn't exists");
                break;
            }
            else
            {
                //parent exist else createParents_f() == true
                if (parentExistsTmp)
                {
                    resultTmp = pathToCreateTmp.mkdir(directoryNameTmp);
                }
                else
                {
                    resultTmp = pathToCreateTmp.mkpath(".");
                }
            }

            if (resultTmp)
            {
                //nothing to set since if no error/stop/kill happened success will be the end state
            }
            else
            {
                //Q_EMIT executionStateChange_signal(actionExecutionState_ec::error);
                Q_EMIT addError_signal("Couldn't create the directory, no permissions?");
            }
            break;
        }
    }
    Q_EMIT anyFinish_signal();
}

void createDirectoryActionExecution_c::derivedStop_f()
{
    //no need
}

void createDirectoryActionExecution_c::derivedKill_f()
{
    //no need
}


