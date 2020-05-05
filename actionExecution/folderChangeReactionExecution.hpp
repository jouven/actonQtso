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
    QString filePath_pri;

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
    fileState_c(
            const QString& filePath_par_con
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
    bool firstCycle_pri = true;

    bool hashRequired_pri = false;

    bool anyChangeToReact_f(const fileState_c& fileStateObj_par_con) const;
    action_c* getActionForReaction_f(const QString& filePath_par_con);
    void monitoringScheduler_f();
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
            actionDataExecutionResult_c* actionExecutionResultObj_par_con
            , folderChangeReactionAction_c* folderChangeReactionActionPtr_par
    );
Q_SIGNALS:
    void monitoringGatherFilesFinished_signal();
private Q_SLOT:
    void monitoringGatherFiles_f();
    void monitoringReact_f();
    void monitoringCycleEnd_f();
    void monitoringCheckReactionPhaseEnded_f(action_c* action_par);
};

#endif // ACTONQTSO_FOLDERCHANGEREACTIONEXECUTION_HPP
