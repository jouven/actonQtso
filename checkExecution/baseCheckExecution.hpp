#ifndef ACTONQTSO_BASECHECKEXECUTION_HPP
#define ACTONQTSO_BASECHECKEXECUTION_HPP

#include "../checkMappings/checkExecutionStates.hpp"
#include "../executionMessage.hpp"

#include <QObject>

class text_c;

class checkExecutionResult_c;

class baseCheckExecution_c : public QObject
{
    Q_OBJECT

    bool executionError_pri = false;

    virtual void derivedExecute_f() = 0;
    virtual void derivedStop_f() = 0;
private Q_SLOTS:
    void setExecutionError_f(const executionMessage_c* messagePtr_par_con);
protected:
    checkExecutionResult_c* const checkExecutionResultObj_pro = nullptr;

    baseCheckExecution_c(checkExecutionResult_c* checkExecutionResultObj_par_con);

    //TODO AT THE END 20210218 noticed that derived class executions only emit error/s and actonQtg in the checksWindow only has a column
    //for the error/s but the possibility of emiting information messages is there too
    void emitExecutionMessage_f(const textCompilation_c& message_par_con, const executionMessage_c::type_ec type_par_con);
    void emitExecutionMessage_f(const text_c& message_par_con, const executionMessage_c::type_ec type_par_con);
public:

    void stop_f();

    void execute_f();

    //to hint the execution has failed
    bool executionError_f() const;

Q_SIGNALS:
    void executionStateChange_signal(const checkExecutionState_ec& checkExecutionState_par_con);
    //from subclasses use emitMessage instead
    void addExecutionMessage_signal(executionMessage_c* messagePtr_par_con);
    //when the check "finishes" in any way
    void anyFinish_signal(const bool result_par_con);
};



#endif // ACTONQTSO_BASECHECKEXECUTION_HPP
