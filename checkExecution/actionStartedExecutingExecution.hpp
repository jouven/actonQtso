#ifndef ACTONQTSO_ACTIONSTARTEDEXECUTINGEXECUTION_HPP
#define ACTONQTSO_ACTIONSTARTEDEXECUTINGEXECUTION_HPP

#include "baseCheckExecution.hpp"

class executionResult_c;
class actionStartedExecutingCheck_c;
class action_c;

class actionStartedExecutingCheckExecution_c : public baseCheckExecution_c
{
    Q_OBJECT

    const actionStartedExecutingCheck_c* const actionStartedExecutingCheckPtr_pri = nullptr;

protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
public:
    actionStartedExecutingCheckExecution_c() = delete;
    explicit actionStartedExecutingCheckExecution_c(
            checkExecutionResult_c* checkExecutionResultObj_par_con
            , actionStartedExecutingCheck_c* actionStartedExecutingCheckPtr_par
    );
private Q_SLOTS:
    void actionStarted_f(executionResult_c* const executionResult_par_ptr_con);
    void finishCheck_f();
};

#endif // ACTONQTSO_ACTIONSTARTEDEXECUTINGEXECUTION_HPP
