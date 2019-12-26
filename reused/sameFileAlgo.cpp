#include "sameFileAlgo.hpp"

#include "comuso/practicalTemplates.hpp"

#include "textQtso/text.hpp"

#include <QFileInfo>
#include <QFile>

QString checkSameFile_c::fileB_f() const
{
    return fileB_pri;
}

void checkSameFile_c::setFileB_f(const QString& fileB_par_con)
{
    if (equalOnce_ft(currentState_f(), state_ec::initial, state_ec::finished))
    {
        fileB_pri = fileB_par_con;
    }
}

bool checkSameFile_c::checkIfSameFile_f()
{
    bool resultTmp(false);
    while (true)
    {
        stateQueue_pri.emplace_back(state_ec::running);
        indexWhereFilesAreDifferent_pri = 0;
        Q_EMIT running_signal();
        QFileInfo fileInfoFileA(fileA_pri);
        if (not fileInfoFileA.exists())
        {
            Q_EMIT error_signal({"File A doesn't exist: {0}", fileA_pri});
            break;
        }

        QFileInfo fileInfoFileB(fileB_pri);
        if (not fileInfoFileB.exists())
        {
            Q_EMIT error_signal({"File B doesn't exist: {0}", fileB_pri});
            break;
        }

        if (fileInfoFileA.size() not_eq fileInfoFileB.size())
        {
            resultTmp = false;
            break;
        }

        if (fileInfoFileA.size() == 0)
        {
            resultTmp = true;
            break;
        }

        QFile fileA(fileA_pri);
        QFile fileB(fileB_pri);

        if (fileA.open(QIODevice::ReadOnly))
        {
        }
        else
        {
            Q_EMIT error_signal({"Error when opening File A: {0}", fileA_pri});
            break;
        }

        if (fileB.open(QIODevice::ReadOnly))
        {
        }
        else
        {
            Q_EMIT error_signal({"Error when opening File B: {0}", fileB_pri});
            break;
        }


        //would use size_t but qt uses qint64 making it unsigned vs signed, compiler complains
        qint64 bytesReadTmpA(0);
        qint64 bytesReadTmpB(0);
        if (bufferSize_pri > fileA.size())
        {
            bufferSize_pri = fileA.size();
        }

        std::string bufferA(bufferSize_pri, '\0');
        std::string bufferB(bufferSize_pri, '\0');

        bool sameBytesReadTmp(true);
        do
        {
            if (pleaseStop_pri)
            {
                break;
            }
            bytesReadTmpA = fileA.read(&bufferA[0], bufferSize_pri);
            bytesReadTmpB = fileB.read(&bufferB[0], bufferSize_pri);
            if (bytesReadTmpA == -1)
            {
                Q_EMIT error_signal({"Error reading File A: {0}", fileA_pri});
                break;
            }
            if (bytesReadTmpB == -1)
            {
                Q_EMIT error_signal({"Error reading File B {0}", fileB_pri});
                break;
            }
            if (fileB.bytesAvailable() not_eq fileA.bytesAvailable()
                or bytesReadTmpA not_eq bytesReadTmpB
                or bufferA not_eq bufferB)
            {
                sameBytesReadTmp = false;
                break;
            }
            else
            {
                indexWhereFilesAreDifferent_pri = indexWhereFilesAreDifferent_pri + bytesReadTmpA;
            }
            if (fileA.bytesAvailable() < bufferSize_pri)
            {
                bufferSize_pri = fileA.bytesAvailable();
            }
        }
        while (fileA.bytesAvailable() > 0
               and bytesReadTmpA > 0
               //and bytesReadTmpA == bytesReadTmpB
               );

        if (currentState_f() == state_ec::error)
        {
            break;
        }
        resultTmp = sameBytesReadTmp and (fileA.bytesAvailable() == 0);
        if (pleaseStop_pri)
        {
            pleaseStop_pri = false;
            stateQueue_pri.emplace_back(state_ec::stopped);
            Q_EMIT stopped_signal();
        }
        else
        {
            stateQueue_pri.emplace_back(state_ec::finished);
            Q_EMIT finished_signal();
        }
        break;
    }

    return resultTmp;
}

std::vector<checkSameFile_c::state_ec> checkSameFile_c::stateQueue_f() const
{
    return stateQueue_pri;
}

checkSameFile_c::state_ec checkSameFile_c::currentState_f() const
{
    return stateQueue_pri.back();
}

void checkSameFile_c::stop_f()
{
    if (pleaseStop_pri)
    {

    }
    else
    {
        pleaseStop_pri = true;
    }
}

void checkSameFile_c::updateToErrorState_f()
{
    stateQueue_pri.emplace_back(state_ec::error);
}

int_fast64_t checkSameFile_c::bufferSize_f() const
{
    return bufferSize_pri;
}

void checkSameFile_c::setBufferSize_f(const int_fast64_t& bufferSize_par_con)
{
    if (equalOnce_ft(currentState_f(), state_ec::initial, state_ec::finished))
    {
        bufferSize_pri = bufferSize_par_con;
    }
}

int_fast64_t checkSameFile_c::indexWhereFilesAreDifferent_f() const
{
    return indexWhereFilesAreDifferent_pri;
}

checkSameFile_c::checkSameFile_c(
        const QString& fileA_par_con
        , const QString& fileB_par_con
        )
    :
      fileA_pri(fileA_par_con)
    , fileB_pri(fileB_par_con)
{
    QObject::connect(this, &checkSameFile_c::error_signal, this, &checkSameFile_c::updateToErrorState_f);
}

QString checkSameFile_c::fileA_f() const
{
    return fileA_pri;
}

void checkSameFile_c::setFileA_f(const QString& fileA_par_con)
{
    if (equalOnce_ft(currentState_f(), state_ec::initial, state_ec::finished))
    {
        fileA_pri = fileA_par_con;
    }
}
