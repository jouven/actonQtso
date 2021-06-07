#include "metaEndExecutionCycleExecution.hpp"

#include "../actionDataExecutionResult.hpp"
#include "../actions/metaEndExecutionCycle.hpp"
#include "../actonDataHub.hpp"

#define MACRO_ADDLOG(...) \
if (metaEndExecutionCycleActionPtr_pri->actonDataHubParent_f() not_eq nullptr) \
{ \
    MACRO_ADDACTONDATAHUBLOG(metaEndExecutionCycleActionPtr_pri->actonDataHubParent_f(),__VA_ARGS__); \
}

metaEndExecutionCycleActionExecution_c::metaEndExecutionCycleActionExecution_c(
        actionExecutionResult_c* actionExecutionResultObj_par_con
        , metaEndExecutionCycleAction_c* metaEndExecutionCycleActionPtr_par
)
    : baseActionExecution_c(actionExecutionResultObj_par_con)//, metaEndExecutionCycleActionPtr_par)
    , metaEndExecutionCycleActionPtr_pri(metaEndExecutionCycleActionPtr_par)
{}

void metaEndExecutionCycleActionExecution_c::derivedExecute_f()
{
    while (true)
    {
        if (not metaEndExecutionCycleActionPtr_pri->parentIsActonDataHubObj_f())
        {
            emitExecutionMessage_f(
            {
                            "metaEndExecutionCycle action with stringId {0} has no actonDataHub parent object"
                            , metaEndExecutionCycleActionPtr_pri->stringId_f()
            }, executionMessage_c::type_ec::error);
            break;
        }

        if (metaEndExecutionCycleActionPtr_pri->endType_f() == metaEndExecutionCycleAction_c::endType_ec::stop)
        {
            MACRO_ADDLOG("Try stop execution", metaEndExecutionCycleActionPtr_pri, messageType_ec::warning);
            if (metaEndExecutionCycleActionPtr_pri->actonDataHubParent_f() not_eq nullptr)
            {
                metaEndExecutionCycleActionPtr_pri->actonDataHubParent_f()->tryStopExecutingActions_f(metaEndExecutionCycleActionPtr_pri->killAfterTimeout_f());
            }
            break;
        }
        if (metaEndExecutionCycleActionPtr_pri->endType_f() == metaEndExecutionCycleAction_c::endType_ec::waitToFinish)
        {
            MACRO_ADDLOG("Stop after current execution cycle finishes", metaEndExecutionCycleActionPtr_pri, messageType_ec::warning);
            if (metaEndExecutionCycleActionPtr_pri->actonDataHubParent_f() not_eq nullptr)
            {
                metaEndExecutionCycleActionPtr_pri->actonDataHubParent_f()->stopWhenCurrentExecutionCycleFinishes_f();
            }
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


