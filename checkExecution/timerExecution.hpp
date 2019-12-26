#ifndef ACTONQTSO_TIMEREXECUTION_HPP
#define ACTONQTSO_TIMEREXECUTION_HPP

#include "baseCheckExecution.hpp"

class timerCheck_c;
class QTimer;

class timerCheckExecution_c : public baseCheckExecution_c
{
    Q_OBJECT

    QTimer* timer_pri = nullptr;

    const timerCheck_c* const timerCheckPtr_pri = nullptr;
protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
public:
    timerCheckExecution_c() = delete;
    explicit timerCheckExecution_c(
            checkDataExecutionResult_c* checkExecutionResultObj_par_con
            , timerCheck_c* timerCheckPtr_par
    );
private Q_SLOTS:
    void timerFinished_f();
};

#endif // ACTONQTSO_TIMEREXECUTION_HPP
