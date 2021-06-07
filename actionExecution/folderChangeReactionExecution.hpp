#ifndef ACTONQTSO_FOLDERCHANGEREACTIONEXECUTION_HPP
#define ACTONQTSO_FOLDERCHANGEREACTIONEXECUTION_HPP

#include "baseActionExecution.hpp"

#include "textQtso/text.hpp"

#include <QString>
#include <QMap>

#include <vector>

class folderChangeReactionAction_c;

class fileState_c
{
    //the filepath + rootPath is to save memory, absolute path can still be used if rootPath is null,
    //because when using absolute file paths depending on the location might use a lot of extra space just to save the root part,
    //this way files are stored without the common root part in filePath and rootPath is a pointer to the same value
    //4 bytes vs whatever common absolute root path
    QString filePath_pri;
    const QString* rootPath_pri = nullptr;

    //current values
    //in bytes
    int_fast64_t currentFileSize_pri = 0;
    //epoch milliseconds
    int_fast64_t currentLastModificationDatetimeMs_pri = 0;
    //to check if two files are equal, only when the lastModificationDatetime has changed and the size is the same
    //probably xxhash
    uint_fast64_t currentHash_pri = 0;
    bool currentHashSet_pri = false;
    bool exists_pri = false;

    //old values
    int_fast64_t oldFileSize_pri = 0;
    int_fast64_t oldLastModificationDatetimeMs_pri = 0;
    uint_fast64_t oldHash_pri = 0;
    bool oldHashSet_pri = false;
    bool oldExists_pri = false;

    //because xxhash can use any possible int64 value, there is no value left to use as "there is/isn't a hash",
    //some extra bool fields are required
    bool requiresHash_pri = false;

    //because JSON de/serialization "will" happen
    //a default ctor will be required
    bool initialized_pri = false;

    //copyCurrentToOld_f initializes it
    //changed/decreased/deleted/new functions will return false always if it hasn't been
    bool oldIinitalized_pri = false;

    void updateCurrentValues_f();
    void copyCurrentToOld_f();
public:
    fileState_c() = default;
    fileState_c(const QString& filePath_par_con
            , const QString* rootPath_par
            , const bool requiresHash_par_con = false);

    void updateFileValues_f();

    int_fast64_t currentFileSize_f() const;
    int_fast64_t currentLastModificationDatetimeMs_f() const;
    uint_fast64_t currentHash_f() const;
    bool exists_f() const;
    uint_fast64_t oldFileSize_f() const;
    int_fast64_t oldLastModificationDatetimeMs_f() const;
    uint_fast64_t oldHash_f() const;
    bool oldExists_f() const;
    QString filePath_f() const;
    const QString* rootPath_f() const;
    //rootPath + / + filePath, checking if rootPath is nullptr and adding / if necessary
    QString fullFilePAth_f() const;

    //updateFileValues_f (which calls copyCurrentToOld_f) must be called once
    //otherwise all this fucntions bellow will return false
    bool sizeChanged_f() const;
    bool sizeIncreased_f() const;
    bool sizeDecreased_f() const;
    bool lastModificationDatetimeChanged_f() const;
    bool isNew_f() const;
    bool isDeleted_f() const;
    bool contentChanged_f() const;
};

class action_c;
class executionResult_c;
class directoryFilter_c;

class folderChangeReactionActionExecution_c : public baseActionExecution_c
{
    Q_OBJECT

    const folderChangeReactionAction_c* const folderChangeReactionActionPtr_pri = nullptr;

    int_fast64_t lastMonitoringStart_pri = 0;
    QMap<QString, fileState_c> monitoredFiles_pri;
    std::vector<QString> monitoredFilesSorted_pri;
    std::vector<QString> monitoredFilesToRemove_pri;
    std::vector<action_c*> executingReactionActions_pri;
    directoryFilter_c* directoryFilter_pri = nullptr;
    QString directoryRoot_pri;
    bool firstCycle_pri = true;

    bool hashRequired_pri = false;

    bool anyChangeToReact_f(const fileState_c& fileStateObj_par_con) const;
    action_c* getActionForReaction_f(const QString& filePath_par_con);
    void monitoringScheduler_f();
    void setupDirectoryFilter_f();
    void launchMonitoringGatherFilesThread_f();
    void executeReaction_f(const QString& monitoredFile_par_con, const bool sequentialReaction_par_con);
protected:
    void derivedExecute_f() override;
    void derivedStop_f() override;
    //does nothing
    void derivedKill_f() override;
public:
    folderChangeReactionActionExecution_c() = delete;
    explicit folderChangeReactionActionExecution_c(
            actionExecutionResult_c* actionExecutionResultObj_par_con
            , folderChangeReactionAction_c* folderChangeReactionActionPtr_par
    );
Q_SIGNALS:
    void monitoringGatherFilesFinished_signal();
private Q_SLOT:
    void monitoringGatherFiles_f();
    void monitoringReact_f();
    void monitoringCycleEnd_f();
    void monitoringCheckReactionPhaseEnded_f(executionResult_c* executionResult_par);

    //slots to add the errors from the cloned actions to the parent (this object) errors
    void addClonedError_f(action_c* action_ptr_par_con, const text_c error_par_con);
    void addClonedErrors_f(action_c* action_ptr_par_con, const textCompilation_c errors_par_con);
};

#endif // ACTONQTSO_FOLDERCHANGEREACTIONEXECUTION_HPP
