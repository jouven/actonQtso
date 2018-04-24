#ifndef ACTONQTG_CREATEDIRECTORYEXECUTION_HPP
#define ACTONQTG_CREATEDIRECTORYEXECUTION_HPP

#include "../actions/createDirectory.hpp"
#include "../mappings/actionExecutionStates.hpp"

#include <QObject>

//QObject objects of this can't be copied, fields can
class createDirectoryActionExecution_c : public QObject, public createDirectoryAction_c
{
    Q_OBJECT

    //working directory? no need because
    //there is an action for that
    
    //to prevent reruns
    bool startedOnce_pri = false;

public:
    createDirectoryActionExecution_c() = delete;
    explicit createDirectoryActionExecution_c(
            const createDirectoryAction_c& createDirectoryAction_par_con
    );

    //returns true on successful execution, maybe no permissions
    bool execute_f();
Q_SIGNALS:
    void executionStateChange_signal(const actionExecutionState_ec actionExecutionState_par_con);
    //to tell if already exists, it's not an error,
    //but it has to be conveyed in some "generic" way
    void addOutput_signal(const QString& error_par_con);
    //this action errors
    void addError_signal(const QString& error_par_con);

    void anyFinish_signal();
private Q_SLOTS:
};

#endif // ACTONQTG_CREATEDIRECTORYEXECUTION_HPP
