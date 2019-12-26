#include "sameFileExecution.hpp"

#include "checks/sameFile.hpp"

#include <string>

sameFileCheckExecution_c::sameFileCheckExecution_c(
        checkDataExecutionResult_c* checkExecutionResultObj_par_con
        , sameFileCheck_c* samefileCheckPtr_par)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
    , sameFileCheckPtr_pri(samefileCheckPtr_par)
    , checkSameFile_pri(samefileCheckPtr_par->fileAPathParsed_f(), samefileCheckPtr_par->fileBPathParsed_f())
{
    QObject::connect(std::addressof(checkSameFile_pri), &checkSameFile_c::error_signal, this, &sameFileCheckExecution_c::addError_signal);
}

void sameFileCheckExecution_c::derivedExecute_f()
{
    //Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
    Q_EMIT anyFinish_signal(checkSameFile_pri.checkIfSameFile_f());
}

void sameFileCheckExecution_c::derivedStop_f()
{
    checkSameFile_pri.stop_f();
}


