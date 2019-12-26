#include "metaEndExecutionCycleExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/metaEndExecutionCycle.hpp"
#include "../actonDataHub.hpp"

metaEndExecutionCycleActionExecution_c::metaEndExecutionCycleActionExecution_c(
        actionDataExecutionResult_c* actionExecutionResultObj_par_con
        , metaEndExecutionCycleAction_c* metaEndExecutionCycleActionPtr_par
)
    : baseActionExecution_c(actionExecutionResultObj_par_con)
    , metaEndExecutionCycleActionPtr_pri(metaEndExecutionCycleActionPtr_par)
{}

void metaEndExecutionCycleActionExecution_c::derivedExecute_f()
{
    while (true)
    {
        if (metaEndExecutionCycleActionPtr_pri->endType_f() == metaEndExecutionCycleAction_c::endType_ec::stop)
        {
            MACRO_ADDACTONQTSOLOG("Try stop execution", logItem_c::type_ec::warning);
            actonDataHub_ptr_ext->tryStopExecutingActions_f(metaEndExecutionCycleActionPtr_pri->killAfterTimeout_f());
            break;
        }
        if (metaEndExecutionCycleActionPtr_pri->endType_f() == metaEndExecutionCycleAction_c::endType_ec::waitToFinish)
        {
            MACRO_ADDACTONQTSOLOG("Stop after current execution cycle finishes", logItem_c::type_ec::warning);
            actonDataHub_ptr_ext->stopWhenCurrentExecutionCycleFinishes_f();
            break;
        }
        break;
    }
    Q_EMIT anyFinish_signal();
}

void metaEndExecutionCycleActionExecution_c::derivedStop_f()
{
    //no need
}

void metaEndExecutionCycleActionExecution_c::derivedKill_f()
{
    //no need
}


