#ifndef ACTONQTSO_ACTIONDATAEXECUTIONRESULT_HPP
#define ACTONQTSO_ACTIONDATAEXECUTIONRESULT_HPP

#include "executionResult.hpp"
#include "actionMappings/actionExecutionStates.hpp"

#include "crossPlatformMacros.hpp"

#include <vector>

class action_c;
//this class it's to deal in a generic way with all the action execution result/s
//so a createDirectory execution result/s can be displayed: display the directory path that has been created, or an error of why it couldn't
//but aalso runProcess execution result/s: return code, process output (stdout and stderr) and internal errors
//the results should be easily able to be show in console and in the gui version
//for how this objects lives check executionResult_c hpp
class EXPIMP_ACTONQTSO actionExecutionResult_c : public executionResult_c
{
    Q_OBJECT

    //this might only apply to the runProcess type, 0 should be success, anything else error,
    //BUT each process might vary
    //usually? it's a number from 0 to 255, https://en.wikipedia.org/wiki/Return_code
    int returnCode_pri = -1;
    bool returnCodeSet_pri = false;

    action_c* const action_ptr_pri;

    //FUTURE add state change time, so state durations can be observed
    std::vector<actionExecutionState_ec> executionStateVector_pri = { actionExecutionState_ec::initial };

    bool killedByUser_pri = false;

    //will clear if finished
    //void derivedTryClear_f() override;

    //void derivedSetStarted_f() override;
    bool derivedTrySetFinished_f() override;

    void derivedAppendMessage_f(executionMessage_c* message_par_con) override;
public:

    explicit actionExecutionResult_c(
            //internally the action is only used for the signals and its getter function
            action_c* action_par_ptr_con
            , QObject* parent_par
    );

    int returnCode_f() const;
    bool returnCodeSet_f() const;

    std::vector<actionExecutionState_ec> executionStateVector_f() const;
    actionExecutionState_ec lastState_f() const;
    action_c* action_ptr_f() const;

    bool killedByUser_f() const;

    type_ec type_f() const override;
    QString stateString_f() const override;
    QString derivedElementTypeString_f() const override;
    QString derivedElementDescription_f() const override;
Q_SIGNALS:

    void externalStdoutAdded_signal(action_c* action_ptr_par, const executionMessage_c* stdoutMessage_par_con);
    void externalStdErrAdded_signal(action_c* action_ptr_par, const executionMessage_c* stderrMessage_par_con);

    void killing_signal(action_c* action_ptr_par);
    void killed_signal(action_c* action_ptr_par);

    void returnCodeSet_signal(action_c* action_ptr_par);
    void executionStateUpdated_signal(action_c* action_ptr_par, actionExecutionState_ec executionState_par_con);

    //20191118 these 4 aren't being used, not here, not in actonQtg
    void executingChecks_signal(action_c* action_ptr_par);
    void success_signal(action_c* action_ptr_par);

    //void resultsCleared_signal(action_c* action_ptr_par);
    //the slots should only be used by the action/execution object
public Q_SLOTS:
    //some of the actionExecutionState are final, like success, after they been set the object can't be modified anymore (without clearing it first)
    //final states: success, killed, stopped and error, except error the other states can't be set,
    //they are set when trySetFinished_f is called
    bool trySetExecutionState_f(const actionExecutionState_ec actionExecutionState_par_con);

    //once set can't be changed (except by a successful derivedTryClear_f)
    void setReturnCode_f(const int returnCode_par_con);
};

#endif // ACTONQTSO_ACTIONDATAEXECUTIONRESULT_HPP
