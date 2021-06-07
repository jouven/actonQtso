#ifndef ACTONQTSO_BASEACTIONEXECUTION_HPP
#define ACTONQTSO_BASEACTIONEXECUTION_HPP

#include "../actionMappings/actionExecutionStates.hpp"
#include "../executionMessage.hpp"

#include <QObject>

class actionExecutionResult_c;

class baseActionExecution_c : public QObject
{
    Q_OBJECT

    bool executionError_pri = false;

    virtual void derivedExecute_f() = 0;
    //end nicely, if possible
    virtual void derivedStop_f() = 0;
    //forced end, will end regardless of how things are left
    virtual void derivedKill_f() = 0;
private Q_SLOTS:
    void setExecutionError_f(const executionMessage_c* messagePtr_par_con);
protected:
    actionExecutionResult_c* const actionExecutionResultObj_pri = nullptr;

    baseActionExecution_c(actionExecutionResult_c* actionExecutionResultObj_par_con);//, QObject* parent_par);

    void emitExecutionMessage_f(const textCompilation_c& message_par_con, const executionMessage_c::type_ec type_par_con);
    void emitExecutionMessage_f(const text_c& message_par_con, const executionMessage_c::type_ec type_par_con);
public:
    void execute_f();

    void stop_f();

    void kill_f();
    //stop should always be called before kill (kill should happen after a timeout from calling stop)
    //stop or kill might not be implemented on some actions, i.e. createDirectory
    //stop should leave the action in a known state, resumable or not
    //kill should finish the action whatever the state/outcome (an action shouldn't be able to hang the "program-library")

    //to hint the execution has failed
    bool executionError_f() const;

    //signals are used because the execution object is always in a different thread than
    //the results object, because if the main thread, which is where the result obj is,
    //were to access the result values mid-update, using regular functions instead of signals,
    //it could get "half-updated" values
Q_SIGNALS:
    void executionStateChange_signal(const actionExecutionState_ec actionExecutionState_par_con);
    //from subclasses use emitMessage instead
    void addExecutionMessage_signal(executionMessage_c* messagePtr_par_con);
    //when the process "finishes" in any way, error, user stopped/killed, successful
    void anyFinish_signal();
};

#endif // ACTONQTSO_BASEACTIONEXECUTION_HPP
