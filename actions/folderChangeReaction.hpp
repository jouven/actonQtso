#ifndef ACTONQTSO_FOLDERCHANGEREACTION_HPP
#define ACTONQTSO_FOLDERCHANGEREACTION_HPP

#include "../actionData.hpp"

#include "../crossPlatformMacros.hpp"

#include <QString>
#include <QMap>

#include <unordered_set>
#include <unordered_map>

class QJsonObject;
class textCompilation_c;

//this action requires a parent actonDataHub_c
//it won't work in the void and it requires the existence of other actions, being in actonDataHub_c, to work
//for the reasons above it "can't" be excuted "alone"

class EXPIMP_ACTONQTSO folderChangeReactionData_c
{
public:
    //aaa~~~ it can get pretty complex with the endless combinations of what can change and what can be done when something changes...
    //plus the combinations of changes or multiple reactions...

    //folders are ignored as elements to monitor, if a folder is created or removed or renamed or w/e
    //they will be navigated to search files see
    enum class changeType_ec
    {
        empty = 0
        , fileCreated = 1
        , fileModifiedDateTimeChanged = 2
        , fileRemoved = 3
        //add another enum to how what type of file change
        , fileSizeChanged = 4
        //worst case scenario it means reading the full file, but it won't check this if fileModifiedDateTimeChanged isn't triggered first/also
        , fileContentsChanged = 5
    };

    static EXPIMP_ACTONQTSO const QMap<QString, changeType_ec> strToChangeTypeMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<changeType_ec, QString> changeTypeToStrUMap_sta_con;

    enum class fileSizeChangeType_ec
    {
        empty = 0
        , any = 1
        , anyIncrease = 2
        , anyDecrease = 3
        //i.e. 0 bytes
        , fixedValue = 4
        //use positive values for "larger than" and negative for "smaller than"?
        , threshold = 5
    };

    static EXPIMP_ACTONQTSO const QMap<QString, fileSizeChangeType_ec> strToFileSizeChangeTypeMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<fileSizeChangeType_ec, QString> fileSizeChangeTypeToStrUMap_sta_con;

    enum class reactionType_ec
    {
        empty = 0
        , execute = 1
        , runProcess = 2
        , copyMove = 3
        //, move = 4
        , remove = 5
        //, playSound = 6
    };

    static EXPIMP_ACTONQTSO const QMap<QString, folderChangeReactionData_c::reactionType_ec> strToReactionTypeMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<folderChangeReactionData_c::reactionType_ec, QString> reactionTypeToStrUMap_sta_con;

    enum class reactionOrder_ec
    {
        empty = 0
        //file datetimeModification
        , newestFirst = 1
        //file size
        , smallerFirst = 2
        //utf-8 character wise, as in ascii first then the more complex characters also known? as numerical or "C"
        , alphabetical = 3
    };

    static EXPIMP_ACTONQTSO const QMap<QString, folderChangeReactionData_c::reactionOrder_ec> strToReactionOrderMap_sta_con;
    static EXPIMP_ACTONQTSO const std::unordered_map<folderChangeReactionData_c::reactionOrder_ec, QString> reactionOrderToStrUMap_sta_con;
protected:
    QString folderPath_pro;

    //recurse folders inside the folder being monitored (folderPath_pro)
    bool recurseFolders_pro = false;
    //monitored files will be stored/passed using absolute paths
    bool useAbsolutePaths_pro = false;

    //if a file fulfills one-any of the changes it will trigger a "reaction"
    std::unordered_set<changeType_ec> changesToMonitor_pro;

    //general types overwritemore exclusive ones
    //std::unordered_set<fileSizeChangeType_ec> fileSizeChangeTypesToMonitor_pro;
    fileSizeChangeType_ec fileSizeChangeTypeToMonitor_pro = fileSizeChangeType_ec::any;

