#ifndef ACTONQTSO_RUNPROCESSEXECUTION_HPP
#define ACTONQTSO_RUNPROCESSEXECUTION_HPP

#include "baseActionExecution.hpp"

#include "../actions/runProcess.hpp"

#include <QProcess>

class runProcessActionExecution_c : public baseActionExecution_c, public runProcessAction_c
{
    Q_OBJECT

    //to prevent reruns
    bool startedOnce_pri = false;
    //the function "setFinished_f"
    bool setFinishedCalled_pri = false;

    QProcess actionProcess_pri;

    //when executing: actionProcess_pri.terminate();
    void terminateExecution_f();
    //when executing: actionProcess_pri.kill();
    void killExecution_f();
protected:
    void derivedExecute_f() override;
    //sends sigterm
    void derivedStop_f() override;
    //sends sigkill
    void derivedKill_f() override;
public:
    runProcessActionExecution_c() = delete;
    explicit runProcessActionExecution_c(
            actionDataExecutionResult_c* actionExecutionResultObj_par_con,
            const runProcessAction_c& processAction_par_con
            , const bool mergeOutErr_par_con = false
            , const int_fast32_t timeoutMilliseconds_par_con = 0
    );


Q_SIGNALS:
    //for the called process stdout
    void addProcessOutput_signal(const QString& processOutput_par_con);
    //for the called process stderr
    void addProcessError_signal(const QString& processError_par_con);

    void setReturnCode_signal(const int returnCode_par_con);

private Q_SLOTS:
    //slots to catch QProcess signals
    void readError_f(QProcess::ProcessError error_par);
    void setStarted_f();
    void setFinished_f(int exitCode_par, QProcess::ExitStatus exitStatus_par);
    void readStderr_f();
    void readStdout_f();
    void readProcessState_f(QProcess::ProcessState newState_par);

};

#endif // ACTONQTSO_RUNPROCESSEXECUTION_HPP
