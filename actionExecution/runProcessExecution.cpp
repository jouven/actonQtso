#include "runProcessExecution.hpp"

//#include <QThread>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

#include <QProcessEnvironment>

runProcessActionExecution_c::runProcessActionExecution_c(
        const runProcessAction_c& processAction_par_con
        , const bool mergeOutErr_par_con
        , const int_fast32_t timeoutMilliseconds_par_con)
    : runProcessAction_c(processAction_par_con)
    //, initialized_pri(true)
    //FUTURE implement the other arguments
{
    connect(&actionProcess_pri, &QProcess::errorOccurred, this, &runProcessActionExecution_c::readError_f);
    connect(&actionProcess_pri, &QProcess::started, this, &runProcessActionExecution_c::setStarted_f);
    connect(&actionProcess_pri, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &runProcessActionExecution_c::setFinished_f);
    connect(&actionProcess_pri, &QProcess::readyReadStandardError, this, &runProcessActionExecution_c::readStderr_f);
    connect(&actionProcess_pri, &QProcess::readyReadStandardOutput, this, &runProcessActionExecution_c::readStdout_f);
    connect(&actionProcess_pri, &QProcess::stateChanged, this, &runProcessActionExecution_c::readProcessState_f);
}

//int runProcessActionExecution_c::returnValue_f() const
//{
//    int exitCodeTmp(-1);
//    if (actionProcess_pri.exitStatus() == QProcess::NormalExit)
//    {
//        exitCodeTmp = actionProcess_pri.exitCode();
//    }
//    return exitCodeTmp;
//}

bool runProcessActionExecution_c::execute_f()
{
    bool resultTmp(false);
    while (not startedOnce_pri)
    {
        startedOnce_pri = true;
        QStringList argumentsTmp;
        for (const argument_c& argument_ite_con : arguments_f())
        {
            if (argument_ite_con.enabled_f())
            {
                argumentsTmp.append(argument_ite_con.argument_f());
            }
        }
        QDir workingDirectoryTmp(workingDirectory_f());
        if (not workingDirectoryTmp.exists())
        {
            Q_EMIT addError_signal("Working directory doesn't exists");
            break;
        }

        //if a "clear" environment must be used OR there is something to append to the environment
        //THEN modify the environment
        const bool envModified(not useActonEnvironment_f() or not environmentToAdd_f().isEmpty());
        if (envModified)
        {
            QProcessEnvironment processEnvironmentTmp(QProcessEnvironment::systemEnvironment());
            if (not useActonEnvironment_f())
            {
                processEnvironmentTmp.clear();
#ifdef DEBUGJOUVEN
                //TODO remove? (the include too)
                //I need to see what output this gives
                qDebug() << "processEnvironmentTmp.clear()" << endl;
                qDebug() << "actionProcess_pri.environment().isEmpty() " << actionProcess_pri.processEnvironment().isEmpty() << endl;
#endif
            }
            if (not environmentToAdd_f().isEmpty())
            {
                QHash<QString, environmentPair_c>::const_iterator iteratorTmp = environmentToAdd_f().constBegin();
                while (iteratorTmp != environmentToAdd_f().constEnd())
                {
                    if (iteratorTmp.value().enabled_f())
                    {
#ifdef DEBUGJOUVEN
                //TODO remove? (the include too)
                //I need to see what output this gives
                qDebug() << "iteratorTmp.key() " << iteratorTmp.key() << "iteratorTmp.value_f() " << iteratorTmp.value().value_f()  << endl;
#endif
                        processEnvironmentTmp.insert(iteratorTmp.key(), iteratorTmp.value().value_f());
                    }
                    ++iteratorTmp;
                }
            }
            actionProcess_pri.setProcessEnvironment(processEnvironmentTmp);
#ifdef DEBUGJOUVEN
                //TODO remove? (the include too)
                //I need to see what output this gives
                qDebug() << "actionProcess_pri.environment().isEmpty() " << actionProcess_pri.processEnvironment().isEmpty() << endl;
                qDebug() << "actionProcess_pri.processEnvironment().value(\"PATH\") " << actionProcess_pri.processEnvironment().value("PATH") << endl;
#endif
        }
        else
        {
            //nothing to do
        }

        actionProcess_pri.setWorkingDirectory(workingDirectory_f());
        //TODO issue an error on the "qt site" that even with a clear environment, no PATH set, it manages to run stuff in /usr/bin
        actionProcess_pri.start(processPath_f(), argumentsTmp);
        resultTmp = true;
    }
    return resultTmp;
}

