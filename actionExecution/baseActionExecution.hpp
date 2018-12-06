#ifndef ACTONQTSO_BASEACTIONEXECUTION_HPP
#define ACTONQTSO_BASEACTIONEXECUTION_HPP

#include "../actionMappings/actionExecutionStates.hpp"

#include <QObject>

class baseActionExecution_c : public QObject
{
    Q_OBJECT

public:
    baseActionExecution_c();

    virtual void execute_f() = 0;

    //stop should always be called before kill (kill should happen after a timeout from calling stop)
    //stop or kill might not be implemented on some actions, i.e. createDirectory
    //stop should leave the action in a known state, resumable or not
    //kill should finish the action whatever the state/outcome (an action shouldn't be able to stuck this "library-program")

    //end nicely, if possible
    virtual void stop_f() = 0;
    //forced end, will end regardless of how things are left
    virtual void kill_f() = 0;
Q_SIGNALS:
    void executionStateChange_signal(const actionExecutionState_ec actionExecutionState_par_con);
    //this action output
    void addOutput_signal(const QString& output_par_con);
    //this action errors
    void addError_signal(const QString& error_par_con);
    //when the process "finishes" in any way, error, user stopped, successful
    void anyFinish_signal();
};

#endif // ACTONQTSO_BASEACTIONEXECUTION_HPP
