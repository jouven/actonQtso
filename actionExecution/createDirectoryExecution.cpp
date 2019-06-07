#include "createDirectoryExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/createDirectory.hpp"

#include <QDir>

createDirectoryActionExecution_c::createDirectoryActionExecution_c(
        actionDataExecutionResult_c* actionExecutionResultObj_par_con
        , createDirectoryAction_c* createDirectoryActionPtr_par
)
    : baseActionExecution_c(actionExecutionResultObj_par_con)
    , createDirectoryActionPtr_pri(createDirectoryActionPtr_par)
{}

void createDirectoryActionExecution_c::derivedExecute_f()
{
    QDir pathToCreateTmp(createDirectoryActionPtr_pri->directoryPathParsed_f());
    if (pathToCreateTmp.exists())
    {
        if (createDirectoryActionPtr_pri->errorIfExists_f())
        {
            //addError already change the state
            //Q_EMIT executionStateChange_signal(actionExecutionState_ec::error);
            Q_EMIT addError_signal(pathToCreateTmp.path() + " already exists");
        }
        else
        {
            Q_EMIT addOutput_signal(pathToCreateTmp.path() + " already exists");
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
            if (not parentExistsTmp and not createDirectoryActionPtr_pri->createParents_f())
            {
                Q_EMIT addError_signal("Can't create directory " + pathToCreateTmp.path() + ", parent directory doesn't exists");
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
                Q_EMIT addError_signal("Couldn't create the directory" + pathToCreateTmp.path() + ", no permissions?");
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


