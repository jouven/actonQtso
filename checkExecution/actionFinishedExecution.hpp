#ifndef ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP
#define ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP

#include "baseCheckExecution.hpp"

class executionResult_c;
class actionFinishedCheck_c;
//class action_c;

class actionFinishedCheckExecution_c : public baseCheckExecution_c
{
    Q_OBJECT

    const actionFinishedCheck_c* const actionFinishedCheckPtr_pri = nullptr;
    uint_fast64_t finishedCount_pri = 0;
protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
public:
    actionFinishedCheckExecution_c() = delete;
    explicit actionFinishedCheckExecution_c(
            checkExecutionResult_c* checkExecutionResultObj_par_con
            , actionFinishedCheck_c* actionFinishedCheckPtr_par
    );

private Q_SLOTS:
    void actionFinished_f(executionResult_c* const executionResult_par_ptr_con);
};

#endif // ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP
