#include "runProcessExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/runProcess.hpp"
#include "../actonDataHub.hpp"

#include "textQtso/text.hpp"
#include "essentialQtso/macros.hpp"

//#include <QThread>
//#include <QCoreApplication>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#define MACRO_ADDLOG(...) MACRO_ADDACTONDATAHUBLOG(runProcessActionPtr_pri->actonDataHubParent_f(),__VA_ARGS__)

runProcessActionExecution_c::runProcessActionExecution_c(
        actionExecutionResult_c* actionExecutionResultObj_par_con
        , runProcessAction_c* runProcessActionPtr_par
        //FUTURE implement the other arguments
        , const bool mergeOutErr_par_con
        , const int_fast32_t timeoutMilliseconds_par_con)
    : baseActionExecution_c(actionExecutionResultObj_par_con)
    , runProcessActionPtr_pri(runProcessActionPtr_par)
{
    QObject::connect(this, &runProcessActionExecution_c::setReturnCode_signal, actionExecutionResultObj_pri, &actionExecutionResult_c::setReturnCode_f);

    //main issue here that might need some testing, if the process crashes or ends because error
    //will always "errorOcurred" happen before "finished"?

    //process writing the stderr doesn't trigger this
    QObject::connect(&actionProcess_pri, &QProcess::errorOccurred, this, &runProcessActionExecution_c::readError_f);
    //QObject::connect(&actionProcess_pri, &QProcess::started, this, &runProcessActionExecution_c::setStarted_f);
    QObject::connect(&actionProcess_pri, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &runProcessActionExecution_c::setFinished_f);
    QObject::connect(&actionProcess_pri, &QProcess::readyReadStandardError, this, &runProcessActionExecution_c::readStderr_f);
    QObject::connect(&actionProcess_pri, &QProcess::readyReadStandardOutput, this, &runProcessActionExecution_c::readStdout_f);
    QObject::connect(&actionProcess_pri, &QProcess::stateChanged, this, &runProcessActionExecution_c::readProcessState_f);
}

