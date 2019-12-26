#include "pathExistsExecution.hpp"

#include "checks/pathExists.hpp"

#include <QFileInfo>

pathExistsCheckExecution_c::pathExistsCheckExecution_c(
        checkDataExecutionResult_c* checkExecutionResultObj_par_con
        , pathExistsCheck_c* pathExistsCheckPtr_par)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
    , pathExistsCheckPtr_pri(pathExistsCheckPtr_par)
{
}

void pathExistsCheckExecution_c::derivedExecute_f()
{
    bool existsTmp(false);
    Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
    QFileInfo fileInfoPathTmp(pathExistsCheckPtr_pri->pathParsed_f());
    existsTmp = fileInfoPathTmp.exists();
    Q_EMIT anyFinish_signal(existsTmp);
}

void pathExistsCheckExecution_c::derivedStop_f()
{
    //no need
}


