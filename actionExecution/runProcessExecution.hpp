#ifndef ACTONQTG_RUNPROCESSEXECUTION_HPP
#define ACTONQTG_RUNPROCESSEXECUTION_HPP

#include "../actions/runProcess.hpp"
#include "../mappings/actionExecutionStates.hpp"

#include <QProcess>
#include <QObject>

//QObject objects of this can't be copied, fields can
class runProcessActionExecution_c : public QObject, public runProcessAction_c
{
    Q_OBJECT

    //to prevent reruns
    bool startedOnce_pri = false;

    QProcess actionProcess_pri;
public:
    runProcessActionExecution_c() = delete;
    explicit runProcessActionExecution_c(
            const runProcessAction_c& processAction_par_con
            , const bool mergeOutErr_par_con = false
            , const int_fast32_t timeoutMilliseconds_par_con = 0
    );

    //int returnValue_f() const;
    //returns true if was successfull trying to execute/start the process
    //this has to do with retrying to execute the process or executing it again when already has ended
    //a process can only be executed/started once per object
    bool execute_f();
    void terminateExecution_f();
    void killExecution_f();
Q_SIGNALS:
    void executionStateChange_signal(const actionExecutionState_ec actionExecutionState_par_con);
    //no need to emit output for the process since the meaningful output comes from the process
    //AKA external output
    //this action errors
    void addError_signal(const QString& error_par_con);
    //for the called process stdout
    void addProcessOutput_signal(const QString& processOutput_par_con);
    //for the called process stderr
    void addProcessError_signal(const QString& processError_par_con);

    void setReturnCode_signal(const int returnCode_par_con);
    //when the process "finishes" in any way, error, user stopped, successful
    void anyFinish_signal();
private Q_SLOTS:
    //slots for QProcess signal catching
    void readError_f(QProcess::ProcessError error_par);
    void setStarted_f();
    void setFinished_f(int exitCode_par, QProcess::ExitStatus exitStatus_par);
    void readStderr_f();
    void readStdout_f();
    void readProcessState_f(QProcess::ProcessState newState_par);

};

#endif // ACTONQTG_RUNPROCESSEXECUTION_HPP
