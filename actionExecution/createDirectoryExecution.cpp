#include "createDirectoryExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/createDirectory.hpp"

#include "textQtso/text.hpp"

#include <QDir>

createDirectoryActionExecution_c::createDirectoryActionExecution_c(
        actionExecutionResult_c* actionExecutionResultObj_par_con
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
            emitExecutionMessage_f({"{0} already exists", pathToCreateTmp.path()}, executionMessage_c::type_ec::error);
        }
        else
        {
            emitExecutionMessage_f({"{0} already exists", pathToCreateTmp.path()}, executionMessage_c::type_ec::warning);
            //Q_EMIT executionStateChange_signal(actionExecutionState_ec::success);
        }
    }
    else
    {
        while (true)
        {
            bool resultTmp(false);
            QString directoryNameTmp(pathToCreateTmp.dirName());
            bool parentExistsTmp(pathToCreateTmp.cdUp());
            if (not parentExistsTmp and not createDirectoryActionPtr_pri->createParents_f())
            {
                emitExecutionMessage_f({"Can't create directory {0}, parent directory doesn't exists", pathToCreateTmp.path()}, executionMessage_c::type_ec::error);
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
                emitExecutionMessage_f({"Couldn't create the directory {0}", pathToCreateTmp.path()}, executionMessage_c::type_ec::error);
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