    //both in bytes
    //used with fileSizeChangeType_ec::fixedValue, if a file size changes to this exact value it triggers a reaction
    int_fast64_t fileSizeChangeFixedValue_pro = 0;
    //positive values work like an upper limit, i.e. 1024 will trigger reactions on files bigger than 1kb
    //negative values work like a lower limit, i.e. -1024 will trigger reactions on files smaller than 1kb
    //0 works like a positive value, i.e. will react to non-empty files
    int_fast64_t fileSizeChangeThreshold_pro = 0;
    //FUTURE do it like a From-To? ("To" optional) with an invert option?

    //QLockFile
    //bool ignoreLockedFiles_pri = false;

    //just allow one reaction for now, anyway the "callCommandWithArgument" option can "workaround" the "one reaction limit"
    reactionType_ec reactionType_pro = reactionType_ec::runProcess;

    //when reaction type is callCommand replace an argument placeholder value with
    //the file path of the file being reacted
    QString argumentPlaceHolderToReplaceWithChangedFilePath_pro;
    //

    //when reacting to changes do it in a sequential way or multithread,
    //multithreading on files of a same device can slow down I/O speed of that device
    bool sequentialReaction_pro = true;

    reactionOrder_ec reactionOrder_pro = reactionOrder_ec::alphabetical;
    //reverses the order, i.e. newestFirst turns into oldestFirst
    bool reverseReactionOrder_pro = false;

    //interval in milliseconds to check for changes
    //if reacting takes more time than the interval time the next one "monitoring cycle"
    //will be delayed until the current finishes
    uint_fast64_t monitorIntervalMs_pro = 10000;

    //FUTURE
    //generate a json file with the folder status
    //which files contains the folder and
    //for each file, size, lastmodificationdatetime...
    //bool saveFolderState_pro = false;

    //action string Id of the action that will be used-modified to do the reactions
    //the action must be disabled to be eligible
    QString reactionActionStringId_pro;

    //prevent public assignments
    folderChangeReactionData_c& operator=(folderChangeReactionData_c const &) = default;
    folderChangeReactionData_c& operator=(folderChangeReactionData_c&) = default;
    folderChangeReactionData_c& operator=(folderChangeReactionData_c&&) = default;
public:
    //declaring move ctor/operators removes non-explicit default copy ctors
    //see https://stackoverflow.com/questions/11255027
    //so... explicit them
    folderChangeReactionData_c(folderChangeReactionData_c const&) = default;
    folderChangeReactionData_c(folderChangeReactionData_c&) = default;

    folderChangeReactionData_c() = default;
    explicit folderChangeReactionData_c(
            const QString& folderPath_par_con
            , const bool recurseFolders_par_con
            , const bool useAbsolutePaths_par_con
            , const std::unordered_set<changeType_ec>& changesToMonitor_par_con
            //changesToMonitor_par_con must contain changeType_ec::fileSizeChanged for the 3 arguments below to take effect
            , const fileSizeChangeType_ec fileSizeChangeTypeToMonitor_par_con
            //only when fileSizeChangeType_ec::fixedValue
            , const uint_fast64_t fileSizeChangeFixedValue_par_con
            //only when fileSizeChangeType_ec::threshold
            , const int_fast64_t fileSizeChangeThreshold_par_con
            , const reactionType_ec reactionType_par_con
            , const QString& argumentPlaceHolderToReplaceWithChangedFilePath_par_con
            , const bool sequentialReaction_par_con
            , const reactionOrder_ec reactionOrder_par_con
            , const bool reverseReactionOrder_par_con
            , const uint_fast64_t monitorIntervalMs_par_con
            , const QString& reactionActionStringId_par_con
    );

    bool isFieldsDataValid_f(textCompilation_c* errorsPtr_par = nullptr) const;

    QString folderPath_f() const;

