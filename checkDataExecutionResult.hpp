#ifndef ACTONQTSO_CHECKDATAEXECUTIONRESULT_HPP
#define ACTONQTSO_CHECKDATAEXECUTIONRESULT_HPP

#include "executionResult.hpp"
#include "checkMappings/checkExecutionStates.hpp"

#include <vector>

#include "crossPlatformMacros.hpp"


class check_c;
//this class it to deal in a generic way with all the check execution result/s
//a bit different from the action one, since the only things that matter are the result (true or false) and if there is any error
//state kept just to hint to where it fails when it fails
class EXPIMP_ACTONQTSO checkExecutionResult_c : public executionResult_c
{
    Q_OBJECT

    check_c* const check_ptr_pri;

    std::vector<checkExecutionState_ec> executionStateVector_pri = { checkExecutionState_ec::initial };

    bool result_pri = false;
    bool resultSet_pri = false;

    //will clear if finished
    //void derivedTryClear_f() override;

    //void derivedSetStarted_f() override;

    //setResult_f must be called before derivedTrySetFinished_f otherwise it does nothing
    bool derivedTrySetFinished_f() override;

    void derivedAppendMessage_f(executionMessage_c* message_par_con) override;

public:
    explicit checkExecutionResult_c(
            //like in the action variant check internally is only used for the signals
            check_c* const check_par_ptr_con
            , QObject* parent_par
    );

    std::vector<checkExecutionState_ec> executionStateVector_f() const;
    checkExecutionState_ec lastState_f() const;
    check_c* check_ptr_f() const;

    //will always return false if the execution hasn't finished
    bool result_f() const;
    //accounts for checkData_c::resultLogic_f
    //and like the above it will always return false if the execution hasn't finished
    bool logicResult_f() const;

    type_ec type_f() const override;
    QString stateString_f() const override;
    QString derivedElementTypeString_f() const override;
    QString derivedElementDescription_f() const override;
Q_SIGNALS:
    void executionStateUpdated_signal(check_c* checkData_par_ptr);

    //void error_signal(check_c* actionData_ptr_par);

    //void resultsCleared_signal(check_c* checkData_par_ptr);
    //the slots should only be used by the check/execution object
public Q_SLOTS:
    //some of the checkExecutionState are final, like error, after the object can't be modified anymore
    bool trySetExecutionState_f(const checkExecutionState_ec checkExecutionState_par_con);

    //can only be set once, successful derivedTryClear_f does reset this
    void setResult_f(const bool result_par_con);
};

#endif // ACTONQTSO_CHECKDATAEXECUTIONRESULT_HPP
