#ifndef ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP
#define ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP

#include "baseCheckExecution.hpp"

#include "../checks/actionFinished.hpp"

class actionData_c;
//QObject objects of this can't be copied, fields can
class actionFinishedCheckExecution_c : public baseCheckExecution_c, public actionFinishedCheck_c
{
    Q_OBJECT

protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
public:
    actionFinishedCheckExecution_c() = delete;
    explicit actionFinishedCheckExecution_c(
            checkDataExecutionResult_c* checkExecutionResultObj_par_con
            , const actionFinishedCheck_c& actionFinishedCheck_par_con
    );

private Q_SLOTS:
    void actionFinished_f(actionData_c* const actionData_par_ptr_con);
};

#endif // ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP
