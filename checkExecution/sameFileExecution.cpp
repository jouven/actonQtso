#include "sameFileExecution.hpp"

#include "../actonDataHub.hpp"

#include <QFile>
#include <QFileInfo>

#include <string>

sameFileCheckExecution_c::sameFileCheckExecution_c(
        checkDataExecutionResult_c* checkExecutionResultObj_par_con
        , const sameFileCheck_c& samefileCheck_par_con)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
    , sameFileCheck_c(samefileCheck_par_con)
    , checkSameFile_pri(samefileCheck_par_con.fileAPath_f(), samefileCheck_par_con.fileBPath_f())
{
    QObject::connect(std::addressof(checkSameFile_pri), &checkSameFile_c::error_signal, this, &sameFileCheckExecution_c::addError_signal);
}

void sameFileCheckExecution_c::derivedExecute_f()
{
    Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
    bool resultTmp(checkSameFile_pri.checkIfSameFile_f());
    if (resultTmp)
    {
        Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedTrue);
    }
    else
    {
        Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedFalse);
    }
}

void sameFileCheckExecution_c::derivedStop_f()
{
    checkSameFile_pri.stop_f();
}