void runProcessActionExecution_c::terminateExecution_f()
{
    if (actionProcess_pri.state() == QProcess::Running)
    {
        actionProcess_pri.terminate();
    }
}

void runProcessActionExecution_c::killExecution_f()
{
    if (actionProcess_pri.state() == QProcess::Running)
    {
        actionProcess_pri.kill();
    }
}

void runProcessActionExecution_c::readError_f(QProcess::ProcessError error_par)
{
    //TODO requires test, maybe the switch case isn't needed
    //if nothing comes out of errorString, use the documentation descriptions
    //some of the errors aren't critical like timeout or the read/write ones

    //if the error is not given by the process being run
    //i.e. process file no existing
    bool calleeErrorTmp(false);
    QString errorStrTmp;
    switch (error_par)
    {
        case QProcess::FailedToStart:
        {
            errorStrTmp.append(actionProcess_pri.errorString());
            calleeErrorTmp = true;
        }
        break;
        case QProcess::Crashed:
        {
            errorStrTmp.append(actionProcess_pri.errorString());
        }
        break;
            //this only applies for the functions that block the thread (won't use them)
//        case QProcess::Timedout:
//        {
//            errorStrTmp.append(actionProcess_pri.errorString());
//        }
//        break;
            //this would apply if anything was written into the process, right now nothing, maybe in the future
//        case QProcess::WriteError:
//        {
//            errorStrTmp.append(actionProcess_pri.errorString());
//        }
//        break;
            //this one shouldn't happen because "An error occurred when attempting to read from the process. For example, the process may not be running."
            //reading only happens when the readReady signal happens and I don't think that it would signal to read when that would trigger an error
//        case QProcess::ReadError:
//        {
//            errorStrTmp.append(actionProcess_pri.errorString());
//        }
//        break;
        case QProcess::UnknownError:
        {
            errorStrTmp.append(actionProcess_pri.errorString());
            //I don't know if this happens when the one at fault is the process or the callee
        }
        break;
        default:
        {
            errorStrTmp.append("switch case default");
            //theoretically it shouldn't enter here ever, default is QProcess::UnknownError
        }
    }
    Q_EMIT addError_signal(errorStrTmp);
    if (calleeErrorTmp)
    {
        Q_EMIT executionStateChange_signal(actionExecutionState_ec::error);
    }
}

void runProcessActionExecution_c::setStarted_f()
{
    Q_EMIT executionStateChange_signal(actionExecutionState_ec::running);
}

void runProcessActionExecution_c::setFinished_f(int exitCode_par, QProcess::ExitStatus exitStatus_par)
{
    //finished_pri = true;
    if (exitStatus_par == QProcess::ExitStatus::CrashExit)
    {
        //theoretically readError_f takes care of this already
        //Q_EMIT stateChange_f(actionExecutionState_ec::error);
    }
    else
    {
        Q_EMIT setReturnCode_signal(exitCode_par);
        if (exitCode_par == 0)
        {
            Q_EMIT executionStateChange_signal(actionExecutionState_ec::success);
        }
        else
        {
            //this case might need an extra end state, because right now
            //it's assumed that if the return code != 0 some error might happened
            //but there might be a case that the process will end with the exit status NormalExit
            //and the return code != 0
        }
    }
    Q_EMIT anyFinish_signal();
}

void runProcessActionExecution_c::readStderr_f()
{
    QString stderrTmp(actionProcess_pri.readAllStandardError());
    Q_EMIT addProcessError_signal(stderrTmp);
}

void runProcessActionExecution_c::readStdout_f()
{
    QString stdoutTmp(actionProcess_pri.readAllStandardOutput());
    Q_EMIT addProcessOutput_signal(stdoutTmp);
}

void runProcessActionExecution_c::readProcessState_f(QProcess::ProcessState newState_par)
{
#ifdef DEBUGJOUVEN
    //TODO remove? (the include too)
    //I need to see what output this gives
    qDebug() << newState_par << endl;
#endif
}

