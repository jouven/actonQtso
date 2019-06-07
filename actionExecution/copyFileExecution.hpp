#ifndef ACTONQTSO_COPYFILEEXECUTION_HPP
#define ACTONQTSO_COPYFILEEXECUTION_HPP

#include "baseActionExecution.hpp"

#include <QMutexLocker>

class checkSameFile_c;
class copyFileAction_c;
class QFileInfo;

class copyFileActionExecution_c : public baseActionExecution_c
{
    Q_OBJECT

    //this case is not full const
    //because it has some interaction with the class,
    //when doing the filelist it can be interruped and there
    //is some state management happening
    copyFileAction_c* const copyFileActionPtr_pri = nullptr;

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
            , copyFileAction_c* copyFileActionPtr_par
    );
};

#endif // ACTONQTSO_COPYFILEEXECUTION_HPP
