#ifndef ACTONQTSO_METAENDEXECUTIONCYCLEEXECUTION_HPP
#define ACTONQTSO_METAENDEXECUTIONCYCLEEXECUTION_HPP

#include "baseActionExecution.hpp"

class metaEndExecutionCycleAction_c;

class metaEndExecutionCycleActionExecution_c : public baseActionExecution_c
{
    Q_OBJECT

    const metaEndExecutionCycleAction_c* const metaEndExecutionCycleActionPtr_pri = nullptr;
protected:
    void derivedExecute_f() override;
    //does nothing
    void derivedStop_f() override;
    //does nothing
    void derivedKill_f() override;
public:
    metaEndExecutionCycleActionExecution_c() = delete;
    explicit metaEndExecutionCycleActionExecution_c(
            actionExecutionResult_c* actionExecutionResultObj_par_con
            , metaEndExecutionCycleAction_c* metaEndExecutionCycleActionPtr_par
    );
};

#endif // ACTONQTSO_METAENDEXECUTIONCYCLEEXECUTION_HPP
