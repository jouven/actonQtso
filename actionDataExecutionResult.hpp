#ifndef ACTONQTSO_ACTIONDATARESULT_HPP
#define ACTONQTSO_ACTIONDATARESULT_HPP

#include "actionMappings/actionExecutionStates.hpp"

#include <QObject>
#include <QString>

class actionData_c;
//this class it to deal in a generic way with all the action execution result/s
//so a createDirectory execution result/s can be displayed: display the directory path that has been created, or an error of why it couldn't
//but at the same runProcess execution result/s: return code, process output (stdout and stderr) and internal errors
//the results should be easily able to be show in console and in the gui version
//
class actionDataExecutionResult_c : public QObject
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

    actionExecutionState_ec executionState_pri = actionExecutionState_ec::initial;

    bool started_pri = false;
    qint64 startTime_pri = 0;
    bool finished_pri = false;
    qint64 finishedTime_pri = 0;

    bool stoppedByUser_pri = false;
    bool killedByUser_pri = false;

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

    actionExecutionState_ec state_f() const;
    actionData_c* parent_ptr_f() const;

    bool started_f() const;
    bool finished_f() const;
    bool stoppedByUser_f() const;

    //signals for these in the future?
    //no need since they can reuse anyfinish and monitor when the state changes to executing
    qint64 startTime_f() const;
    qint64 finishedTime_f() const;

    void clear_f();

Q_SIGNALS:
    void outputUpdated_signal(actionData_c* actionData_ptr_par_con);
    void errorUpdated_signal(actionData_c* actionData_ptr_par_con);

    void externalOutputUpdated_signal(actionData_c* actionData_ptr_par_con);
    void externalErrorUpdated_signal(actionData_c* actionData_ptr_par_con);

    void returnCodeUpdated_signal(actionData_c* actionData_ptr_par_con);
    void executionStateUpdated_signal(actionData_c* actionData_ptr_par_con);

    void started_signal(actionData_c* actionData_ptr_par_con);
    void finished_signal(actionData_c* actionData_ptr_par_con);

    void stopping_signal(actionData_c* actionData_ptr_par_con);
    void stopped_signal(actionData_c* actionData_ptr_par_con);

    void killing_signal(actionData_c* actionData_ptr_par_con);
    void killed_signal(actionData_c* actionData_ptr_par_con);

    void resultsCleared_signal(actionData_c* actionData_ptr_par_con);
public Q_SLOTS:
    void appendOutput_f(const QString& output_par_con);
    void appendError_f(const QString& error_par_con);

    void appendExternalOutput_f(const QString& actionOutput_par_con);
    void appendExternalError_f(const QString& actionError_par_con);
    //once set can't be changed
    void setReturnCode_f(const int returnCode_par_con);
    //some of the actionExecutionState are final, like finished, after the object can't be modified anymore
    //the other fields must be set first before transitioning into final state
    void setExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con);

    void setStarted_f();
    void setFinished_f();
};

#endif // ACTONQTSO_ACTIONDATARESULT_HPP
