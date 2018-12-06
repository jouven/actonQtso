#ifndef ACTONQTSO_BASECHECKEXECUTION_HPP
#define ACTONQTSO_BASECHECKEXECUTION_HPP

#include "../checkMappings/checkExecutionStates.hpp"

#include <QObject>

class baseCheckExecution_c : public QObject
{
    Q_OBJECT

    bool stopping_pri = false;
public:
    baseCheckExecution_c();

    bool stopping_f() const;
    void stop_f();

    virtual void execute_f() = 0;

Q_SIGNALS:
    void executionStateChange_signal(const checkExecutionState_ec& checkExecutionState_par_con);
    //this check errors
    void addError_signal(const QString& error_par_con);

    void anyCheckResult_signal(const bool checkResult_par_con);
    //used internally, this will trigger the object deletion
    void anyCheckFinish_signal();
};



#endif // ACTONQTSO_BASECHECKEXECUTION_HPP
