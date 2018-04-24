#include "createDirectoryExecution.hpp"

#include <QDir>

createDirectoryActionExecution_c::createDirectoryActionExecution_c(
        const createDirectoryAction_c& createDirectoryAction_par_con)
    : createDirectoryAction_c(createDirectoryAction_par_con)
{
}

bool createDirectoryActionExecution_c::execute_f()
{
    bool resultTmp(false);
    if (not startedOnce_pri)
    {
        startedOnce_pri = true;
        Q_EMIT executionStateChange_signal(actionExecutionState_ec::running);

        QDir pathToCreateTmp(directoryPath_f());
        if (pathToCreateTmp.exists())
        {
            Q_EMIT addOutput_signal("Already exists");
            resultTmp = true;
        }
        else
        {
            if (createParents_f())
            {
                resultTmp = pathToCreateTmp.mkpath(directoryPath_f());
            }
            else
            {
                resultTmp = pathToCreateTmp.mkdir(directoryPath_f());
            }
        }
        if (resultTmp)
        {
            Q_EMIT executionStateChange_signal(actionExecutionState_ec::success);
        }
        else
        {
            Q_EMIT executionStateChange_signal(actionExecutionState_ec::error);
            Q_EMIT addError_signal("Couldn't create the directory, no permissions? trying to create a sub-directory without existing parent/s?");
        }
        Q_EMIT anyFinish_signal();
    }
    return resultTmp;
}


