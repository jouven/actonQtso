#ifndef ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP
#define ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP

#include "baseCheckExecution.hpp"

#include "../checks/actionFinished.hpp"

class actionData_c;
//QObject objects of this can't be copied, fields can
class actionFinishedCheckExecution_c : public baseCheckExecution_c, public actionFinishedCheck_c
{
    Q_OBJECT

public:
    actionFinishedCheckExecution_c() = delete;
    explicit actionFinishedCheckExecution_c(
            const actionFinishedCheck_c& actionFinishedCheck_par_con
    );

    void execute_f() override;
//Q_SIGNALS:

private Q_SLOTS:
    void actionFinished_f(actionData_c* const actionData_par_ptr_con);
    void verifyStopped_f();
};

#endif // ACTONQTSO_ACTIONFINISHEDEXECUTION_HPP
