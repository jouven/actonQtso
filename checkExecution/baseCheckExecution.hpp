#ifndef ACTONQTSO_BASECHECKEXECUTION_HPP
#define ACTONQTSO_BASECHECKEXECUTION_HPP

#include "../checkMappings/checkExecutionStates.hpp"

#include <QObject>

class text_c;

class checkDataExecutionResult_c;

class baseCheckExecution_c : public QObject
{
    Q_OBJECT

    bool executionError_pri = false;

    virtual void derivedExecute_f() = 0;
    virtual void derivedStop_f() = 0;
private Q_SLOTS:
    void setExecutionError_f();
protected:
    checkDataExecutionResult_c* const checkExecutionResultObj_pro = nullptr;

    baseCheckExecution_c(checkDataExecutionResult_c* checkExecutionResultObj_par_con);
public:

    void stop_f();

    void execute_f();

    //to hint the execution has failed
    bool executionError_f() const;

Q_SIGNALS:
    void executionStateChange_signal(const checkExecutionState_ec& checkExecutionState_par_con);
    //this check errors
    void addError_signal(const text_c& error_par_con);
    //when the check "finishes" in any way
    void anyFinish_signal(const bool result_par_con);
};



#endif // ACTONQTSO_BASECHECKEXECUTION_HPP
