#include "baseActionExecution.hpp"

baseActionExecution_c::baseActionExecution_c()
{
    connect(this, &baseActionExecution_c::anyFinish_signal, this, &QObject::deleteLater);
}