    void setFolderPath_f(const QString& folderPath_par_con);
    std::unordered_set<changeType_ec> changesToMonitor_f() const;
    void setChangesToMonitor_pri(const std::unordered_set<changeType_ec>& changesToMonitor_par_con);
//    std::unordered_set<fileSizeChangeType_ec> fileSizeChangeTypesToMonitor_f() const;
//    void setFileSizeChangeTypesToMonitor_pri(const std::unordered_set<fileSizeChangeType_ec>& fileSizeChangeTypesToMonitor_par_con);
    int_fast64_t fileSizeChangeFixedValue_f() const;
    void setFileSizeChangeFixedValue_f(const int_fast64_t& fileSizeChangeFixedValue_par_con);
    int_fast64_t fileSizeChangeThreshold_f() const;
    void setFileSizeChangeThreshold_pri(const int_fast64_t& fileSizeChangeThreshold_par_con);
    reactionType_ec reactionType_f() const;
    void setReactionType_pri(const reactionType_ec& reactionType_par_con);
    bool sequentialReaction_f() const;
    void setSequentialReaction_pri(const bool sequentialReaction_par_con);
    reactionOrder_ec reactionOrder_f() const;
    void setReactionOrder_pri(const reactionOrder_ec& reactionOrder_par_con);
    uint_fast64_t monitorIntervalMs_f() const;
    void setMonitorIntervalMs_f(const uint_fast64_t& monitorIntervalMs_par_con);
    fileSizeChangeType_ec fileSizeChangeTypeToMonitor_f() const;
    void setFileSizeChangeTypeToMonitor_f(const fileSizeChangeType_ec& fileSizeChangeTypeToMonitor_par_con);
    bool recurseFolders_f() const;
    void setRecurseFolders_f(const bool recurseFolders_par_con);
    bool useAbsolutePaths_f() const;
    void setUseAbsolutePaths_f(const bool useAbsolutePaths_par_con);

    QString argumentPlaceHolderToReplaceWithChangedFilePath_f() const;
    void setArgumentPlaceHolderToReplaceWithChangedFilePath_f(const QString& argumentPlaceHolderToReplaceWithChangedFilePath_par_con);
    QString reactionActionStringId_f() const;
    //QString reactionActionStringIdParsed_f() const;
    void setReactionActionStringId_f(const QString& reactionActionStringId_par_con);
    bool reverseReactionOrder_f() const;
    void setReverseReactionOrder_f(const bool reverseReactionOrder_par_con);

    static bool isValidActionForReaction_f(const action_c* const action_par, const reactionType_ec reactionType_par_con);
};

class EXPIMP_ACTONQTSO folderChangeReactionAction_c : public action_c, public folderChangeReactionData_c
{
    Q_OBJECT

    void derivedWrite_f(QJsonObject &json_par) const override;
    void derivedRead_f(const QJsonObject &json_par_con) override;
    bool derivedIsValidAction_f(textCompilation_c* errors_par = nullptr) const override;

    uint_fast64_t derivedUpdateActionStringIdDependencies_f(const QString& oldActionStringId_par_con, const QString& newActionStringId_par_con) override;
    uint_fast64_t derivedActionStringIdDependencyCount_f(const QString& actionStringId_par_con) const override;
    //uint_fast64_t derivedStringTriggerCreationConflictCount_f(const QString& ) const override;
    uint_fast64_t derivedUpdateStringTriggerDependecies_f(const QString& oldStringTrigger_par_con, const QString& newStringTrigger_par_con) override;
    uint_fast64_t derivedStringTriggerDependencyCount_f(const QString& stringTrigger_par_con) const override;
    //QSet<QString> derivedStringTriggerCreationCollection_f() const override;
    QSet<QString> derivedStringTriggersInUse_f(const QSet<QString>& searchValues_par_con) const override;

    action_c* derivedClone_f() const override;

    baseActionExecution_c* createExecutionObj_f(actionExecutionResult_c* actionDataExecutionResult_ptr_par) override;
    actionType_ec type_f() const override;
    //QString typeStr_f() const override;

    QString derivedReference_f() const override;
public:
    folderChangeReactionAction_c() = default;
    folderChangeReactionAction_c(actonDataHub_c* parent_par, const actionData_c& actionData_par_con, const folderChangeReactionData_c& folderChangeReactionData_par_con);

    QString folderPathParsed_f() const;
    QString argumentPlaceHolderToReplaceWithChangedFilePathParsed_f() const;

    void updateFolderChangeReactionData_f(const folderChangeReactionData_c& folderChangeReactionData_par_con);
};

#endif // ACTONQTSO_FOLDERCHANGEREACTION_HPP
