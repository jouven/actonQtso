#include "sameFileExecution.hpp"

#include "../actonDataHub.hpp"

#include <QFile>
#include <QFileInfo>

#include <string>

//FUTURE make it setting
#define BUFFERSIZE 1024 * 32

sameFileCheckExecution_c::sameFileCheckExecution_c(
        checkDataExecutionResult_c* checkExecutionResultObj_par_con
        , const sameFileCheck_c& samefileCheck_par_con)
    : baseCheckExecution_c(checkExecutionResultObj_par_con)
    , sameFileCheck_c(samefileCheck_par_con)
{}

void sameFileCheckExecution_c::derivedExecute_f()
{
    bool resultTmp(false);
    bool resultSetTmp(false);
    while (true)
    {
        Q_EMIT executionStateChange_signal(checkExecutionState_ec::executing);
        QFileInfo fileInfoFileA(fileAPath_f());
        if (not fileInfoFileA.exists())
        {
            Q_EMIT addError_signal("File A doesn't exist: " + fileAPath_f());
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            break;
        }

        QFileInfo fileInfoFileB(fileBPath_f());
        if (not fileInfoFileB.exists())
        {
            Q_EMIT addError_signal("File B doesn't exist: " + fileBPath_f());
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            break;
        }

        if (fileInfoFileA.size() not_eq fileInfoFileB.size())
        {
            Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedFalse);
            break;
        }

        if (fileInfoFileA.size() == 0)
        {
            resultTmp = true;
            break;
        }

        QFile fileA(fileAPath_f());
        QFile fileB(fileBPath_f());

        if (fileA.open(QIODevice::ReadOnly))
        {
        }
        else
        {
            Q_EMIT addError_signal("Error when opening File A: " + fileAPath_f());
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            break;
        }

        if (fileB.open(QIODevice::ReadOnly))
        {
        }
        else
        {
            Q_EMIT addError_signal("Error when opening File B: " + fileBPath_f());
            //Q_EMIT executionStateChange_signal(checkExecutionState_ec::error);
            break;
        }


        //would use size_t but qt uses qint64 making it unsigned vs signed, compiler complains
        qint64 bufferSizeTmp(BUFFERSIZE);
        qint64 bytesReadTmpA(0);
        qint64 bytesReadTmpB(0);
        if (bufferSizeTmp > fileA.size())
        {
            bufferSizeTmp = fileA.size();
        }
        //FUTURE make buffer size a parameter
        std::string bufferA(bufferSizeTmp, '\0');
        std::string bufferB(bufferSizeTmp, '\0');

        bool sameBytesReadTmp(true);
        do
        {
            if (stopping_pri)
            {
                break;
            }
            bytesReadTmpA = fileA.read(&bufferA[0], bufferSizeTmp);
            bytesReadTmpB = fileB.read(&bufferB[0], bufferSizeTmp);
            if (fileB.bytesAvailable() not_eq fileA.bytesAvailable() or bytesReadTmpA not_eq bytesReadTmpB or bufferA not_eq bufferB)
            {
                sameBytesReadTmp = false;
                break;
            }
            if (fileA.bytesAvailable() < bufferSizeTmp)
            {
                bufferSizeTmp = fileA.bytesAvailable();
            }
        }
        while (fileA.bytesAvailable() > 0
               and bytesReadTmpA > 0
               //and bytesReadTmpA == bytesReadTmpB
               );

        resultTmp = sameBytesReadTmp;
        resultSetTmp = true;

        break;
    }

    if (stopping_pri and not resultSetTmp)
    {
        Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedFalse);
    }
    else
    {
        if (resultTmp)
        {
            Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedTrue);
        }
        else
        {
            Q_EMIT executionStateChange_signal(checkExecutionState_ec::finishedFalse);
        }
    }
}

void sameFileCheckExecution_c::derivedStop_f()
{
    stopping_pri = true;
}


