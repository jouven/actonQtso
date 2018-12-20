#ifndef ACTONQTSO_ACTIONDATARESULT_HPP
#define ACTONQTSO_ACTIONDATARESULT_HPP

#include "actionMappings/actionExecutionStates.hpp"

#include "crossPlatformMacros.hpp"

#include <QObject>
#include <QString>

#include <vector>

class actionData_c;
//this class it to deal in a generic way with all the action execution result/s
//so a createDirectory execution result/s can be displayed: display the directory path that has been created, or an error of why it couldn't
//but at the same runProcess execution result/s: return code, process output (stdout and stderr) and internal errors
//the results should be easily able to be show in console and in the gui version
//
class EXPIMP_ACTONQTSO actionDataExecutionResult_c : public QObject
{
    Q_OBJECT
    //description of what the action did successfully during the execution
    QString output_pri;
    //when the action recieves external output like in runProcess
    QString externalOutput_pri;
    //when the action recieves external error output like in runProcess
    QString externalErrorOutput_pri;
    //description of any errors during the execution of this action
    QString errors_pri;
    //this might only apply to the runProcess kind, 0 should be success, anything else error,
    //BUT each process might vary
    //usually? it's a number from 0 to 255, https://en.wikipedia.org/wiki/Return_code
    int returnCode_pri = -1;
    bool returnCodeSet_pri = false;

    actionData_c* const parent_ptr_pri;

    //FUTURE add state change time, so state durations can be observed
    std::vector<actionExecutionState_ec> executionStateVector_pri = { actionExecutionState_ec::initial };

    bool started_pri = false;
    qint64 startTime_pri = 0;
    bool finished_pri = false;
    qint64 finishedTime_pri = 0;

    bool stoppedByUser_pri = false;
    bool killedByUser_pri = false;

    void setStarted_f();
public:
    explicit actionDataExecutionResult_c(
            actionData_c* const parentActionData_par_ptr_con
    );

    QString output_f() const;
    QString error_f() const;

    QString externalOutput_f() const;
    QString externalErrorOutput_f() const;

    int returnCode_f() const;
    bool returnCodeSet_f() const;

    std::vector<actionExecutionState_ec> executionStateVector_f() const;
    actionExecutionState_ec lastState_f() const;
    actionData_c* parent_ptr_f() const;

    bool started_f() const;
    bool finished_f() const;
    bool stoppedByUser_f() const;

    //start time begins when the execution state changes from initial to executingChecks or preparing
    qint64 startTime_f() const;
    qint64 finishedTime_f() const;

    //will clear if finished
    bool tryClear_f();

Q_SIGNALS:
    void outputUpdated_signal(actionData_c* actionData_ptr_par_con);

    void externalOutputUpdated_signal(actionData_c* actionData_ptr_par_con);
    void externalErrorUpdated_signal(actionData_c* actionData_ptr_par_con);

    void returnCodeSet_signal(actionData_c* actionData_ptr_par_con);
    void executionStateUpdated_signal(actionData_c* actionData_ptr_par_con);

    void started_signal(actionData_c* actionData_ptr_par_con);
    void finished_signal(actionData_c* actionData_ptr_par_con);

    void stopping_signal(actionData_c* actionData_ptr_par_con);
    void stopped_signal(actionData_c* actionData_ptr_par_con);

    void killing_signal(actionData_c* actionData_ptr_par_con);
    void killed_signal(actionData_c* actionData_ptr_par_con);

    void preparing_signal(actionData_c* actionData_ptr_par_con);
    void executingChecks_signal(actionData_c* actionData_ptr_par_con);
    void executing_signal(actionData_c* actionData_ptr_par_con);
    void success_signal(actionData_c* actionData_ptr_par_con);
    void error_signal(actionData_c* actionData_ptr_par_con);

    void resultsCleared_signal(actionData_c* actionData_ptr_par_con);
    //the slots should only be used by the action/execution object
public Q_SLOTS:
    //some of the actionExecutionState are final, like finished, after the object can't be modified anymore
    bool trySetExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con);

    void appendOutput_f(const QString& output_par_con);
    //changes the execution state to error
    void appendError_f(const QString& error_par_con);

    void appendExternalOutput_f(const QString& actionOutput_par_con);
    void appendExternalError_f(const QString& actionError_par_con);
    //once set can't be changed
    void setReturnCode_f(const int returnCode_par_con);

    void trySetFinished_f();
};

#endif // ACTONQTSO_ACTIONDATARESULT_HPP
