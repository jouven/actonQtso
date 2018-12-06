#include "baseCheckExecution.hpp"

bool baseCheckExecution_c::stopping_f() const
{
    return stopping_pri;
}

void baseCheckExecution_c::stop_f()
{
    if (not stopping_pri)
    {
        stopping_pri = true;
        Q_EMIT executionStateChange_signal(checkExecutionState_ec::stoppingByUser);
    }
}

baseCheckExecution_c::baseCheckExecution_c()
{
    connect(this, &baseCheckExecution_c::anyCheckFinish_signal, this, &QObject::deleteLater);
}