void runProcessActionExecution_c::derivedExecute_f()
{
#ifdef DEBUGJOUVEN
    //qDebug() << "runProcessActionExecution_c::execute_f()" << Qt::endl;
#endif
    bool errorHappenedTmp(false);
    while (true)
    {
        QStringList argumentsTmp;
        for (const argument_c& argument_ite_con : runProcessActionPtr_pri->arguments_f())
        {
            if (argument_ite_con.enabled_f())
            {
                argumentsTmp.append(argument_ite_con.argumentParsed_f(runProcessActionPtr_pri->actonDataHubParent_f()->executionOptions_f().stringParserMap_f()));
            }
        }
        QDir workingDirectoryTmp(runProcessActionPtr_pri->workingDirectoryParsed_f());
        if (not workingDirectoryTmp.exists())
        {
            emitExecutionMessage_f({"Working directory {0} doesn't exists", runProcessActionPtr_pri->workingDirectoryParsed_f()}, executionMessage_c::type_ec::error);
            errorHappenedTmp = true;
            break;
        }

        //if a "clear" environment must be used OR there is something to append to the environment
        //THEN modify the environment
        const bool envModified(not runProcessActionPtr_pri->useProgramEnvironment_f() or not runProcessActionPtr_pri->environmentToAdd_f().isEmpty());
        if (envModified)
        {
            QProcessEnvironment processEnvironmentTmp(QProcessEnvironment::systemEnvironment());
            if (not runProcessActionPtr_pri->useProgramEnvironment_f())
            {
                processEnvironmentTmp.clear();
#ifdef DEBUGJOUVEN
                qDebug() << "processEnvironmentTmp.clear()" << Qt::endl;
                qDebug() << "actionProcess_pri.environment().isEmpty() " << actionProcess_pri.processEnvironment().isEmpty() << Qt::endl;
#endif
            }

            if (not runProcessActionPtr_pri->environmentToAdd_f().isEmpty())
            {
                const QHash<QString, environmentPairConfig_c> environmentToAddTmp_con(runProcessActionPtr_pri->environmentToAddParsed_f());
                QHash<QString, environmentPairConfig_c>::const_iterator iteratorTmp(environmentToAddTmp_con.constBegin());
                while (iteratorTmp not_eq environmentToAddTmp_con.constEnd())
                {
                    if (iteratorTmp.value().enabled_f())
                    {
#ifdef DEBUGJOUVEN
                        qDebug() << "iteratorTmp.keyParsed() " << iteratorTmp.key()
                                 << "iteratorTmp.valueParsed_f() " << iteratorTmp.value().environmentValueParsed_f(runProcessActionPtr_pri->actonDataHubParent_f()->executionOptions_f().stringParserMap_f())  << Qt::endl;
#endif
                        processEnvironmentTmp.insert(iteratorTmp.key(), iteratorTmp.value().environmentValueParsed_f(runProcessActionPtr_pri->actonDataHubParent_f()->executionOptions_f().stringParserMap_f()));
                    }
                    ++iteratorTmp;
                }
            }
            actionProcess_pri.setProcessEnvironment(processEnvironmentTmp);
#ifdef DEBUGJOUVEN
            qDebug() << "actionProcess_pri.environment().isEmpty() " << actionProcess_pri.processEnvironment().isEmpty() << Qt::endl;
            qDebug() << "actionProcess_pri.processEnvironment().value(\"PATH\") " << actionProcess_pri.processEnvironment().value("PATH") << Qt::endl;
#endif
        }
        else
        {
            //nothing to do
        }
#ifdef DEBUGJOUVEN
        qDebug() << "workingDirectoryParsed_f() " << runProcessActionPtr_pri->workingDirectoryParsed_f() << Qt::endl;
        qDebug() << "actionProcess_pri.workingDirectory() before set: " << actionProcess_pri.workingDirectory() << Qt::endl;
#endif
        actionProcess_pri.setWorkingDirectory(runProcessActionPtr_pri->workingDirectoryParsed_f());
#ifdef DEBUGJOUVEN
        qDebug() << "actionProcess_pri.workingDirectory() after set: " << actionProcess_pri.workingDirectory() << Qt::endl;
#endif
#ifdef Q_OS_WIN
        //(WINDOWS) use a console to run a process, otherwise it's impossible to stop them nicely
        //and the only option is "hard" kill
        actionProcess_pri.setCreateProcessArgumentsModifier([] (QProcess::CreateProcessArguments *args)
        {
            args->flags |= CREATE_NEW_CONSOLE;
            args->inheritHandles = true;
            //args->startupInfo->hStdError = GetStdHandle(STD_ERROR_HANDLE);
            //args->startupInfo->hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
            //args->startupInfo->dwFlags &= ~STARTF_USESTDHANDLES;
        });
#endif
        //TODO? issue an error on the "qt bug site" that even with a clear environment, no PATH set, it manages to run stuff in /usr/bin
        //IMPORTANT ignore above, the issue is that to find the process location path what is used is the actonQtg environment (not actionProcess_pri),
        //qprocess::setProcessEnvironment set variables of the QProcess which won't help,
        //so actonQtg environment must be modified, use relative paths or use absolute path
        actionProcess_pri.start(runProcessActionPtr_pri->processPathParsed_f(), argumentsTmp);
        {
            text_c logMessageTmp("Process {0} started", runProcessActionPtr_pri->processPathParsed_f());
            MACRO_ADDLOG(logMessageTmp, runProcessActionPtr_pri, messageType_ec::information);
        }
        break;
    }
    if (errorHappenedTmp)
    {
        Q_EMIT anyFinish_signal();
    }
}

void runProcessActionExecution_c::derivedStop_f()
{
    terminateExecution_f();
}

void runProcessActionExecution_c::derivedKill_f()
{
    QTimer::singleShot(runProcessActionPtr_pri->killTimeoutMilliseconds_f(), this, &runProcessActionExecution_c::killExecution_f);
}

void runProcessActionExecution_c::terminateExecution_f()
{
    if (actionProcess_pri.state() == QProcess::Running)
    {
        MACRO_ADDLOG("Terminate while executing, this is SIGTERM(Linux) or WM_CLOSE(Windows)", runProcessActionPtr_pri, messageType_ec::information);
        actionProcess_pri.terminate();
    }
}

void runProcessActionExecution_c::killExecution_f()
{
    if (actionProcess_pri.state() == QProcess::Running)
    {
        MACRO_ADDLOG("Kill while executing, this is SIGKILL(Linux or Windows)", runProcessActionPtr_pri, messageType_ec::information);
        actionProcess_pri.kill();
    }
}

