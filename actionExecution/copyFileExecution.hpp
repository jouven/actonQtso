#ifndef ACTONQTSO_COPYFILEEXECUTION_HPP
#define ACTONQTSO_COPYFILEEXECUTION_HPP

#include "baseActionExecution.hpp"

#include <QMutexLocker>

class checkSameFile_c;
class copyFileAction_c;
class textCompilation_c;
class directoryFilter_c;

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
    QMutex directoryFilteringMutex_pri;
    directoryFilter_c* directoryFilterPtr_pri = nullptr;
    checkSameFile_c* checkSameFile_ptr = nullptr;
    int_fast64_t diffPos_pri = 0;


    void executeSinglePath_f(const QString& path_par_con, textCompilation_c* errorsPtr_par = nullptr);
    bool sameFile_f(const QString& source_par_con, const QString& destination_par_con);

    void prepareCopyFile_f(
            const QFileInfo& sourceFileInfo_par_con
            , const QFileInfo& destinationFileInfo_par_con
            , textCompilation_c* errorsPtr_par = nullptr
    );
    void blockMove_f(
            const QString& source_par_con
            , const QString& destination_par_con
            , textCompilation_c* errorsPtr_par = nullptr
    );
    void blockMoveFiles_f(
            const QString& source_par_con
            , const QString& destination_par_con
            , textCompilation_c* errorsPtr_par = nullptr
    );
    void blockCopy_f(
            const QString& source_par_con
            , const QString& destination_par_con
            , textCompilation_c* errorsPtr_par = nullptr
    );

    bool pleaseStop_pri = false;

    //for the below 2 functions
    //pass a mutex if stopping the directory filtering is required from another thread
    //The mutex is "mandatory" to prevent the stopDirectoryFiltering_f function and the testSourceFileList_f function to
    //conflict, because stopDirectoryFiltering_f might try to access directoryFilterPtr_pri,
    //the object that does the filtering, when it's getting ctored or dtored

    //tries to generate a file list, used in copyFileActionExecution_c during execution and
    //in actonQtg copyFile editor to do dry runs of what will be copied
    std::vector<QString> testSourceFileList_f(const copyFileAction_c* const copyFileDataPtr_par
//            //this should be null because it will be asigned a value for the duration of the file list generation
//            , directoryFilter_c*& directoryFilterPtrRef_par
            , textCompilation_c* errors_ptr = nullptr);
    //to stop directoryFilter_c* filtering, no effect if it's not filtering or already stopping/ed
    void stopDirectoryFiltering_f();
    void stopSameFile_f();

protected:
    void derivedExecute_f() override;

    void derivedStop_f() override;
    //does nothing
    void derivedKill_f() override;
public:
    copyFileActionExecution_c() = delete;
    explicit copyFileActionExecution_c(
            actionExecutionResult_c* actionExecutionResultObj_par_con
            , copyFileAction_c* copyFileActionPtr_par
    );



private Q_SLOTS:
    void copyDirectory_f(std::vector<QString> result_par);
};

#endif // ACTONQTSO_COPYFILEEXECUTION_HPP
