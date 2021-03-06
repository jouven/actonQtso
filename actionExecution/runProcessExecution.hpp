#ifndef ACTONQTSO_RUNPROCESSEXECUTION_HPP
#define ACTONQTSO_RUNPROCESSEXECUTION_HPP

#include "baseActionExecution.hpp"

#include <QProcess>

class runProcessAction_c;
class executionMessage_c;

class runProcessActionExecution_c : public baseActionExecution_c
{
    Q_OBJECT

    const runProcessAction_c* const runProcessActionPtr_pri = nullptr;

    //the function "setFinished_f"
    bool setFinishedCalled_pri = false;

    QProcess actionProcess_pri;

    //when executing: actionProcess_pri.terminate();
    void terminateExecution_f();

protected:
    void derivedExecute_f() override;
    //sends sigterm
    void derivedStop_f() override;
    //sends sigkill
    void derivedKill_f() override;
public:
    runProcessActionExecution_c() = delete;
    explicit runProcessActionExecution_c(
            actionExecutionResult_c* actionExecutionResultObj_par_con
            , runProcessAction_c* runProcessActionPtr_par
            , const bool mergeOutErr_par_con = false
            , const int_fast32_t timeoutMilliseconds_par_con = 0
    );

Q_SIGNALS:
    void setReturnCode_signal(const int returnCode_par_con);

private Q_SLOTS:
    //slots to catch QProcess signals
    void readError_f(QProcess::ProcessError error_par);
    //void setStarted_f();
    void setFinished_f(int exitCode_par, QProcess::ExitStatus exitStatus_par);
    void readStderr_f();
    void readStdout_f();
    void readProcessState_f(QProcess::ProcessState newState_par);
    //when executing: actionProcess_pri.kill();
    void killExecution_f();
};

#endif // ACTONQTSO_RUNPROCESSEXECUTION_HPP
