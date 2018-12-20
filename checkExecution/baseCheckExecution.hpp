#ifndef ACTONQTSO_BASECHECKEXECUTION_HPP
#define ACTONQTSO_BASECHECKEXECUTION_HPP

#include "../checkMappings/checkExecutionStates.hpp"

#include <QObject>

class checkDataExecutionResult_c;

class baseCheckExecution_c : public QObject
{
    Q_OBJECT
private:
    virtual void derivedExecute_f() = 0;
    virtual void derivedStop_f() = 0;
protected:
    checkDataExecutionResult_c* const checkExecutionResultObj_pri = nullptr;

    baseCheckExecution_c(checkDataExecutionResult_c* checkExecutionResultObj_par_con);
public:

    void stop_f();

    void execute_f();

Q_SIGNALS:
    void executionStateChange_signal(const checkExecutionState_ec& checkExecutionState_par_con);
    //this check errors
    void addError_signal(const QString& error_par_con);
};



#endif // ACTONQTSO_BASECHECKEXECUTION_HPP