void runProcessActionExecution_c::readError_f(QProcess::ProcessError error_par)
{
    //requires test, maybe the switch case isn't needed
    //if nothing comes out of errorString, use the documentation descriptions
    //some of the errors aren't critical like timeout or the read/write ones

    //if the error is not given by the process being run
    //i.e. process file no existing
    bool calleeErrorTmp(false);
    bool anyFinishTmp(false);
    QString errorStrTmp;
    switch (error_par)
    {
        case QProcess::FailedToStart:
        {
            errorStrTmp.append(actionProcess_pri.errorString());
            calleeErrorTmp = true;
            anyFinishTmp = true;
        }
        break;
        case QProcess::Crashed:
        {
            errorStrTmp.append(actionProcess_pri.errorString());
            anyFinishTmp = true;
        }
        break;
            //this only applies for the functions that block the thread (won't use them)
//        case QProcess::Timedout:
//        {
//            errorStrTmp.append(actionProcess_pri.errorString());
//        }
//        break;
            //this would apply if anything was written into the process while executing,
            //right now nothing is, maybe in the future
//        case QProcess::WriteError:
//        {
//            errorStrTmp.append(actionProcess_pri.errorString());
//        }
//        break;
            //this one shouldn't happen because "An error occurred when attempting to read from the process. For example, the process may not be executing."
            //reading only happens when the readReady signal happens and I don't think that it would signal to read when that would trigger an error
//        case QProcess::ReadError:
//        {
//            errorStrTmp.append(actionProcess_pri.errorString());
//        }
//        break;
            //I don't know if this happens when the one at fault is the process or the callee
        case QProcess::UnknownError:
        {
            MACRO_ADDLOG("QProcess::UnknownError", runProcessActionPtr_pri, messageType_ec::warning);
            errorStrTmp.append(actionProcess_pri.errorString());
        }
        break;
        default:
        {
            errorStrTmp.append("runProcess QProcess error switch case default");
            //theoretically it shouldn't enter here ever, default is QProcess::UnknownError
        }
    }

    //MACRO_ADDACTONQTSOLOG("same thread as main " + QSTRINGBOOL(QThread::currentThread() == QCoreApplication::instance()->thread()), messageType_ec::debug);
    MACRO_ADDLOG(errorStrTmp, runProcessActionPtr_pri, messageType_ec::warning);
    text_c errorTmp("Callee error {0}", QSTRINGBOOL(calleeErrorTmp));
    MACRO_ADDLOG(errorTmp, runProcessActionPtr_pri, messageType_ec::warning);
    //if QProcess error messages are fixed, it should be translatable
    emitExecutionMessage_f(errorStrTmp, executionMessage_c::type_ec::error);
    if (calleeErrorTmp)
    {
        //the execution never started and setFinished(in this class) won't be called
        Q_EMIT anyFinish_signal();
    }
    else
    {
        if (anyFinishTmp and not setFinishedCalled_pri)
        {
            setFinished_f(actionProcess_pri.exitCode(), actionProcess_pri.exitStatus());
        }
    }
}

//void runProcessActionExecution_c::setStarted_f()
//{
//    Q_EMIT executionStateChange_signal(actionExecutionState_ec::executing);
//}

void runProcessActionExecution_c::setFinished_f(int exitCode_par, QProcess::ExitStatus exitStatus_par)
{
    if (setFinishedCalled_pri)
    {
        return;
    }
    setFinishedCalled_pri = true;
    Q_EMIT setReturnCode_signal(exitCode_par);
    text_c textTmp("Exit code {0}", exitCode_par);
    MACRO_ADDLOG(textTmp, runProcessActionPtr_pri, messageType_ec::information);
    if (exitStatus_par == QProcess::ExitStatus::CrashExit)
    {
        //MACRO_ADDACTONQTSOLOG("Crash exit, shoudn't? enter here", messageType_ec::debug);
        //theoretically readError_f takes care of this already
        //Q_EMIT executionStateChange_signal(actionExecutionState_ec::error);
        emitExecutionMessage_f("Crash exit", executionMessage_c::type_ec::error);
    }
    else
    {
        //FUTURE might need an extra setting of what return code is error/success
        //return code != 0, doesn't always mean it's an error
//        if (exitCode_par == 0)
//        {

//        }
//        else
//        {

//        }
        emitExecutionMessage_f({"Exit/return code: {0}", exitCode_par}, executionMessage_c::type_ec::information);
    }
    Q_EMIT anyFinish_signal();
}

void runProcessActionExecution_c::readStderr_f()
{
    QString stderrTmp(actionProcess_pri.readAllStandardError());
    emitExecutionMessage_f(stderrTmp, executionMessage_c::type_ec::externalstderr);
}

void runProcessActionExecution_c::readStdout_f()
{
    QString stdoutTmp(actionProcess_pri.readAllStandardOutput());
    emitExecutionMessage_f(stdoutTmp, executionMessage_c::type_ec::externalsdout);
}

void runProcessActionExecution_c::readProcessState_f(QProcess::ProcessState newState_par)
{
#ifdef DEBUGJOUVEN
    //I need to see what output this gives
    qDebug() << "readProcessState_f newState_par" << newState_par << Qt::endl;
#endif
}

