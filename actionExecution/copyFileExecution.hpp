#ifndef ACTONQTSO_COPYFILEEXECUTION_HPP
#define ACTONQTSO_COPYFILEEXECUTION_HPP

#include "baseActionExecution.hpp"

#include "../actions/copyFile.hpp"

#include <QMutexLocker>
#include <QFileInfo>

class checkSameFile_c;

class copyFileActionExecution_c : public baseActionExecution_c, public copyFileAction_c
{
    Q_OBJECT

    QMutex checkSameFileMutex_pri;
    checkSameFile_c* checkSameFile_ptr = nullptr;
    int_fast64_t diffPos_pri = 0;

    bool sameFile_f(const QString& source_par_con, const QString& destination_par_con);

    void prepareCopyFile_f(
            const QFileInfo& sourceFileInfo_par_con
            , const QFileInfo& destinationFileInfo_par_con
            , QString* errorStrPtr_par = nullptr
    );
    void blockMove_f(
            const QString& source_par_con
            , const QString& destination_par_con
            , QString* errorStrPtr_par = nullptr
    );
    void blockCopy_f(
            const QString& source_par_con
            , const QString& destination_par_con
            , QString* errorStrPtr_par = nullptr
    );

    bool pleaseStop_pri = false;
protected:
    void derivedExecute_f() override;

    void derivedStop_f() override;
    //does nothing
    void derivedKill_f() override;
public:
    copyFileActionExecution_c() = delete;
    explicit copyFileActionExecution_c(
            actionDataExecutionResult_c* actionExecutionResultObj_par_con
            , const copyFileAction_c& copyFileAction_par_con
    );
};

#endif // ACTONQTSO_COPYFILEEXECUTION_HPP
